#include <QtGlobal> // qWarning
#include <QCoreApplication> // applicationName
#include <QTime> // currentTime
#include <QDir> // temp
#include <string.h> // memset
#include "Recorder.h"


Recorder::Recorder(QObject *parent) :
    QObject(parent),
    maxEdgeLength(0),
    frameRate(0),
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

void Recorder::setFfmpegLocation(const QString &location)
{
    qWarning("setFfmpegLocation %s", qUtf8Printable(location));
    process->setProgram(location);
}

void Recorder::setWindowInfo(const WindowInfo &info)
{
    windowInfo = info;
}

void Recorder::setMaxEdgeLength(int length)
{
    maxEdgeLength = length;
}

void Recorder::setFrameRate(float rate)
{
    frameRate = rate;
}

void Recorder::setRecordType(RecordType type)
{
    _recordType = type;
}

Recorder::RecordType Recorder::recordType() const
{
    return _recordType;
}

QString Recorder::mimeType() const
{
    switch(_recordType)
    {
    case Image: return "image/webp";
    case Video: return "video/webm";
    }
    return "";
}

bool Recorder::appendInputArguments(QStringList &arguments)
{
    Q_ASSERT(windowInfo.w > 0);
    Q_ASSERT(windowInfo.h > 0);
    Q_ASSERT(frameRate > 0);

    const QString x = QString().setNum(windowInfo.x);
    const QString y = QString().setNum(windowInfo.y);
    const QString inputSize = QString("%1x%2").arg(windowInfo.w).arg(windowInfo.h);
    const QString frameRateStr = QString().setNum(frameRate);

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
    arguments << "-framerate" << frameRateStr;

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
        arguments << "-c:v" << "libwebp";
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
    arguments << "-loglevel" << "warning";
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
    process->start(QIODevice::ReadWrite);
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
