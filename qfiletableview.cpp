
#include "qfiletableview.h"

QLogfileTableView::QLogfileTableView(QWidget *parent)
    : QTableView(parent)
    , m_onCellClickRight(nullptr)
    , m_onCellClickLeft(nullptr)
{

}

void QLogfileTableView::mousePressEvent(QMouseEvent *event){
    QModelIndex index = indexAt(event->pos());
    if ((event->button() & Qt::LeftButton) == Qt::LeftButton)
    {
        if (m_onCellClickLeft != nullptr)
            m_onCellClickLeft(index, event->globalPosition().toPoint());
    }

    if ((event->button() & Qt::RightButton) == Qt::RightButton)
    {
        if (m_onCellClickRight != nullptr)
            m_onCellClickRight(index, event->globalPosition().toPoint());
    }
}
