#include <QMimeDatabase>
#include <QTableWidget>
#include <QProgressBar>
#include <QToolButton>
#include <QLabel>
#include <QIcon>
#include "UploadQueueEntry.h"


struct UnitDesc
{
    const char *symbol;
    int size;
};

static const int SizeUnitsCount = 3;
static const UnitDesc SizeUnits[] = // Must be sorted largest to smallest!
{
    {"MiB", 1048576},
    {"KiB", 1024},
    {"B", 1}
};

static const UnitDesc *GetSizeUnit(int value)
{
    for(int i = 0; i < SizeUnitsCount; i++)
    {
        const UnitDesc *unit = &SizeUnits[i];
        if(value >= unit->size*3/2) // value >= 1.5*unitSize
            return unit;
    }
    return &SizeUnits[SizeUnitsCount-1];
}


UploadQueueEntry::UploadQueueEntry(Upload *_upload,
                                   QTableWidget *_tableWidget,
                                   int row,
                                   QWidget *parent) :
    QObject(parent),
    upload(_upload),
    tableWidget(_tableWidget)
{
    const qint64 totalBytes = upload->totalBytes();
    const UnitDesc *sizeUnit = GetSizeUnit(totalBytes);

    item = new QTableWidgetItem();
    item->setIcon(QIcon::fromTheme("video-x-generic")); // TODO: Get icon from Mime-DB.
    item->setText("webm");
    tableWidget->setItem(row, 0, item);

    QLabel *totalSizeLabel = new QLabel();
    totalSizeLabel->setText(
        QString::asprintf("%.2f %s",
                          (float)totalBytes / (float)sizeUnit->size,
                          sizeUnit->symbol));
    tableWidget->setCellWidget(row, 1, totalSizeLabel);

    progressBar = new QProgressBar();
    progressBar->setMaximum(totalBytes);
    progressBar->setFormat("%p%");
    progressBar->setTextVisible(true);
    progressBar->setEnabled(false); // gets enabled once upload is active
    tableWidget->setCellWidget(row, 2, progressBar);

    QToolButton *cancelButton = new QToolButton();
    cancelButton->setIcon(QIcon::fromTheme("edit-delete"));
    cancelButton->setText(tr("Cancel"));
    tableWidget->setCellWidget(row, 3, cancelButton);

    connect(upload, &Upload::stateChanged,
            this, &UploadQueueEntry::stateChanged);
    connect(upload, &Upload::sentBytesChanged,
            this, &UploadQueueEntry::sentBytesChanged);

    connect(cancelButton, &QToolButton::clicked,
            upload, &Upload::abort);
}

UploadQueueEntry::~UploadQueueEntry()
{
}

void UploadQueueEntry::stateChanged(Upload::State state)
{
    switch(state)
    {
    case Upload::Ready:
        break;

    case Upload::Active:
        progressBar->setEnabled(true);
        break;

    case Upload::Completed:
        tableWidget->removeRow(item->row());
        delete this;
        break;

    case Upload::Failed:
        progressBar->setEnabled(false);
        progressBar->setFormat(upload->errorString()); // TODO
        break;
    }
}

void UploadQueueEntry::sentBytesChanged(qint64 bytes)
{
    progressBar->setValue((int)bytes);
}
