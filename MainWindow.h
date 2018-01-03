#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFile>
#include <QUrl>
#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QHotkey>
#include "Recorder.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void _on_imageHotkey_activated();
    void _on_videoHotkey_activated();
    void _on_recorder_finished(QFile *outputFile);
    void on_urlLineEdit_editingFinished();
    void on_imageKeySequenceEdit_keySequenceChanged(QKeySequence sequence);
    void on_videoKeySequenceEdit_keySequenceChanged(QKeySequence sequence);
    void on_ffmpegLineEdit_editingFinished();
    void on_ffmpegFileDialogButton_clicked();

private:
    void startRecording(Recorder::RecordType recordType);

    Ui::MainWindow *ui;

    //QNetworkAccessManager* networkAccessManager;
    //QFile *uploadedFile;

    QHotkey *imageHotkey;
    QHotkey *videoHotkey;
    Recorder *recorder;
};

#endif // MAINWINDOW_H
