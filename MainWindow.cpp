#include <QtGlobal> // qWarning
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QFileDialog>
#include <QNetworkReply>
#include <QMimeDatabase>
#include "ExecutableValidator.h"
#include "WindowInfo.h"
#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),

    //networkAccessManager(new QNetworkAccessManager(this)),
    //uploadedFile(nullptr),

    imageHotkey(new QHotkey(this)),
    videoHotkey(new QHotkey(this)),
    recorder(new Recorder(this))
{
    connect(imageHotkey, &QHotkey::activated,
            this, &MainWindow::_on_imageHotkey_activated);

    connect(videoHotkey, &QHotkey::activated,
            this, &MainWindow::_on_videoHotkey_activated);

    connect(recorder, &Recorder::finished,
            this, &MainWindow::_on_recorder_finished);

    ui->setupUi(this);

    // TODO:
    // ui->categoryLineEdit->setValidator(); - no / at start or end; only URL safe characters
    {
        // Protocol must be http or https
        // Must not end with a slash
        const QRegularExpression regExp("https?:\\/\\/[^\\s]*[^\\/]");
        QValidator *validator = new QRegularExpressionValidator(regExp, this);
        ui->urlLineEdit->setValidator(validator);
    }

    {
        ExecutableValidator *validator = new ExecutableValidator(this);
        ui->ffmpegLineEdit->setValidator(validator);
    }

    connect(ui->imageHotkeyResetButton, &QToolButton::clicked,
            ui->imageKeySequenceEdit, &KeySequence_Widget::clear);

    connect(ui->videoHotkeyResetButton, &QToolButton::clicked,
            ui->videoKeySequenceEdit, &KeySequence_Widget::clear);
}

MainWindow::~MainWindow()
{
    delete ui;
}

/*
void MainWindow::uploadFile(const QString& fileName)
{
    Q_ASSERT(uploadedFile == nullptr);
    uploadedFile = new QFile(fileName);
    uploadedFile->open(QIODevice::ReadOnly);

    const QString url = serviceUrl + QString("/user/") + uploadedFile->fileName();

    QMimeDatabase mimeDb;
    const QMimeType mimeType = mimeDb.mimeTypeForFile(fileName);

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, mimeType.name());

    QNetworkReply* reply = networkAccessManager->post(request, uploadedFile);
    reply->deleteLater();

    connect(reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
        [=](QNetworkReply::NetworkError code){});

    connect(reply, &QNetworkReply::uploadProgress,
            this, [this](qint64 sent, qint64 total){ printf("upload %lld/%lld\n", sent, total); });
    connect(reply, &QNetworkReply::finished,
            this, [this](){ printf("upload complete\n"); });
}
*/

void MainWindow::_on_imageHotkey_activated()
{
    qWarning("imageHotkey pressed");
    if(!recorder->isRecording())
        startRecording(Recorder::Image);
}

void MainWindow::_on_videoHotkey_activated()
{
    qWarning("videoHotkey pressed");
    if(recorder->isRecording())
    {
        if(recorder->recordType() == Recorder::Video)
            recorder->stop();
    }
    else
        startRecording(Recorder::Video);
}

void MainWindow::startRecording(Recorder::RecordType recordType)
{
    Q_ASSERT(!recorder->isRecording());

    WindowInfo windowInfo;
    if(!GetInfoOfActiveWindow(&windowInfo))
    {
        qWarning("GetInfoOfActiveWindow failed");
        return;
    }

    int maxEdgeLength;
    switch(recordType)
    {
    case Recorder::Image:
        maxEdgeLength = ui->maxImageEdgeLengthSpinBox->value();
        break;

    case Recorder::Video:
        maxEdgeLength = ui->maxVideoEdgeLengthSpinBox->value();
        break;
    }

    recorder->setFrameRate(ui->frameRateDoubleSpinBox->value());
    recorder->setMaxEdgeLength(maxEdgeLength);
    recorder->setWindowInfo(windowInfo);
    recorder->setRecordType(recordType);
    recorder->start();
}

void MainWindow::_on_recorder_finished(QFile *outputFile)
{
    if(outputFile)
        qWarning("on_recorder_finished %s (%s)",
                qUtf8Printable(outputFile->fileName()),
                qUtf8Printable(recorder->mimeType()));
    else
        qWarning("on_recorder_finished null");
}

void MainWindow::on_urlLineEdit_editingFinished()
{
    //serviceUrl = ui->urlLineEdit->text();
}

void MainWindow::on_imageKeySequenceEdit_keySequenceChanged(QKeySequence sequence)
{
    if(!imageHotkey->setShortcut(sequence, true))
        qWarning("Can't set image hotkey.");
}

void MainWindow::on_videoKeySequenceEdit_keySequenceChanged(QKeySequence sequence)
{
    if(!videoHotkey->setShortcut(sequence, true))
        qWarning("Can't set video hotkey.");
}

void MainWindow::on_ffmpegLineEdit_editingFinished()
{
    recorder->setFfmpegLocation(ui->ffmpegLineEdit->text());
}

void MainWindow::on_ffmpegFileDialogButton_clicked()
{
    QFileDialog dialog(this);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setFileMode(QFileDialog::ExistingFile);
#if defined(_WIN32)
    dialog.setNameFilter("*.exe");
#endif
    if(dialog.exec() == QDialog::Accepted)
    {
        const QString ffmpegLocation = dialog.selectedFiles().first();
        ui->ffmpegLineEdit->setText(ffmpegLocation);
        recorder->setFfmpegLocation(ffmpegLocation);
    }
}
