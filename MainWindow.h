#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFile>
#include <QUrl>
#include <QMainWindow>
#include <QNetworkAccessManager>

class RecordingManager;
class UploadManager;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_urlLineEdit_editingFinished();
    void on_userNameLineEdit_editingFinished();
    void on_ffmpegLineEdit_editingFinished();
    void on_ffmpegFileDialogButton_clicked();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    void readSettings();
    void writeSettings();

    Ui::MainWindow *ui;

    RecordingManager *recordingManager;
    UploadManager *uploadManager;
};

#endif // MAINWINDOW_H
