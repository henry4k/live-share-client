#ifndef UPLOADMANAGER_H
#define UPLOADMANAGER_H

#include <QString>
#include <QQueue>
#include <QObject>
#include "Upload.h"

class QNetworkAccessManager;
class QSettings;
class QFile;


class UploadManager : public QObject
{
    Q_OBJECT

public:
    UploadManager(QObject *parent = nullptr);
    ~UploadManager();

    void readSettings(QSettings &settings);
    void writeSettings(QSettings &settings);

    const QString &serviceUrl() const;
    const QString &userName() const;

signals:
    void serviceUrlChanged(const QString &url);
    void userNameChanged(const QString &name);
    void uploadEnqueued(Upload *upload);

public slots:
    void setServiceUrl(const QString &url);
    void setUserName(const QString &name);
    void enqueueUpload(Upload *upload);

private slots:
    void uploadStateChanged(Upload::State state);

private:
    void tryStartUpload();
    void startUpload(Upload *upload);

    QString _serviceUrl;
    QString _userName;
    QQueue<Upload*> queue;
    QNetworkAccessManager *networkAccessManager;
    int maxActiveUploadCount;
    int activeUploadCount;
};

#endif
