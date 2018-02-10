#include <QtGlobal> // qWarning, qCritical
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSettings>
#include <QFile>
#include <QUrl>
#include "UploadManager.h"

using namespace QKeychain;

UploadManager::UploadManager(QObject *parent) :
    QObject(parent),
    networkAccessManager(new QNetworkAccessManager(this)),
    maxActiveUploadCount(1),
    activeUploadCount(0)
{
}

UploadManager::~UploadManager()
{
}

void UploadManager::setupPasswordJob(Job *job)
{
    QSettings *settings = new QSettings(job);
    settings->beginGroup("upload");
    settings->beginGroup("password");

    Q_ASSERT(!_userName.isEmpty());
    job->setKey(_userName);
    job->setInsecureFallback(true);
    job->setSettings(settings); // used for insecure fallback
}

void UploadManager::writePassword()
{
    Q_ASSERT(!_serviceUrl.isEmpty());
    Job *job;
    if(_password.isEmpty())
    {
        DeletePasswordJob *deleteJob = new DeletePasswordJob(_serviceUrl.toLatin1());
        job = deleteJob;
    }
    else
    {
        WritePasswordJob *writeJob = new WritePasswordJob(_serviceUrl.toLatin1());
        writeJob->setTextData(_password);
        job = writeJob;
    }

    job->setAutoDelete(true);
    setupPasswordJob(job);

    connect(job, &Job::finished,
            this, &UploadManager::writePasswordFinished);

    job->start();
}

void UploadManager::writePasswordFinished(Job *job)
{
    if(job->error())
        qCritical("Writing password failed: %s",
                  qUtf8Printable(job->errorString()));
}

void UploadManager::readPassword()
{
    Q_ASSERT(!_serviceUrl.isEmpty());
    ReadPasswordJob *job = new ReadPasswordJob(_serviceUrl.toLatin1());
    job->setAutoDelete(true);
    setupPasswordJob(job);

    connect(job, &Job::finished,
            this, &UploadManager::readPasswordFinished);

    job->start();
}

void UploadManager::readPasswordFinished(Job *_job)
{
    ReadPasswordJob *job = dynamic_cast<ReadPasswordJob*>(_job);

    if(job->error())
    {
        qCritical("Reading password failed: %s",
                  qUtf8Printable(job->errorString()));
        return;
    }

    _password = job->textData();
    emit passwordChanged(password());
}

void UploadManager::readSettings(QSettings &settings)
{
    setServiceUrl(settings.value("serviceUrl").value<QString>());
    setUserName(settings.value("userName").value<QString>());

    emit serviceUrlChanged(serviceUrl());
    emit userNameChanged(userName());

    readPassword();
}

void UploadManager::writeSettings(QSettings &settings)
{
    settings.setValue("serviceUrl", serviceUrl());
    settings.setValue("userName", userName());

    //writePassword(); // is done in setPassword already
}


// ---- Getters ----

const QString &UploadManager::serviceUrl() const
{
    return _serviceUrl;
}

const QString &UploadManager::userName() const
{
    return _userName;
}

const QString &UploadManager::password() const
{
    return _password;
}


// ---- Setters ----

void UploadManager::setServiceUrl(const QString &url)
{
    _serviceUrl = url;
}

void UploadManager::setUserName(const QString &name)
{
    _userName = name;
}

void UploadManager::setPassword(const QString &pw)
{
    _password = pw;
    writePassword();
}

void UploadManager::enqueueUpload(Upload *upload)
{
    queue.enqueue(upload);
    upload->setParent(this);
    emit uploadEnqueued(upload);
    tryStartUpload();
}

static void disableCertificateVerification(QNetworkRequest *request)
{
    QSslConfiguration conf = request->sslConfiguration();
    conf.setPeerVerifyMode(QSslSocket::VerifyNone);
    request->setSslConfiguration(conf);
}

void UploadManager::startUpload(Upload *upload)
{
    Q_ASSERT(activeUploadCount < maxActiveUploadCount);

    QFile *file = upload->file();
    Q_ASSERT(file->exists());
    Q_ASSERT(!file->isOpen());
    file->open(QIODevice::ReadOnly);

    const QString requestUrl = QString("%1/upload?category=%2")
        .arg(_serviceUrl)
        .arg(upload->category());
    qWarning(qUtf8Printable(requestUrl));

    const QString userInfo = QString("%1:%2")
        .arg(_userName)
        .arg(_password);
    QByteArray authorization("Basic ");
    authorization.append(userInfo.toUtf8().toBase64());

    QNetworkRequest request(requestUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, upload->mimeType());
    request.setHeader(QNetworkRequest::ContentLengthHeader, file->size());
    request.setRawHeader(QByteArray("Authorization"), authorization);
    disableCertificateVerification(&request);

    QNetworkReply *reply = networkAccessManager->post(request, file);

    connect(upload, &Upload::stateChanged,
            this, &UploadManager::uploadStateChanged);

    activeUploadCount++;

    upload->started(reply);
}

void UploadManager::tryStartUpload()
{
    if(activeUploadCount >= maxActiveUploadCount)
        return;

    if(queue.isEmpty())
        return;

    startUpload(queue.dequeue());
}

void UploadManager::uploadStateChanged(Upload::State state)
{
    if(state == Upload::Completed ||
       state == Upload::Failed)
    {
        activeUploadCount--;
        Q_ASSERT(activeUploadCount >= 0);
        tryStartUpload();
    }
}
