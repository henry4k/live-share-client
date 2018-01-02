#ifndef RECORDER_H
#define RECORDER_H

#include <QString>
#include <QStringList>
#include <QObject>
#include <QProcess>
#include <QFile>
#include "WindowInfo.h"

class QIODevice;

class Recorder : public QObject
{
    Q_OBJECT

//    Q_PROPERTY(QString ffmpegLocation
//               WRITE setFfmpegLocation)
//
//    Q_PROPERY(WindowInfo windowInfo
//              WRITE setWindowInfo)
//
//    Q_PROPERY(int maxEdgeLength
//              WRITE setMaxEdgeLength)
//
//    Q_PROPERY(float frameRate
//              WRITE setFrameRate)
//
//    Q_PROPERTY(RecordType recordType
//               READ recordType
//               WRITE setRecordType)
//
//    Q_PROPERTY(QString mimeType
//               READ mimeType)

public:
    enum RecordType
    {
        Image,
        Video
    };

    explicit Recorder(QObject *parent = Q_NULLPTR);
    ~Recorder();

    void setFfmpegLocation(const QString &location);
    void setWindowInfo(const WindowInfo &info);
    void setMaxEdgeLength(int length);
    void setFrameRate(float rate);
    void setRecordType(RecordType type);

    RecordType recordType() const;
    QString mimeType() const;

    void start();
    void stop();
    bool isRecording() const;

signals:
    void finished(QFile *outputFile);

private slots:
    void process_errorOccurred(QProcess::ProcessError error);
    void process_finished(int exitCode, QProcess::ExitStatus exitStatus);

private:
    bool appendInputArguments(QStringList &arguments);
    bool appendOutputArguments(QStringList &arguments);
    void getOutputSize(int *outputWidth, int *outputHeight);
    bool appendOutputSizeArguments(QStringList &arguments);
    bool ownsOutputFile() const;
    void removeOutputFile();
    void createOutputFile();

    WindowInfo windowInfo;
    int maxEdgeLength;
    float frameRate;
    RecordType _recordType;
    QProcess *process;
    QFile *outputFile;
};

#endif
