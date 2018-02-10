#ifndef UPLOADMANAGER_H
#define UPLOADMANAGER_H

#include <QString>
#include <QQueue>
#include <QObject>
#include <keychain.h>
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
    const QString &password() const;

signals:
    void serviceUrlChanged(const QString &url);
    void userNameChanged(const QString &name);
    void passwordChanged(const QString &pw);
    void uploadEnqueued(Upload *upload);

public slots:
    void setServiceUrl(const QString &url);
    void setUserName(const QString &name);
    void setPassword(const QString &pw);
    void enqueueUpload(Upload *upload);

private slots:
    void uploadStateChanged(Upload::State state);
    void readPasswordFinished(QKeychain::Job *job);
    void writePasswordFinished(QKeychain::Job *job);

private:
    void setupPasswordJob(QKeychain::Job *job);
    void readPassword();
    void writePassword();
    void tryStartUpload();
    void startUpload(Upload *upload);

    QString _serviceUrl;
    QString _userName;
    QString _password;
    QQueue<Upload*> queue;
    QNetworkAccessManager *networkAccessManager;
    int maxActiveUploadCount;
    int activeUploadCount;
};

#endif
