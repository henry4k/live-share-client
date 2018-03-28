#include <QStringList>
#include <QHeaderView>
#include "Upload.h"
#include "UploadQueueEntry.h"
#include "UploadQueueWidget.h"


UploadQueueWidget::UploadQueueWidget(QWidget *parent) :
    QTableWidget(parent)
{
    setSelectionMode(NoSelection);
    setShowGrid(false);
    setAlternatingRowColors(true);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    verticalHeader()->hide();
    const QStringList headers = QStringList() <<
                                    tr("Type") <<
                                    tr("Total Size") <<
                                    tr("Progress") <<
                                    ""; // cancel button
    setColumnCount(headers.count());
    setHorizontalHeaderLabels(headers);
    horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch); // stretch progress column
}

UploadQueueWidget::~UploadQueueWidget()
{
}

void UploadQueueWidget::uploadEnqueued(Upload *upload)
{
    const int row = rowCount();
    insertRow(row);
    new UploadQueueEntry(upload, this, row, this);
}
