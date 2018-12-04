#include <QtGlobal> // qWarning
#include <QAction>
#include <QSettings>
#include <QKeySequence>
#include <QCloseEvent>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QFileDialog>
#include <QSignalBlocker>
#include "ExecutableValidator.h"
#include "WindowInfo.h"
#include "RecordingManager.h"
#include "Recorder.h"
#include "UploadManager.h"
#include "Upload.h"
#include "MainWindow.h"
#include "ui_MainWindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    recordingManager(new RecordingManager(this)),
    uploadManager(new UploadManager(this))
{
    ui->setupUi(this);

    // Actions
    QAction *quitAction = new QAction();
    quitAction->setAutoRepeat(false);
    quitAction->setMenuRole(QAction::QuitRole);
    quitAction->setShortcuts(QKeySequence::Quit);
    addAction(quitAction);
    connect(quitAction, &QAction::triggered,
            this, &MainWindow::quitActionTriggered);

    QAction *preferencesAction = new QAction();
    preferencesAction->setAutoRepeat(false);
    preferencesAction->setMenuRole(QAction::PreferencesRole);
    preferencesAction->setShortcuts(QKeySequence::Preferences);
    addAction(preferencesAction);
    connect(preferencesAction, &QAction::triggered,
            this, &MainWindow::preferencesActionTriggered);

    qWarning("quit-shortcut: %s", qUtf8Printable(QKeySequence(QKeySequence::Quit).toString()));
    qWarning("pref-shortcut: %s", qUtf8Printable(QKeySequence(QKeySequence::Preferences).toString()));

    {
        // Protocol must be http or https
        // Must not end with a slash
        const QRegularExpression regExp("https?:\\/\\/[^\\s]*[^\\/]");
        QValidator *validator = new QRegularExpressionValidator(regExp, this);
        ui->urlLineEdit->setValidator(validator);
    }

    {
        // URL safe characters
        const QRegularExpression regExp("[A-Za-z0-9_. -]*");
        QValidator *validator = new QRegularExpressionValidator(regExp, this);
        ui->userNameLineEdit->setValidator(validator);
    }

    //{
    //    // URL safe characters
    //    const QRegularExpression regExp("[A-Za-z0-9_. -]*");
    //    QValidator *validator = new QRegularExpressionValidator(regExp, this);
    //    ui->passwordLineEdit->setValidator(validator);
    //}

    {
        // URL safe characters
        const QRegularExpression regExp("[A-Za-z0-9_. -]*");
        QValidator *validator = new QRegularExpressionValidator(regExp, this);
        ui->categoryLineEdit->setValidator(validator);
    }

    {
        ExecutableValidator *validator = new ExecutableValidator(this);
        ui->ffmpegLineEdit->setValidator(validator);
    }

    Recorder *recorder = recordingManager->recorder();

    connect(recordingManager, &RecordingManager::recordingFinished,
            this, &MainWindow::recordingFinished);

    // Widget --{changed}-> RecordingManager
    connect(ui->imageKeySequenceEdit, &KeySequence_Widget::keySequenceChanged,
            recordingManager, &RecordingManager::setImageShortcut);
    connect(ui->videoKeySequenceEdit, &KeySequence_Widget::keySequenceChanged,
            recordingManager, &RecordingManager::setVideoShortcut);

    // Widget --{changed}-> Recorder
    connect(ui->maxImageEdgeLengthSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            recorder, &Recorder::setMaxImageEdgeLength);
    connect(ui->maxVideoEdgeLengthSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            recorder, &Recorder::setMaxVideoEdgeLength);
    connect(ui->frameRateDoubleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            recorder, &Recorder::setVideoFrameRate);
    connect(ui->maxVideoLengthSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            recorder, &Recorder::setMaxVideoLength);

    // HotkeyResetButton --{clear}-> KeySequenceEdit
    connect(ui->imageHotkeyResetButton, &QToolButton::clicked,
            ui->imageKeySequenceEdit, &KeySequence_Widget::clear);
    connect(ui->videoHotkeyResetButton, &QToolButton::clicked,
            ui->videoKeySequenceEdit, &KeySequence_Widget::clear);

    // RecordingManager --{changed}-> Widget
    connect(recordingManager, &RecordingManager::imageShortcutChanged,
            ui->imageKeySequenceEdit, &KeySequence_Widget::setKeySequence);
    connect(recordingManager, &RecordingManager::videoShortcutChanged,
            ui->videoKeySequenceEdit, &KeySequence_Widget::setKeySequence);

    // Recorder --{changed}-> Widget
    connect(recorder, &Recorder::ffmpegExecutableChanged,
            ui->ffmpegLineEdit, &QLineEdit::setText);
    connect(recorder, &Recorder::maxImageEdgeLengthChanged,
            ui->maxImageEdgeLengthSpinBox, &QSpinBox::setValue);
    connect(recorder, &Recorder::maxVideoEdgeLengthChanged,
            ui->maxVideoEdgeLengthSpinBox, &QSpinBox::setValue);
    connect(recorder, &Recorder::videoFrameRateChanged,
            ui->frameRateDoubleSpinBox, &QDoubleSpinBox::setValue);
    connect(recorder, &Recorder::maxVideoLengthChanged,
            ui->maxVideoLengthSpinBox, &QSpinBox::setValue);

    // UploadManager --{changed}-> Widget
    connect(uploadManager, &UploadManager::serviceUrlChanged,
            ui->urlLineEdit, &QLineEdit::setText);
    connect(uploadManager, &UploadManager::userNameChanged,
            ui->userNameLineEdit, &QLineEdit::setText);
    connect(uploadManager, &UploadManager::passwordChanged,
            ui->passwordLineEdit, &QLineEdit::setText);

    connect(uploadManager, &UploadManager::uploadEnqueued,
            ui->uploadQueueWidget, &UploadQueueWidget::uploadEnqueued);

    const QSignalBlocker imageKeySequenceBlocker(ui->imageKeySequenceEdit);
    const QSignalBlocker videoKeySequenceBlocker(ui->videoKeySequenceEdit);
    const QSignalBlocker ffmpegExecutableBlocker(ui->ffmpegLineEdit);
    const QSignalBlocker maxImageEdgeLengthBlocker(ui->maxImageEdgeLengthSpinBox);
    const QSignalBlocker maxVideoEdgeLengthBlocker(ui->maxVideoEdgeLengthSpinBox);
    const QSignalBlocker videoFrameRateBlocker(ui->frameRateDoubleSpinBox);
    const QSignalBlocker maxVideoLengthBlocker(ui->maxVideoLengthSpinBox);
    const QSignalBlocker serviceUrlBlocker(ui->urlLineEdit);
    const QSignalBlocker userNameBlocker(ui->userNameLineEdit);
    const QSignalBlocker passwordBlocker(ui->passwordLineEdit);
    const QSignalBlocker categoryBlocker(ui->categoryLineEdit);
    readSettings();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::readSettings()
{
    QSettings settings;
    //qWarning("read settings %s", qUtf8Printable(settings.fileName()));

    settings.beginGroup("recording");
    recordingManager->readSettings(settings);
    settings.endGroup();

    settings.beginGroup("upload");
    uploadManager->readSettings(settings);
    ui->categoryLineEdit->setText(settings.value("category").value<QString>());
    settings.endGroup();
}

void MainWindow::writeSettings()
{
    QSettings settings;
    //qWarning("write settings %s", qUtf8Printable(settings.fileName()));

    settings.beginGroup("recording");
    recordingManager->writeSettings(settings);
    settings.endGroup();

    settings.beginGroup("upload");
    uploadManager->writeSettings(settings);
    settings.setValue("category", ui->categoryLineEdit->text());
    settings.endGroup();
}

void MainWindow::quitActionTriggered()
{
    close();
}

void MainWindow::preferencesActionTriggered()
{
    ui->tabWidget->setCurrentWidget(ui->preferencesTabPage);
    activateWindow();
    raise();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    writeSettings();
    event->accept();
}

void MainWindow::recordingFinished(QFile *file)
{
    Upload *upload = new Upload(file,
                                recordingManager->recorder()->mimeType(),
                                ui->categoryLineEdit->text());
    uploadManager->enqueueUpload(upload);
}

void MainWindow::on_urlLineEdit_editingFinished()
{
    uploadManager->setServiceUrl(ui->urlLineEdit->text());
}

void MainWindow::on_userNameLineEdit_editingFinished()
{
    uploadManager->setUserName(ui->userNameLineEdit->text());
}

void MainWindow::on_passwordLineEdit_editingFinished()
{
    uploadManager->setPassword(ui->passwordLineEdit->text());
}

void MainWindow::on_ffmpegLineEdit_editingFinished()
{
    recordingManager->recorder()->setFfmpegExecutable(ui->ffmpegLineEdit->text());
}

void MainWindow::on_ffmpegFileDialogButton_clicked()
{
#if defined(_WIN32)
    const char *filter = "*.exe";
#else
    const char *filter = "";
#endif

    const QString selection =
        QFileDialog::getOpenFileName(this,
                                     tr("Select FFmpeg executable"), // caption
                                     recordingManager->recorder()->ffmpegExecutable(), // start dir/file
                                     filter);
    if(!selection.isNull())
    {
        ui->ffmpegLineEdit->setText(selection);
        recordingManager->recorder()->setFfmpegExecutable(selection);
    }

    /*
    QFileDialog dialog(this);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    //dialog.setFileMode(QFileDialog::ExistingFile);
#if defined(_WIN32)
    dialog.setNameFilter("*.exe");
#endif
    if(dialog.exec() == QDialog::Accepted)
    {
        const QString ffmpegLocation = dialog.selectedFiles().first();
        ui->ffmpegLineEdit->setText(ffmpegLocation);
        recordingManager->setFfmpegExecutable(ffmpegLocation);
    }
    */
}
