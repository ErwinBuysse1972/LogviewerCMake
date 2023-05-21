
#include <clogfile.h>
#include <QObject>
#include <QAbstractTableModel>
#include <QGridLayout>
#include <QTableView>
#include <QPushButton>
#include <QStyledItemDelegate>
#include <QItemDelegate>
#include <QSortFilterProxyModel>
#include <QInputDialog>
#include <QHeaderView>
#include <QColor>
#include <QMessageBox>
#include <QTextEdit>
#include <QPainter>
#include <QMouseEvent>
#include <QMenu>
#include <QAction>

#include <functional>
#include <list>

#include "cfunctracer.h"
#include "ctracer.h"
#include "cscopedtimer.h"

#include "qfilelogfilemodel.h"


QLogFileModel::QLogFileModel(std::shared_ptr<CTracer> tracer, QObject* parent)
    : QAbstractTableModel{parent}
    , m_trace(tracer)
{
    CFuncTracer trace("QLogFileModel::QLogFileModel", m_trace);
}
int QLogFileModel::rowCount(const QModelIndex &) const
{
    CFuncTracer trace("QLogFileModel::rowCount", m_trace, false);
    trace.Trace("#rows : %ld", m_entries.count());
    return m_entries.count();
}
int QLogFileModel::columnCount(const QModelIndex &) const
{
    CFuncTracer trace("QLogFileModel::columnCount", m_trace, false);
    trace.Trace("#Columns : %ld", static_cast<int>(eColumns::eNumOfColumns));
    return static_cast<int>(eColumns::eNumOfColumns);
}
QVariant QLogFileModel::data(const QModelIndex &index, int role) const
{
    CFuncTracer trace("QLogFileModel::data", m_trace, false);
    const IData* entry = m_entries.at(index.row());
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
    if (  (role == Qt::DisplayRole)
        ||(role == Qt::EditRole))
        return entry->GetDataElement(index.column());

    if (role == Qt::BackgroundRole)
        return entry->GetBackGroundColor(index.column());

    return QVariant();
}
QVariant QLogFileModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    CFuncTracer trace("QLogFileModel::headerData", m_trace, false);
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

        switch(section)
        {
        case (int)eColumns::eDateTime: return "Time";
        case (int)eColumns::eTraceLevel: return "level";
        case (int)eColumns::eProc: return "PID";
        case (int)eColumns::eThread: return "TID";
        case (int)eColumns::eClass: return "Class";
        case (int)eColumns::eFunction: return "Function";
        case (int)eColumns::eDescription: return "Description";
        default: return {};
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
    return {};
}
void QLogFileModel::append(std::vector<IData *> entries)
{
    CFuncTracer trace("QLogFileModel::append", m_trace);
    CScopeTimer timer("QLogFileModel::append", 0, [=, &trace](const std::string& msg){
        trace.Info("Timings:");
        trace.Info("    %s", msg.c_str());
    });
    try
    {
        timer.SetTime("01");
        beginInsertRows({}, m_entries.count(), m_entries.count() + entries.size());
        for(IData* entry : entries)
        {
            timer.SetTime("02");
            m_entries.append(entry);
            timer.SetTime("03");
        }
        endInsertRows();
        timer.SetTime("04");
        trace.Info("Relative time 02-03 : %s" , timer.GetRelativeTimes("02","03").c_str());

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
void QLogFileModel::clear()
{
    CFuncTracer trace("QLogFileModel::clear", m_trace);
    try
    {
        m_entries.clear();
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
long long QLogFileModel::rowToggleMark(const QModelIndex& index, bool& bMark)
{
    CFuncTracer trace("QLogFileModel::rowToggleMark", m_trace);
    long long id = -1;
    try
    {
        if (index.row() >= m_entries.size())
        {
            trace.Error("Row does not exist inside the model! (index : %ld)", index);
            return id;
        }

        CLogEntry* lpEntry = reinterpret_cast<CLogEntry *>(&m_entries[index.row()]);

        if (lpEntry != nullptr)
        {
            if (lpEntry->IsMarked())
            {
                lpEntry->SetMark( false );
                bMark = false;
            }
            else
            {
                lpEntry->SetMark( true );
                bMark = true;
            }
            trace.Trace("row %ld is changed (%lld)", index.row(), id);
            QModelIndex first = createIndex((index.row() >  1)?(index.row() - 1) : 0, 0);
            QModelIndex last = createIndex(((index.row() + 2) < m_entries.size())?(index.row() + 1) : index.row(), (int)eColumns::eNumOfColumns);
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
IData * QLogFileModel::getLogEntry(int index) const
{
    CFuncTracer trace("QLogFileModel::getLogEntry", m_trace);
    try
    {
        trace.Trace("index : %d", index);
        if (index >= m_entries.size())
        {
            trace.Error("Row does not exist inside the model! (index : %ld)", index);
            return nullptr;
        }
        return m_entries[index];
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
int QLogFileModel::rowGetNextToggleMark(QModelIndex& currentIdx)
{
    CFuncTracer trace("QLogFileModel::rowGetNextToggleMark", m_trace);
    try
    {
        if (currentIdx.row() < m_entries.size())
        {
            // From current position to the end of the file
            for (int idx = currentIdx.row() + 1; idx < m_entries.size(); idx++)
            {
                CLogEntry* lpEntry = reinterpret_cast<CLogEntry *>(&m_entries[idx]);
                if (lpEntry != nullptr)
                {
                    if (lpEntry->IsMarked() == true)
                        return idx;
                }
                else
                    trace.Error("IData is not of CLogEntry type");
            }
            // From the beginning of the file until the current position
            for (int idx = 0; idx < currentIdx.row(); idx++)
            {
                CLogEntry* lpEntry = reinterpret_cast<CLogEntry *>(&m_entries[idx]);
                if (lpEntry != nullptr)
                {
                    if (lpEntry->IsMarked() == true)
                        return idx;
                }
                else
                    trace.Error("IData is not of CLogEntry type");
            }
            trace.Error("No next mark found!");
        }
        else
            trace.Error("row is out of range: %d", currentIdx.row());
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
std::list<long long> QLogFileModel::IndicateSearchText(const std::string& text)
{
    CFuncTracer trace("QLogFileModel::IndicateSearchText", m_trace);
    int iCount = 0;
    int iRow = 0;
    std::list<long long> ids;
    std::list<int> rowsChanged;
    try
    {
        trace.Trace("Text : %s", text.c_str());
        std::for_each(  m_entries.begin(),
                        m_entries.end(),
                        [=, &trace, &text, &iCount, &iRow, &ids, &rowsChanged](IData* entry){
                            CLogEntry *lpEntry = reinterpret_cast<CLogEntry*>(entry);
                            if (lpEntry != nullptr)
                            {
                                if (lpEntry->Description().find(text) != std::string::npos)
                                {
                                    if (lpEntry->IsMarked() == false)
                                    {
                                        lpEntry->SetSearchMark(true, text);
                                        ids.push_back(lpEntry->GetID());
                                        rowsChanged.push_back(iRow);
                                        iCount++;
                                    }
                                }
                                else
                                {
                                    if (lpEntry->IsMarked() == true)
                                    {
                                        lpEntry->SetSearchMark(false, "");
                                        rowsChanged.push_back(iRow);
                                    }
                                }
                            }
                            ++iRow;
                        });
        trace.Trace("entries marked as required : %d", ids.size());
        //Signal which rows are changed
        std::for_each(rowsChanged.begin(), rowsChanged.end(), [=, &trace](int& row){
            trace.Trace("row %ld is changed", row);
            QModelIndex first = createIndex(row, 0);
            QModelIndex last = createIndex(row, (int)eColumns::eNumOfColumns);
            emit dataChanged(first, last);
        });
    }
    catch(std::exception& ex)
    {
        trace.Error("Exception occurred : %s", ex.what());
    }
    catch(...)
    {
        trace.Error("Exception occurred");
    }
    return ids;
}
int QLogFileModel::rowGetNextSearchFoundItem(const QModelIndex& currentIdx)
{
    CFuncTracer trace("QLogFileModel::rowGetNextSearchFoundItem", m_trace);
    try
    {
        if (currentIdx.row() < m_entries.size())
        {
            // From current position to the end of the file
            for (int idx = currentIdx.row() + 1; idx < m_entries.size(); idx++)
            {
                CLogEntry* lpEntry = reinterpret_cast<CLogEntry *>(m_entries[idx]);
                if (lpEntry != nullptr)
                {
                    if (lpEntry->IsEntryRequired() == true)
                    {
                        trace.Trace("Next item idx : %ld", idx);
                        return idx;
                    }
                }
                else
                    trace.Error("IData is not of CLogEntry type");
            }
            // From the beginning of the file until the current position
            for (int idx = 0; idx < currentIdx.row(); idx++)
            {
                CLogEntry* lpEntry = reinterpret_cast<CLogEntry *>(m_entries[idx]);
                if (lpEntry != nullptr)
                {
                    if (lpEntry->IsEntryRequired() == true)
                    {
                        trace.Trace("Next item idx : %ld", idx);
                        return idx;
                    }
                    else
                        trace.Error("IData is not of CLogEntry type");
                }
            }
            trace.Error("No next mark found!");
        }
        else
            trace.Error("row is out of range: %d", currentIdx.row());
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
QModelIndex QLogFileModel::CreateIndex(int row, int column)
{
    CFuncTracer trace("QLogFileModel::CreateIndex", m_trace);
    return createIndex(row, column);
}

