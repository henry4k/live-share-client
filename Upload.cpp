#include <QFile>
#include <QNetworkReply>
#include "Upload.h"

Upload::Upload(QFile *file,
               const QString &mimeType,
               const QString &category,
               QObject *parent) :
    QObject(parent),
    _file(file),
    _mimeType(mimeType),
    _category(category),
    _state(Ready),
    _sentBytes(0),
    networkReply(nullptr)
{
    Q_ASSERT(file->exists());
    Q_ASSERT(!file->isOpen());

    file->setParent(this);

    _totalBytes = file->size();
}

Upload::~Upload()
{
    if(networkReply)
    {
        qWarning("Upload: There is still a network reply here.");
        networkReply->deleteLater();
    }
}

QFile *Upload::file() const
{
    return _file;
}

const QString &Upload::mimeType() const
{
    return _mimeType;
}

const QString &Upload::category() const
{
    return _category;
}

const Upload::State Upload::state() const
{
    return _state;
}

const qint64 Upload::totalBytes() const
{
    return _totalBytes;
}

const qint64 Upload::sentBytes() const
{
    return _sentBytes;
}

void Upload::started(QNetworkReply *reply)
{
    Q_ASSERT(_state == Ready);
    Q_ASSERT(!networkReply);

    networkReply = reply;

    connect(reply, &QNetworkReply::uploadProgress,
            this, &Upload::progress);
    connect(reply, &QNetworkReply::finished,
            this, &Upload::finished);

    _state = Active;
    emit stateChanged(_state);
}

void Upload::abort()
{
    Q_ASSERT(networkReply);
    networkReply->abort();
}

void Upload::progress(qint64 sent, qint64 total)
{
    _totalBytes = total; // TODO: Is this ok?
    _sentBytes = sent;
}

void Upload::finished()
{
    _state = Completed;
    emit stateChanged(_state);

    networkReply->deleteLater();
    networkReply = nullptr;
}
