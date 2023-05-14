#include "qcashfilemodel.h"

#include <QObject>
#include <QColor>
#include <QBrush>
#include <QContiguousCache>
#include <QAbstractListModel>
#include <IDataObject.h>
#include <memory>
#include "ctracer.h"
#include "cfunctracer.h"
#include "clogentry.h"

static const int bufferSize(500);
static const int lookAhead(100);
static const int halfLookAhead(lookAhead / 2);

QCashFileModel::QCashFileModel(std::shared_ptr<CTracer> tracer,
                               IDataContainer& container,
                               QObject *parent)
    : QAbstractListModel(parent)
    , m_container(container)
    , m_rows(bufferSize)
    , m_count(10000)
    , m_trace(tracer)
{
    CFuncTracer trace("QCashFileModel::QCashFileModel", m_trace);
    try
    {

    }
    catch(std::exception& ex)
    {
        trace.Error("Exception occurred : %s", ex.what());
    }
    catch(...)
    {
        trace.Error("Exception occurred");
    }
}

QCashFileModel::~QCashFileModel()
{
    CFuncTracer trace("QCashFileModel::~QCashFileModel", m_trace);
    try
    {

    }
    catch(std::exception& ex)
    {
        trace.Error("Exception occurred : %s", ex.what());
    }
    catch(...)
    {
        trace.Error("Exception occurred");
    }
}

int QCashFileModel::rowCount(const QModelIndex &) const
{
    CFuncTracer trace("QCashFileModel::rowCount", m_trace);
    try
    {
        return m_count;
    }
    catch(std::exception& ex)
    {
        trace.Error("Exception occurred : %s", ex.what());
    }
    catch(...)
    {
        trace.Error("Exception occurred");
    }
    return 0;
}

QVariant QCashFileModel::data(const QModelIndex &index, int role) const
{
    CFuncTracer trace("QCashFileModel::data", m_trace);
    try
    {
        trace.Trace("col:%ld, row:%ld, role:%s"
                    , index.column()
                    , index.row()
                    , (role == Qt::DisplayRole)?"DisplayRole":
                        (role == Qt::DecorationRole)?"DecorationRole":
                        (role == Qt::EditRole)?"EditRole":
                        (role == Qt::ToolTipRole)?"ToolTipRole":
                        (role == Qt::StatusTipRole)?"StatusTipRole":
                        (role == Qt::WhatsThisRole)?"WhatsThisRole":
                        (role == Qt::FontRole)?"WhatsThisRole":
                        (role == Qt::TextAlignmentRole)?"TextAlignmentRole":
                        (role == Qt::BackgroundRole)?"BackgroundRole":
                        (role == Qt::ForegroundRole)?"ForegroundRole":
                        (role == Qt::CheckStateRole)?"CheckStateRole":
                        (role == Qt::AccessibleTextRole)?"AccessibleTextRole":
                        (role == Qt::AccessibleDescriptionRole)?"AccessibleDescriptionRole":
                        (role == Qt::SizeHintRole)?"SizeHintRole":
                        (role == Qt::InitialSortOrderRole)?"InitialSortOrderRole":
                        (role == Qt::DisplayPropertyRole)?"DisplayPropertyRole":
                        (role == Qt::DecorationPropertyRole)?"DecorationPropertyRole":
                        (role == Qt::ToolTipPropertyRole)?"ToolTipPropertyRole":
                        (role == Qt::StatusTipPropertyRole)?"StatusTipPropertyRole":
                        (role == Qt::WhatsThisPropertyRole)?"WhatsThisPropertyRole":
                        (role == Qt::UserRole)?"UserRole":
                        "Unknown");

        if (  (role != Qt::DisplayRole)
            &&(role != Qt::BackgroundRole))
            return QVariant();

        int row = index.row();
        int column = index.column();

        if (row > m_rows.lastIndex()){
            if (row - m_rows.lastIndex() > lookAhead)
                cacheRows(row-halfLookAhead, qMin(m_count, row+halfLookAhead));
            else while (row > m_rows.lastIndex())
            {
                m_rows.append(fetchRow(m_rows.lastIndex() + 1));
            }
        } else if (row < m_rows.firstIndex()){
            if (m_rows.firstIndex() - row > lookAhead)
                cacheRows( qMax(0, row-halfLookAhead), row+halfLookAhead);
            else while (row < m_rows.firstIndex())
            {
                m_rows.prepend(fetchRow(m_rows.firstIndex() - 1));
            }
        }

        const IData& data_row = m_rows.at(row);
        switch(role)
        {
            case Qt::DisplayRole:
            {
                return data_row.GetDataElement(column);
            }
            case Qt::BackgroundRole:
            {
                QColor clr = data_row.GetDataElement(column);
                QBrush bgBrush(clr);
                return bgBrush;
            }
            default:
                break;
        }
    }
    catch(std::exception& ex)
    {
        trace.Error("Exception occurred : %s", ex.what());
    }
    catch(...)
    {
        trace.Error("Exception occurred");
    }
    return QVariant();
}

void QCashFileModel::rowToggleMark(const QModelIndex &index, bool &bMark)
{
    CFuncTracer trace("QCashFileModel::rowToggleMark", m_trace);
    try
    {
        int row = index.row();
        int column = index.column();

        if (row > m_rows.lastIndex()){
            if (row - m_rows.lastIndex() > lookAhead)
                cacheRows(row-halfLookAhead, qMin(m_count, row+halfLookAhead));
            else while (row > m_rows.lastIndex())
            {
                m_rows.append(fetchRow(m_rows.lastIndex() + 1));
            }
        } else if (row < m_rows.firstIndex()){
            if (m_rows.firstIndex() - row > lookAhead)
                cacheRows( qMax(0, row-halfLookAhead), row+halfLookAhead);
            else while (row < m_rows.firstIndex())
            {
                m_rows.prepend(fetchRow(m_rows.firstIndex() - 1));
            }
        }

        m_container.ToggleMark(row);

        trace.Trace("row %ld is changed", row);
        QModelIndex first = createIndex((row >  1)?(row - 1) : 0, 0);
        QModelIndex last = createIndex(((row + 2) < m_rows.lastIndex())?(row + 1) : row, (int)eColumns::eNumOfColumns);
        trace.Trace("Send signal that the row is changed (first:%ld, %ld  last:%ld, %ld", first.row(), first.column(), last.row(), last.column());
        emit dataChanged(first, last);

    }
    catch(std::exception& ex)
    {
        trace.Error("Exception occurred : %s", ex.what());
    }
    catch(...)
    {
        trace.Error("Exception occurred");
    }
}

void QCashFileModel::cacheRows(int from, int to) const
{
    CFuncTracer trace("QCashFileModel::cacheRows", m_trace);
    try
    {
        for (int i= from; i < to; ++i)
        {
            const IData& data = fetchRow(i);
            m_rows.insert(i, data);
        }
    }
    catch(std::exception& ex)
    {
        trace.Error("Exception occurred : %s", ex.what());
    }
    catch(...)
    {
        trace.Error("Exception occurred");
    }

}

IData QCashFileModel::fetchRow(int position) const
{
    CFuncTracer trace("QCashFileModel::fetchRow", m_trace);
    return m_container.GetData(position);
}
