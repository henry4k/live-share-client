#ifndef RECORDER_H
#define RECORDER_H

#include <QString>
#include <QObject>
#include <QProcess>
#include "WindowInfo.h"

class QFile;
class QStringList;
class QIODevice;
class QSettings;

class Recorder : public QObject
{
    Q_OBJECT

//    Q_PROPERTY(QString ffmpegExecutable
//               WRITE setFfmpegExecutable)
//
//    Q_PROPERY(WindowInfo windowInfo
//              WRITE setWindowInfo)
//
//    Q_PROPERY(int maxEdgeLength
//              WRITE setMaxEdgeLength)
//
//    Q_PROPERY(double videoFrameRate
//              WRITE setVideoFrameRate)
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

    explicit Recorder(QObject *parent = nullptr);
    ~Recorder();

    void readSettings(QSettings &settings);
    void writeSettings(QSettings &settings);

    QString ffmpegExecutable() const;
    int maxImageEdgeLength() const;
    int maxVideoEdgeLength() const;
    double videoFrameRate() const;
    int maxVideoLength() const;
    RecordType recordType() const;
    QString mimeType() const;

    void start();
    void stop();
    bool isRecording() const;

signals:
    void finished(QFile *outputFile);

    void ffmpegExecutableChanged(const QString &executable);
    void maxImageEdgeLengthChanged(int length);
    void maxVideoEdgeLengthChanged(int length);
    void videoFrameRateChanged(double rate);
    void maxVideoLengthChanged(int length);

public slots:
    void setWindowInfo(const WindowInfo &info);
    void setFfmpegExecutable(const QString &executable);
    void setMaxImageEdgeLength(int length);
    void setMaxVideoEdgeLength(int length);
    void setVideoFrameRate(double rate);
    void setMaxVideoLength(int length);
    void setRecordType(RecordType type);

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
    int _maxImageEdgeLength;
    int _maxVideoEdgeLength;
    double _videoFrameRate;
    int _maxVideoLength;
    RecordType _recordType;
    QProcess *process;
    QFile *outputFile;
};

#endif
