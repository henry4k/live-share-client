#include <QtGlobal> // qWarning
#include <QCoreApplication> // applicationName
#include <QTime> // currentTime
#include <QDir> // temp
#include <QFile>
#include <QStringList>
#include <QSettings>
#include <string.h> // memset
#include "Recorder.h"


static QString DefaultFfmpegExecutable = "ffmpeg";
static int DefaultMaxImageEdgeLength = 800;
static int DefaultMaxVideoEdgeLength = 400;
static double DefaultVideoFrameRate = 25;
static int DefaultMaxVideoLength = 30;

Recorder::Recorder(QObject *parent) :
    QObject(parent),
    _maxImageEdgeLength(0),
    _maxVideoEdgeLength(0),
    _videoFrameRate(0),
    _maxVideoLength(0),
    _recordType(Image),
    process(new QProcess(this)),
    outputFile(nullptr)
{
    memset(&windowInfo, 0, sizeof(WindowInfo));

    process->setProcessChannelMode(QProcess::ForwardedChannels);

    connect(process, &QProcess::errorOccurred,
            this, &Recorder::process_errorOccurred);
    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &Recorder::process_finished);
}

Recorder::~Recorder()
{
    if(outputFile && outputFile->parent() == this)
        removeOutputFile();
}

void Recorder::readSettings(QSettings &settings)
{
    setFfmpegExecutable(settings.value("ffmpegExecutable", DefaultFfmpegExecutable).value<QString>());
    setMaxImageEdgeLength(settings.value("maxImageEdgeLength", DefaultMaxImageEdgeLength).value<int>());
    setMaxVideoEdgeLength(settings.value("maxVideoEdgeLength", DefaultMaxVideoEdgeLength).value<int>());
    setVideoFrameRate(settings.value("videoFrameRate", DefaultVideoFrameRate).value<double>());
    setMaxVideoLength(settings.value("maxVideoLength", DefaultMaxVideoLength).value<int>());

    emit ffmpegExecutableChanged(ffmpegExecutable());
    emit maxImageEdgeLengthChanged(maxImageEdgeLength());
    emit maxVideoEdgeLengthChanged(maxVideoEdgeLength());
    emit videoFrameRateChanged(videoFrameRate());
    emit maxVideoLengthChanged(maxVideoLength());
}

void Recorder::writeSettings(QSettings &settings)
{
    settings.setValue("ffmpegExecutable", ffmpegExecutable());
    settings.setValue("maxImageEdgeLength", maxImageEdgeLength());
    settings.setValue("maxVideoEdgeLength", maxVideoEdgeLength());
    settings.setValue("videoFrameRate", videoFrameRate());
    settings.setValue("maxVideoLength", maxVideoLength());
}


// ---- Getters ----

QString Recorder::ffmpegExecutable() const
{
    return process->program();
}

int Recorder::maxImageEdgeLength() const
{
    return _maxImageEdgeLength;
}

int Recorder::maxVideoEdgeLength() const
{
    return _maxVideoEdgeLength;
}

double Recorder::videoFrameRate() const
{
    return _videoFrameRate;
}

int Recorder::maxVideoLength() const
{
    return _maxVideoLength;
}

Recorder::RecordType Recorder::recordType() const
{
    return _recordType;
}

QString Recorder::mimeType() const
{
    switch(_recordType)
    {
    //case Image: return "image/png";
    case Image: return "image/jpeg";
    //case Image: return "image/webp";
    case Video: return "video/webm";
    }
    return "";
}


// ---- Setters ----

void Recorder::setWindowInfo(const WindowInfo &info)
{
    windowInfo = info;
}

void Recorder::setFfmpegExecutable(const QString &executable)
{
    qWarning("setFfmpegExecutable %s", qUtf8Printable(executable));
    process->setProgram(executable);
}

void Recorder::setMaxImageEdgeLength(int length)
{
    _maxImageEdgeLength = length;
}

void Recorder::setMaxVideoEdgeLength(int length)
{
    _maxVideoEdgeLength = length;
}

void Recorder::setVideoFrameRate(double rate)
{
    _videoFrameRate = rate;
}

void Recorder::setMaxVideoLength(int length)
{
    _maxVideoLength = length;
}

void Recorder::setRecordType(RecordType type)
{
    _recordType = type;
}


// ---- Program Arguments ----

bool Recorder::appendInputArguments(QStringList &arguments)
{
    Q_ASSERT(windowInfo.w > 0);
    Q_ASSERT(windowInfo.h > 0);
    Q_ASSERT(_videoFrameRate > 0);

    const QString x = QString().setNum(windowInfo.x);
    const QString y = QString().setNum(windowInfo.y);
    const QString inputSize = QString("%1x%2").arg(windowInfo.w).arg(windowInfo.h);
    const QString videoFrameRateStr = QString().setNum(_videoFrameRate);

    bool drawMouse = false;
    bool showRegion = false;

#if defined(_WIN32)
    arguments << "-f" << "gdigrab";
    arguments << "-offset_x" << x;
    arguments << "-offset_y" << y;
#else
    arguments << "-f" << "x11grab";
    arguments << "-grab_x" << x;
    arguments << "-grab_y" << y;
#endif

    arguments << "-draw_mouse"  << QString().setNum((int)drawMouse);
    arguments << "-show_region" << QString().setNum((int)showRegion);

    arguments << "-video_size" << inputSize;
    arguments << "-framerate" << videoFrameRateStr;

#if defined(_WIN32)
    arguments << "-i";
    if(windowInfo.title[0] == '\0') // empty string
    {
        arguments << "desktop";
    }
    else
    {
        const QString titleStr = QString("title=%1").arg(windowInfo.title);
        arguments << titleStr;
        // TODO: dont use offset and size then
    }
#else
    arguments << "-i" << windowInfo.displayName;
#endif

    return true;
}

