#ifndef RECORDINGMANAGER_H
#define RECORDINGMANAGER_H

#include <QObject>
#include <QKeySequence>
#include "Recorder.h"

class QSettings;
class QHotkey;

class RecordingManager : public QObject
{
    Q_OBJECT

public:
    RecordingManager(QObject *parent = nullptr);
    ~RecordingManager();

    void readSettings(QSettings &settings);
    void writeSettings(QSettings &settings);

    QKeySequence imageShortcut() const;
    QKeySequence videoShortcut() const;
    Recorder *recorder() const;

signals:
    void imageShortcutChanged(const QKeySequence &shortcut);
    void videoShortcutChanged(const QKeySequence &shortcut);

    void recordingFinished(QFile *file);

public slots:
    void setImageShortcut(QKeySequence shortcut);
    void setVideoShortcut(QKeySequence shortcut);

private slots:
    void imageHotkeyActivated();
    void videoHotkeyActivated();
    void imageHotkeyRegisteredChanged(bool registered);
    void videoHotkeyRegisteredChanged(bool registered);
    void recorderFinished(QFile *outputFile);

private:
    void startRecording(Recorder::RecordType recordType);

    QHotkey *imageHotkey;
    QHotkey *videoHotkey;
    Recorder *_recorder;
};

#endif
