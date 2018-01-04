#include <QtGlobal> // qWarning
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSettings>
#include <QFile>
#include "UploadManager.h"


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

void UploadManager::readSettings(QSettings &settings)
{
    setServiceUrl(settings.value("serviceUrl").value<QString>());
    setUserName(settings.value("userName").value<QString>());

    emit serviceUrlChanged(serviceUrl());
    emit userNameChanged(userName());
}

void UploadManager::writeSettings(QSettings &settings)
{
    settings.setValue("serviceUrl", serviceUrl());
    settings.setValue("username", userName());
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


// ---- Setters ----

void UploadManager::setServiceUrl(const QString &url)
{
    _serviceUrl = url;
}

void UploadManager::setUserName(const QString &name)
{
    _userName = name;
}

void UploadManager::enqueueUpload(Upload *upload)
{
    queue.enqueue(upload);
    upload->setParent(this);
    emit uploadEnqueued(upload);
    tryStartUpload();
}

void UploadManager::startUpload(Upload *upload)
{
    Q_ASSERT(activeUploadCount < maxActiveUploadCount);

    QFile *file = upload->file();
    Q_ASSERT(file->exists());
    Q_ASSERT(!file->isOpen());
    file->open(QIODevice::ReadOnly);

    const QString requestUrl = _serviceUrl + "/upload";

    QNetworkRequest request(requestUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, upload->mimeType());
    // TODO: userName
    // TODO: category

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
    if(state != Upload::Active)
    {
        activeUploadCount--;
        Q_ASSERT(activeUploadCount >= 0);
        tryStartUpload();
    }
}