bool Recorder::appendOutputArguments(QStringList &arguments)
{
    switch(_recordType)
    {
    case Image:
        arguments << "-f" << "image2";
        //arguments << "-c:v" << "png";
        arguments << "-c:v" << "jpeg2000";
        //arguments << "-q:v" << "90"; // jpg quality
        arguments << "-pred" << "1"; // lossless jpg
        //arguments << "-c:v" << "libwebp";
        arguments << "-frames:v" << "1";
        break;

    case Video:
        arguments << "-f" << "webm";
        arguments << "-c:v" << "libvpx";
        break;
    }
    return true;
}

void Recorder::getOutputSize(int *outputWidth, int *outputHeight)
{
    Q_ASSERT(windowInfo.w > 0);
    Q_ASSERT(windowInfo.h > 0);

    int maxEdgeLength;
    switch(_recordType)
    {
    case Image:
        maxEdgeLength = _maxImageEdgeLength;
        break;

    case Video:
        maxEdgeLength = _maxVideoEdgeLength;
        break;
    }
    Q_ASSERT(maxEdgeLength > 0);

    const int inputWidth  = windowInfo.w;
    const int inputHeight = windowInfo.h;

    const int  *longInputEdge;
    const int *shortInputEdge;
    int  *longOutputEdge;
    int *shortOutputEdge;

    if(inputWidth >= inputHeight)
    {
        longInputEdge   = &inputWidth;
        shortInputEdge  = &inputHeight;
        longOutputEdge  = outputWidth;
        shortOutputEdge = outputHeight;
    }
    else
    {
        longInputEdge   = &inputHeight;
        shortInputEdge  = &inputWidth;
        longOutputEdge  = outputHeight;
        shortOutputEdge = outputWidth;
    }

    if(*longInputEdge > maxEdgeLength)
    {
        *longOutputEdge  = maxEdgeLength;
        *shortOutputEdge = maxEdgeLength * *shortInputEdge / *longInputEdge;
    }
    else
    {
        *longOutputEdge  = *longInputEdge;
        *shortOutputEdge = *shortInputEdge;
    }
}

bool Recorder::appendOutputSizeArguments(QStringList &arguments)
{
    int outputWidth, outputHeight;
    getOutputSize(&outputWidth, &outputHeight);

    const QString outputSize =
        QString("%1x%2").arg(outputWidth).arg(outputHeight);
    arguments << "-s" << outputSize;

    //const QString scaleFilter =
    //    QString("scale=%1x%2").arg(outputWidth).arg(outputHeight);
    //arguments << "-filter:v" << scaleFilter;

    //const QString scaleFilter =
    //    QString("scale=width=%1:height=%2:force_original_aspect_ratio=decrease")
    //        .arg(windowInfo.w)
    //        .arg(windowInfo.h);
    //arguments << "-filter:v" << scaleFilter;

    return true;
}

bool Recorder::ownsOutputFile() const
{
    return outputFile &&
           outputFile->parent() == this;
}

void Recorder::removeOutputFile()
{
    Q_ASSERT(ownsOutputFile());
    Q_ASSERT(!outputFile->isOpen());

    if(outputFile->exists())
        outputFile->remove();

    delete outputFile;
    outputFile = nullptr;
}

static QString GetTempFileName()
{
    const QString applicationName = QCoreApplication::applicationName();
    const QDir tempDir = QDir::temp();

    int uniqueValue = QTime::currentTime().msecsSinceStartOfDay();
    QString filePath;

    while(true)
    {
        const QString name = QString("%1.%2").arg(applicationName).arg(uniqueValue);
        filePath = tempDir.filePath(name);

        if(QFile::exists(filePath))
            uniqueValue++;
        else
            break;
    }

    return filePath;
}

void Recorder::createOutputFile()
{
    if(ownsOutputFile())
        removeOutputFile();
    outputFile = new QFile(GetTempFileName(), this);
}

void Recorder::start()
{
    Q_ASSERT(!isRecording());
    createOutputFile();

    QStringList arguments;
    arguments << "-hide_banner";
#if !defined(QT_NO_WARNING_OUTPUT)
    arguments << "-loglevel" << "warning";
#endif
    if(!appendInputArguments(arguments)  ||
       !appendOutputArguments(arguments) ||
       !appendOutputSizeArguments(arguments))
    {
        qWarning("ffmpeg parameter generation failed");
        return;
    }
    arguments << outputFile->fileName();

    qWarning("%s %s", qUtf8Printable(process->program()), qUtf8Printable(arguments.join(" ")));

    process->setArguments(arguments);
    process->start(QIODevice::WriteOnly);
    //process->start(QIODevice::ReadWrite);
}

void Recorder::stop()
{
    Q_ASSERT(isRecording());
    qWarning("Recorder::stop");
    process->write("q\n");
    process->closeWriteChannel();
    //process->waitForFinished();
}

bool Recorder::isRecording() const
{
    return process->state() != QProcess::NotRunning;
}

void Recorder::process_errorOccurred(QProcess::ProcessError error)
{
    // TODO: Print error!
    qWarning("errorOccurred %d", error);
}

void Recorder::process_finished(int exitCode, QProcess::ExitStatus exitStatus)
{
    // TODO: Print error!
    qWarning("finished exitCode=%d exitStatus=%d", exitCode, exitStatus);

    const bool success = exitStatus == QProcess::NormalExit &&
                         exitCode == 0;

    Q_ASSERT(ownsOutputFile());

    if(success)
        emit finished(outputFile);
    else
    {
        emit finished(nullptr);
        removeOutputFile();
    }
}
