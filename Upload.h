#ifndef UPLOAD_H
#define UPLOAD_H

#include <QString>
#include <QObject>

class QFile;
class QNetworkReply;


class Upload : public QObject
{
    Q_OBJECT

public:
    enum State
    {
        Ready,
        Active,
        Completed
    };

    Upload(QFile *file,
           const QString &mimeType,
           const QString &category,
           QObject *parent = nullptr);
    ~Upload();

    QFile *file() const;
    const QString &mimeType() const;
    const QString &category() const;
    const State state() const;
    const qint64 totalBytes() const;
    const qint64 sentBytes() const;

signals:
    void stateChanged(State state);
    void sentBytesChanged(qint64 bytes);

public slots:
    void started(QNetworkReply *reply);
    void abort(); // TODO

private slots:
    void progress(qint64 sent, qint64 total);
    void finished();

private:
    QFile *_file;
    QString _mimeType;
    QString _category;
    State _state;
    qint64 _totalBytes;
    qint64 _sentBytes;
    QNetworkReply *networkReply;
};

#endif
