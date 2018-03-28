#ifndef UPLOADQUEUEWIDGET_H
#define UPLOADQUEUEWIDGET_H

#include <QTableWidget>

class Upload;


class UploadQueueWidget : public QTableWidget
{
    Q_OBJECT

public:
    UploadQueueWidget(QWidget *parent = nullptr);
    ~UploadQueueWidget();

public slots:
    void uploadEnqueued(Upload *upload);
};

#endif
