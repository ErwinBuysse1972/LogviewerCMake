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
                               IDataContainer *container,
                               QObject *parent)
    : QAbstractTableModel(parent)
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
    CFuncTracer trace("QCashFileModel::rowCount", m_trace, false);
    int count = 0;
    try
    {
        count = m_container->RowCount();
        trace.Trace("Count : %ld", count);
    }
    catch(std::exception& ex)
    {
        trace.Error("Exception occurred : %s", ex.what());
    }
    catch(...)
    {
        trace.Error("Exception occurred");
    }
    return count;
}

int QCashFileModel::columnCount(const QModelIndex &parent) const
{
    CFuncTracer trace("QCashFileModel::columnCount", m_trace, false);
    int count = 0;
    try
    {
        count = m_container->ColumnCount();
        trace.Trace("Count : %ld", count);
    }
    catch(std::exception& ex)
    {
        trace.Error("Exception occurred : %s", ex.what());
        count = 0;
    }
    catch(...)
    {
        trace.Error("Exception ocucrred");
        count = 0;
    }
    return count;
}

QModelIndex QCashFileModel::CreateIndex(int row, int column)
{
    CFuncTracer trace("QCashFileModel::CreateIndex", m_trace);
    return createIndex(row, column);
}
QVariant QCashFileModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    CFuncTracer trace("QCashFileModel::headerData", m_trace, false);
    try
    {
        trace.Trace("Role : %s, orientation : %s, section : %d"
                    , (role == Qt::DisplayRole)?"DisplayRole":
                        (role == Qt::DecorationRole)?"DecorationRole":
                        (role == Qt::EditRole)?"EditRole":
                        (role == Qt::ToolTipRole)?"ToolTipRole":
                        (role == Qt::StatusTipRole)?"StatusTipRole":
                        (role == Qt::WhatsThisRole)?"WhatsThisRole":
                        (role == Qt::FontRole)?"FontRole":
                        (role == Qt::TextAlignmentRole)?"TextAlignmentRole":
                        (role == Qt::ForegroundRole)?"ForegroundRole":
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
                        "Unknown"
                    , (orientation == Qt::Horizontal)?"Horizontal":
                        (orientation == Qt::Vertical)?"Vertical":
                        "Unknown"
                    , section);
        if (  (role != Qt::DisplayRole)
            ||(orientation != Qt::Horizontal))
            return {};

        std::map<int, std::string> mpCols = m_container->GetColumns();
        if (mpCols.find(section) != mpCols.end())
            return QString::fromStdString(mpCols[section]);
    }
    catch(std::exception& ex)
    {
        trace.Error("Exception occurred : %s", ex.what());
    }
    catch(...)
    {
        trace.Error("Exception occurred");
    }
    return {};
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

        trace.Trace("#rows : %ld", m_rows.count());
        const IData* data_row = m_rows.at(row);
        if (data_row != nullptr)
        {
            switch(role)
            {
                case Qt::DisplayRole:
                {
                return data_row->GetDataElement(column);
                }
                case Qt::BackgroundRole:
                {
                    QColor clr = data_row->GetBackGroundColor(column);
                    QBrush bgBrush(clr);
                    return bgBrush;
                }
                default:
                    break;
            }
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
long long QCashFileModel::CashCurrentPosition(int position)
{
    CFuncTracer trace("QCashFileModel::CashCurrentPosition", m_trace);
    try
    {
        if (position >= 0)
        {
            if (position > m_rows.lastIndex())
            {
                    if (position - m_rows.lastIndex() > lookAhead)
                cacheRows(position-halfLookAhead, qMin(m_count, position+halfLookAhead));
                    else while (position > m_rows.lastIndex())
                {
                    m_rows.append(fetchRow(m_rows.lastIndex() + 1));
                }
            } else if (position < m_rows.firstIndex()){
                    if (m_rows.firstIndex() - position > lookAhead)
                cacheRows( qMax(0, position-halfLookAhead), position+halfLookAhead);
                    else while (position < m_rows.firstIndex())
                {
                    m_rows.prepend(fetchRow(m_rows.firstIndex() - 1));
                }
            }
            return position;
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
    return -1;
}
long long QCashFileModel::rowGetNextSearchFoundItem(const QModelIndex& currentIdx)
{
    CFuncTracer trace("QCashFileModel::rowGetNextSearchFoundItem", m_trace);
    long long id = -1;
    try
    {
        id = m_container->GetNextRequiredText(currentIdx.row());
    }
    catch(std::exception& ex)
    {
        trace.Error("Exception occurred : %s", ex.what());
    }
    catch(...)
    {
        trace.Error("Exception occurred");
    }
    return id;
}
long long QCashFileModel::rowGetNextToggleMark(QModelIndex& currentIdx)
{
    CFuncTracer trace("QCashFileModel::rowGetNextToggleMark", m_trace);
    long long id = -1;
    try
    {
        id = m_container->GetNextToggleMark(currentIdx.row());
    }
    catch(std::exception& ex)
    {
        trace.Error("Exception occurred : %s", ex.what());
    }
    catch(...)
    {
        trace.Error("Exception occurred");
    }
    return id;
}

long long QCashFileModel::rowToggleMark(const QModelIndex &index, bool &bMark)
{
    CFuncTracer trace("QCashFileModel::rowToggleMark", m_trace);
    int id = -1;
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

        id = m_container->ToggleMark(row);
        if (id >= 0)
        {
            if (bMark == true)bMark = false; else bMark = true;
            m_rows.clear();

            trace.Trace("row %ld is changed", row);
            QModelIndex first = createIndex((row >  1)?(row - 1) : 0, 0);
            QModelIndex last = createIndex(((row + 2) < m_rows.lastIndex())?(row + 1) : row, (int)eColumns::eNumOfColumns);
            trace.Trace("Send signal that the row is changed (first:%ld, %ld  last:%ld, %ld", first.row(), first.column(), last.row(), last.column());
            emit dataChanged(first, last);
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
    return id;
}

void QCashFileModel::append(std::vector<IData *> dt)
{
    CFuncTracer trace("QCashFileModel::append", m_trace);
    try
    {
        QModelIndex topLeft = createIndex(m_container->RowCount(), 0);

        beginInsertRows({}, m_container->RowCount(), m_container->RowCount() + dt.size());
        m_container->append(dt);
        QModelIndex bottomRight = createIndex(m_container->RowCount(), (int)eColumns::eNumOfColumns);
        endInsertRows();
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

void QCashFileModel::clear()
{
    CFuncTracer trace("QCashFileModel::clear", m_trace);
    try
    {
        m_container->clear();
        m_rows.clear();
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

IData* QCashFileModel::getLogEntry(int index) const
{
    CFuncTracer trace("QCashFileModel::getLogEntry", m_trace);
    try
    {
        if (index > m_rows.lastIndex()){
            if (index - m_rows.lastIndex() > lookAhead)
            cacheRows(index-halfLookAhead, qMin(m_count, index+halfLookAhead));
            else while (index > m_rows.lastIndex())
            {
                m_rows.append(fetchRow(m_rows.lastIndex() + 1));
            }
        } else if (index < m_rows.firstIndex()){
            if (m_rows.firstIndex() - index > lookAhead)
            cacheRows( qMax(0, index-halfLookAhead), index+halfLookAhead);
            else while (index < m_rows.firstIndex())
            {
                m_rows.prepend(fetchRow(m_rows.firstIndex() - 1));
            }
        }

        return m_rows.at(index);
    }
    catch(std::exception& ex)
    {
        trace.Error("Exception occurred : %s", ex.what());
    }
    catch(...)
    {
        trace.Error("Exception occurred");
    }
    return nullptr;
}

void QCashFileModel::cacheRows(int from, int to) const
{
    CFuncTracer trace("QCashFileModel::cacheRows", m_trace, false);
    try
    {
        trace.Trace("Cash rows from %ld to %ld", from, to);
        for (int i= from; i < to; ++i)
        {
            IData* data = fetchRow(i);
            if (data)
                m_rows.insert(i, data);
            else
                trace.Error("Could not fetch at %ld", i);
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

IData* QCashFileModel::fetchRow(int position) const
{
    CFuncTracer trace("QCashFileModel::fetchRow", m_trace, false);
    IData* fetched_item = nullptr;
    try
    {
        trace.Trace("Fetch(%ld)", position);
        fetched_item = m_container->GetData(position);
    }
    catch(std::exception& ex)
    {
        trace.Error("Exception occurred : %s", ex.what());
    }
    catch(...)
    {
        trace.Error("Exception occurred");
    }
    return fetched_item;
}
