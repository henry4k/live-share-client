#ifndef UPLOADQUEUEENTRY_H
#define UPLOADQUEUEENTRY_H

#include <QObject>
#include "Upload.h" // Upload::State

class QTableWidget;
class QTableWidgetItem;
class QProgressBar;


class UploadQueueEntry : public QObject
{
    Q_OBJECT

public:
    UploadQueueEntry(Upload *upload,
                     QTableWidget *tableWidget,
                     int row,
                     QWidget *parent = nullptr);
    ~UploadQueueEntry();

private slots:
    void stateChanged(Upload::State state);
    void sentBytesChanged(qint64 bytes);

private:
    Upload *upload;
    QTableWidget *tableWidget;
    QTableWidgetItem *item;
    QProgressBar *progressBar;
};

#endif
