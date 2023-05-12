#pragma once
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

enum class eColumns
{
    eDateTime = 0,
    eTraceLevel = 1,
    eProc = 2,
    eThread = 3,
    eClass = 4,
    eFunction = 5,
    eDescription = 6,
    eNumOfColumns = 7
};


class QLogFileModel : public QAbstractTableModel
{
    Q_OBJECT

private:
    QList<CLogEntry> m_entries;
    std::shared_ptr<CTracer> m_trace;
    int pageLength; // Page nr of items that will be viewed in the current window
    int currentPage;
public:
    QLogFileModel(std::shared_ptr<CTracer> tracer, QObject* parent = {})
        : QAbstractTableModel{parent}
        , m_trace(tracer)
    {
        CFuncTracer trace("QLogFileModel::QLogFileModel", m_trace);
    }
    int rowCount(const QModelIndex &) const override
    {
        CFuncTracer trace("QLogFileModel::rowCount", m_trace, false);
        trace.Trace("#rows : %ld", m_entries.count());
        return m_entries.count();
    }
    int columnCount(const QModelIndex &) const override
    {
        CFuncTracer trace("QLogFileModel::columnCount", m_trace, false);
        trace.Trace("#Columns : %ld", static_cast<int>(eColumns::eNumOfColumns));
        return static_cast<int>(eColumns::eNumOfColumns);
    }
    QVariant data(const QModelIndex &index, int role) const override
    {
        CFuncTracer trace("QLogFileModel::data", m_trace, false);
        const CLogEntry& entry = m_entries[index.row()];
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
        {
            switch(index.column())
            {
                case (int)eColumns::eDateTime:
                    return QString::fromStdString(entry.Time());
                case (int)eColumns::eTraceLevel:
                    return QString::fromStdString(entry.Level());
                case (int)eColumns::eProc:
                    return QString::fromStdString(std::to_string(entry.GetProcId()));
                case (int)eColumns::eThread:
                    return QString::fromStdString(std::to_string(entry.GetThreadId()));
                case (int)eColumns::eClass:
                    return QString::fromStdString(entry.Class());
                case (int)eColumns::eFunction:
                    return QString::fromStdString(entry.FuncName());
                case (int)eColumns::eDescription:
                    return QString::fromStdString(entry.Description());
                default: return {};
            }
        }

        if (role == Qt::BackgroundRole)
        {
            QColor backcolor;
            backcolor.setRgb(255,255,255);
            if (entry.IsMarked())
                backcolor.setRgb(212,235,242);

            QBrush background(backcolor);
            return background;
        }

        if (role == Qt::UserRole)
        {
            if (index.row() < m_entries.count())
            {
                return QVariant::fromValue<CLogEntry>(m_entries[index.row()]);
            }
            else
                return {};
        }

        return {};
    }
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override
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
    void append(std::vector<CLogEntry> entries)
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
            for(const CLogEntry& entry : entries)
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
    void clear()
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
    long long rowToggleMark(const QModelIndex& index, bool& bMark)
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

            id = m_entries[index.row()].GetID();
            if (m_entries[index.row()].IsMarked())
            {
                m_entries[index.row()].SetMark( false );
                bMark = false;
            }
            else
            {
                m_entries[index.row()].SetMark( true );
                bMark = true;
            }
            trace.Trace("row %ld is changed (%lld)", index.row(), id);
            QModelIndex first = createIndex((index.row() >  1)?(index.row() - 1) : 0, 0);
            QModelIndex last = createIndex(((index.row() + 2) < m_entries.size())?(index.row() + 1) : index.row(), (int)eColumns::eNumOfColumns);
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
        return id;
    }
    std::optional<CLogEntry> getLogEntry(int index) const
    {
        CFuncTracer trace("QLogFileModel::getLogEntry", m_trace);
        try
        {
            trace.Trace("index : %d", index);
            if (index >= m_entries.size())
            {
                trace.Error("Row does not exist inside the model! (index : %ld)", index);
                return std::nullopt;
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
        return std::nullopt;
    }
    int rowGetNextToggleMark(QModelIndex& currentIdx)
    {
        CFuncTracer trace("QLogFileModel::rowGetNextToggleMark", m_trace);
        try
        {
            if (currentIdx.row() < m_entries.size())
            {
                // From current position to the end of the file
                for (int idx = currentIdx.row() + 1; idx < m_entries.size(); idx++)
                {
                    if (m_entries[idx].IsMarked() == true)
                        return idx;
                }
                // From the beginning of the file until the current position
                for (int idx = 0; idx < currentIdx.row(); idx++)
                {
                    if (m_entries[idx].IsMarked() == true)
                        return idx;
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
    std::list<long long> IndicateSearchText(const std::string& text)
    {
        CFuncTracer trace("QLogFileModel::IndicateSearchText", m_trace);
        int iCount = 0;
        int iRow = 0;
        std::list<long long> ids;
        std::list<int> rowsChanged;
        try
        {
            trace.Trace("Text : %s", text.c_str());
            std::for_each(m_entries.begin(), m_entries.end(), [=, &trace, &text, &iCount, &iRow, &ids, &rowsChanged](CLogEntry& entry){
                if (entry.Description().find(text) != std::string::npos)
                {
                    if (entry.IsMarked() == false)
                    {
                        entry.SetSearchMark(true, text);
                        ids.push_back(entry.GetID());
                        rowsChanged.push_back(iRow);
                        iCount++;
                    }
                }
                else
                {
                    if (entry.IsMarked() == true)
                    {
                        entry.SetSearchMark(false, "");
                        rowsChanged.push_back(iRow);
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
    int rowGetNextSearchFoundItem(const QModelIndex& currentIdx)
    {
        CFuncTracer trace("QLogFileModel::rowGetNextSearchFoundItem", m_trace);
        try
        {
            if (currentIdx.row() < m_entries.size())
            {
                // From current position to the end of the file
                for (int idx = currentIdx.row() + 1; idx < m_entries.size(); idx++)
                {
                    if (m_entries[idx].IsEntryRequired() == true)
                    {
                        trace.Trace("Next item idx : %ld", idx);
                        return idx;
                    }
                }
                // From the beginning of the file until the current position
                for (int idx = 0; idx < currentIdx.row(); idx++)
                {
                    if (m_entries[idx].IsEntryRequired() == true)
                    {
                        trace.Trace("Next item idx : %ld", idx);
                        return idx;
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
    QModelIndex CreateIndex(int row, int column)
    {
        CFuncTracer trace("QLogFileModel::CreateIndex", m_trace);
        return createIndex(row, column);
    }
};

class RichTextDelegate : public QItemDelegate
{
        Q_OBJECT
public:
    RichTextDelegate(std::shared_ptr<CTracer> tracer, QObject *parent)
        : QItemDelegate(parent)
        , m_trace(tracer)
    {
        CFuncTracer trace("RichTextDelegate::RichTextDelegate", m_trace);
    }
    void paint(QPainter *painter,
               const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        CFuncTracer trace("RichTextDelegate::paint", m_trace);

        CLogEntry entry = index.data(Qt::UserRole).value<CLogEntry>();
        trace.Trace("col :%ld, row:%ld, entry: %s",index.column(), index.row(), entry.Time().c_str());
        trace.Trace("Mark : %s, Required : %s, required text : %s"
                    , (entry.IsMarked() == true)?"TRUE":"FALSE"
                    , (entry.IsEntryRequired() == true)?"TRUE":"FALSE"
                    , (entry.GetRequiredText().c_str()));

        if (   (index.column() == (int)eColumns::eDescription)
            &&(entry.IsEntryRequired()) )
        {
            trace.Trace("text should be placed in red: %s", entry.Description().c_str());
            painter->save();
            QStyleOptionViewItem opt = setOptions(index, option);
            drawBackground(painter, opt, index);

            painter->setFont(opt.font);
            painter->setPen(Qt::red);
            opt.backgroundBrush = QBrush(QColor(230,200,200));
            opt.font.setBold(true);
            opt.font.setUnderline(true);

            drawBackground(painter, opt, index);
            painter->setFont(opt.font);
            painter->drawText(opt.rect, opt.displayAlignment, QString::fromStdString(entry.Description().c_str()));
            drawFocus(painter, opt, option.rect);
            painter->restore();
        }
        else
            QItemDelegate::paint(painter, option, index);
    }
    QSize sizeHint(const QStyleOptionViewItem &option,
                  const QModelIndex &index) const
    {
        CFuncTracer trace("RichTextDelegate::sizeHint", m_trace);

        return QItemDelegate::sizeHint(option, index);
    }

    QWidget *createEditor(QWidget *parent,
                          const QStyleOptionViewItem &option,
                          const QModelIndex &index) const
    {
        CFuncTracer trace("RichTextDelegate::createEditor", m_trace);
        try
        {
            if (index.column() == (int)eColumns::eDescription)
                return new QTextEdit();
        }
        catch(std::exception& ex)
        {
            trace.Error("Exception occurred : %s", ex.what());
        }
        catch(...)
        {
            trace.Error("Exception occurred");
        }
        return QItemDelegate::createEditor(parent, option, index);
    }
    void setEditorData(QWidget *editor, const QModelIndex &index) const
    {
        CFuncTracer trace("RichTextDelegate::setEditorData", m_trace);
        QItemDelegate::setEditorData(editor, index);
    }
    void setModelData(QWidget *editor,
                      QAbstractItemModel *model,
                      const QModelIndex &index) const
    {
        CFuncTracer trace("RichTextDelegate::setModelData", m_trace);
        try
        {
            QLogFileModel *logModel = qobject_cast<QLogFileModel *>(model);
            if (logModel != nullptr)
            {
                auto entry = logModel->getLogEntry(index.row());
                if(entry)
                {
                    if (entry->IsEntryRequired() == true)
                    {
                        QTextEdit *editText = qobject_cast<QTextEdit *>(editor);
                        if (editText)
                        {
                            QList<QTextEdit::ExtraSelection> selections;
                            QTextCharFormat fmt;
                            fmt.setUnderlineStyle(QTextCharFormat::SingleUnderline);
                            fmt.setForeground(QBrush(QColor(200,0,0)));
                            fmt.setUnderlineColor(QColor(200,0,0));
                            fmt.setBackground(QBrush(QColor(230,200,200)));
                            QTextCursor cursor = editText->textCursor();

                            while (! (cursor = editText->document()->find(QString::fromStdString(entry->GetRequiredText()), cursor)).isNull())
                            {
                                QTextEdit::ExtraSelection sel = {cursor, fmt };
                                selections.append(sel);
                            }

                            // set, show, go!
                            editText->setExtraSelections(selections);
                        }
                    }
                }
                else
                    trace.Error("entry is not available for index: %ld", index.row());
            }
            else
                trace.Error("model is not a QLogFileModel!");
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

    void updateEditorGeometry(QWidget *editor,
                              const QStyleOptionViewItem &option,
                              const QModelIndex &index) const
    {
        CFuncTracer trace("RichTextDelegate::updateEditorGeometry", m_trace);
        QItemDelegate::updateEditorGeometry(editor, option, index);
    }
protected:
    bool eventFilter(QObject *object, QEvent *event)
    {
        CFuncTracer trace("RichTextDelegate::eventFilter", m_trace);
        return QItemDelegate::eventFilter(object, event);
    }
    bool editorEvent(QEvent *event, QAbstractItemModel *model,
                     const QStyleOptionViewItem &option, const QModelIndex &index)
    {
        CFuncTracer trace("RichTextDelegate::editorEvent", m_trace);
        return  QItemDelegate::editorEvent(event, model, option, index);
    }


private:
    std::shared_ptr<CTracer> m_trace;

};

class QLogfileTableView : public QTableView
{
public:
    explicit QLogfileTableView(QWidget *parent = nullptr)
        : QTableView(parent)
        , m_onCellClickRight(nullptr)
        , m_onCellClickLeft(nullptr)
    {}
    virtual ~QLogfileTableView(){}
    void RegisterRightClick(std::function<void(QModelIndex, QPoint)> event){ m_onCellClickRight = event;}
    void RegisterLeftClick(std::function<void(QModelIndex, QPoint)> event){ m_onCellClickLeft = event;}

protected:
    void mousePressEvent(QMouseEvent *event) override{
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

private:
    std::function<void(QModelIndex, QPoint)> m_onCellClickRight;
    std::function<void(QModelIndex, QPoint)> m_onCellClickLeft;
};

class QLogFileWidget : public QWidget
{
    Q_OBJECT

public:
    explicit QLogFileWidget(std::shared_ptr<CTracer> tracer, std::vector<CLogEntry> entries, __attribute__((unused))QWidget *parent = nullptr)
        : m_tabindex(-1)
        , m_Layout(nullptr)
        , m_delegate(nullptr)
        , m_View(nullptr)
        , m_Model(nullptr)
        , m_trace(tracer)
        , actNotifyStartDateTime(nullptr)
        , actNotifyStopDateTime(nullptr)
        , actNotifyProcessID(nullptr)
        , actNotifyThreadID(nullptr)
        , actNotifyMarkToggle(nullptr)
        , actNotifyDisableClass(nullptr)
        , actNotifyDisableFunction(nullptr)
        , actNotifyDisableTraceLevel(nullptr)
        , m_notStartTime(nullptr)
        , m_notStopTime(nullptr)
        , m_notProcId(nullptr)
        , m_notThreadId(nullptr)
        , m_notMarkToggle(nullptr)
        , m_notDisableClass(nullptr)
        , m_notDisableFunc(nullptr)
        , m_notDisableTraceLevel(nullptr)
    {
        CFuncTracer trace("QLogFileWidget::QLogFileWidget", tracer);
        try
        {
            init_createActions();
            init_createForm(entries);
            connect(m_View, &QTableView::clicked, this, &QLogFileWidget::row_click);
            connect(m_View, &QTableView::doubleClicked, this, &QLogFileWidget::row_double_click);
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

    ~QLogFileWidget()
    {
        CFuncTracer trace("QLogFileModel~QLogFileModel", m_trace);
        try
        {
            if (m_Layout != nullptr)
            {
                delete m_Layout;
                m_Layout = nullptr;
            }
            if (m_View != nullptr)
            {
                delete m_View;
                m_View = nullptr;
            }
            if (m_delegate)
            {
                delete m_delegate;
                m_delegate = nullptr;
            }
            if (m_Model != nullptr)
            {
                delete m_Model;
                m_Model = nullptr;
            }
        }
        catch(std::exception& ex)
        {
            trace.Error("Exception occurred: %s", ex.what());
        }
        catch(...)
        {
            trace.Error("Exception occurred");
        }

    }

    QLogFileModel *GetModel(void)
    {
        CFuncTracer trace("QLogFileWidget::GetModel", m_trace);
        return m_Model;
    }
    void SetTabIndex(int index)
    {
        CFuncTracer trace("QLogFileWidget::setTabIndex", m_trace);
        m_tabindex = index;
    }
    int GetTabIndex(void)
    {
        CFuncTracer trace("QLogFileWidget::GetTabIndex", m_trace);
        return m_tabindex;
    }
    void SetFileName(std::string file)
    {
        CFuncTracer trace("QLogFileWidget::SetFileName", m_trace);
        m_FileName = file;
    }
    std::string GetFilename(void)
    {
        CFuncTracer trace("QLogFileWidget::GetFileName", m_trace);
        return m_FileName;
    }
    QModelIndex GetCurrentIndex(void)
    {
        CFuncTracer trace("QLogFileWidget::GetCurrentIndex", m_trace);
        return m_View->currentIndex();
    }
    long long ToggleMark(QModelIndex index, bool& bMark)
    {
        CFuncTracer trace("QLogFileWidget::ToggleMark", m_trace);
        return m_Model->rowToggleMark(index, bMark);
    }
    void GotoToNextMark(void)
    {
        CFuncTracer trace("QLogFileWidget::GotoToNextMark", m_trace);
        try
        {
            // Find next mark
            QModelIndex index = m_View->currentIndex();
            int row = m_Model->rowGetNextToggleMark(index);
            if (row >= 0)
            {
                QModelIndex nextMark = m_Model->CreateIndex(row, 0);
                m_View->setCurrentIndex(nextMark);
            }
            else
            {
                QMessageBox::warning(this, "WARNING - No Marks", "No next marks were found",
                                     QMessageBox::StandardButton::Ok, QMessageBox::StandardButton::NoButton);
                trace.Error("No next Mark found!");
            }
        }
        catch(std::exception& ex)
        {
            trace.Error("Exception occured : %s", ex.what());
        }
        catch(...)
        {
            trace.Error("Exception occurred");
        }
    }
    void GotoNextRequiredText(void)
    {
        CFuncTracer trace("QLogFileWidget::GotoNextRequiredText", m_trace);
        try
        {
            // Find next mark
            QModelIndex index = m_View->currentIndex();
            int row = m_Model->rowGetNextSearchFoundItem(index);
            if (row >= 0)
            {
                QModelIndex nextMark = m_Model->CreateIndex(row, 0);
                m_View->setCurrentIndex(nextMark);
            }
            else
            {
                trace.Error("No next search found!");
            }
        }
        catch(std::exception& ex)
        {
            trace.Error("Exception occured : %s", ex.what());
        }
        catch(...)
        {
            trace.Error("Exception occurred");
        }
    }
    void GotoTopOfTable(void)
    {
        CFuncTracer trace("QLogFileWidget::GotoTopOfTable", m_trace);
        try
        {
            QModelIndex top = m_Model->CreateIndex(0,0);
            m_View->setCurrentIndex(top);
        }
        catch(std::exception& ex)
        {
            trace.Error("Exception occured : %s", ex.what());
        }
        catch(...)
        {
            trace.Error("Exception occurred");
        }
    }
    void RegisterDropDown_StartTime(std::function<void (const std::string&)> event){ m_notStartTime = event; }
    void RegisterDropDown_StopTime(std::function<void (const std::string&)> event){ m_notStopTime = event; }
    void RegisterDropDown_ProcID(std::function<void (const std::string&)> event){ m_notProcId = event; }
    void RegisterDropDown_ThreadID(std::function<void (const std::string&)> event){ m_notThreadId = event; }
    void RegisterDropDown_MarkToggle(std::function<void (QModelIndex)> event){ m_notMarkToggle = event; }
    void RegisterDropDown_DisableClass(std::function<void (const std::string&)> event){ m_notDisableClass = event; }
    void RegisterDropDown_DisableFunc(std::function<void (const std::string&)> event){ m_notDisableFunc = event; }
    void RegisterDropDown_DisableTraceLevel(std::function<void (const std::string&)> event){ m_notDisableTraceLevel = event; }

signals:
    void RowDoubleClicked(const CLogEntry& index);

private slots:
    void row_double_click(const QModelIndex &  index)
    {
        CFuncTracer trace("QLogFileWidget::row_double_click", m_trace);
        try
        {
            trace.Trace("index : %ld", index);
            std::optional<CLogEntry> entry = GetModel()->getLogEntry(index.row());
            if (entry)
            {
                emit RowDoubleClicked(*entry);
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
    void row_click(__attribute__((unused)) const QModelIndex &  index)
    {
        CFuncTracer trace("QLogFileWidget::row_click", m_trace);
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
    void notifyStartDateTime(void)
    {
        CFuncTracer trace("QLogFileWidget::", m_trace, false);
        if (m_notStartTime != nullptr)
        {
            std::optional<CLogEntry> entry = m_Model->getLogEntry(m_notIndex.row());
            if (entry)
            {
                std::string time = entry->Time();
                trace.Trace("time : %s", time.c_str());
                m_notStartTime(time);
            }
            else
                trace.Error("entry on %ld row does not exist", m_notIndex.row());
        }
        else
            trace.Error("disable tracelevel dropdown is not registered");
    }
    void notifyStopDateTime(void)
    {
        CFuncTracer trace("QLogFileWidget::", m_trace, false);
        if (m_notStopTime != nullptr)
        {
            std::optional<CLogEntry> entry = m_Model->getLogEntry(m_notIndex.row());
            if (entry)
            {
                std::string time = entry->Time();
                trace.Trace("time : %s", time.c_str());
                m_notStopTime(time);
            }
            else
                trace.Error("entry on %ld row does not exist", m_notIndex.row());
        }
        else
            trace.Error("disable tracelevel dropdown is not registered");
    }
    void notifyProcessId(void)
    {
        CFuncTracer trace("QLogFileWidget::", m_trace, false);
        if (m_notProcId != nullptr)
        {
            std::optional<CLogEntry> entry = m_Model->getLogEntry(m_notIndex.row());
            if (entry)
            {
                std::string id = entry->ProcID();
                trace.Trace("id : %s", id.c_str());
                m_notProcId(id);
            }
            else
                trace.Error("entry on %ld row does not exist", m_notIndex.row());
        }
        else
            trace.Error("disable tracelevel dropdown is not registered");
    }
    void notifyThreadID(void)
    {
        CFuncTracer trace("QLogFileWidget::", m_trace, false);
        if (m_notThreadId != nullptr)
        {
            std::optional<CLogEntry> entry = m_Model->getLogEntry(m_notIndex.row());
            if (entry)
            {
                std::string id = entry->ThreadID();
                trace.Trace("id : %s", id.c_str());
                m_notThreadId(id);
            }
            else
                trace.Error("entry on %ld row does not exist", m_notIndex.row());
        }
        else
            trace.Error("disable tracelevel dropdown is not registered");
    }
    void toggleMark(void)
    {
        CFuncTracer trace("QLogFileWidget::", m_trace, false);
        if (m_notMarkToggle != nullptr)
        {
            trace.Trace("row : %ld", m_notIndex.row());
            m_notMarkToggle(m_notIndex);
        }
        else
            trace.Error("disable tracelevel dropdown is not registered");
    }
    void disableClass(void)
    {
        CFuncTracer trace("QLogFileWidget::", m_trace, false);
        if (m_notDisableClass != nullptr)
        {
            std::optional<CLogEntry> entry = m_Model->getLogEntry(m_notIndex.row());
            if (entry)
            {
                std::string cls = entry->Class();
                trace.Trace("Class : %s", cls.c_str());
                m_notDisableClass(cls);
            }
            else
                trace.Error("entry on %ld row does not exist", m_notIndex.row());
        }
        else
            trace.Error("disable tracelevel dropdown is not registered");
    }
    void disablefunction(void)
    {
        CFuncTracer trace("QLogFileWidget::", m_trace, false);
        if (m_notDisableFunc != nullptr)
        {
            std::optional<CLogEntry> entry = m_Model->getLogEntry(m_notIndex.row());
            if (entry)
            {
                std::string funcName = entry->FuncName();
                trace.Trace("funcname : %s", funcName.c_str());
                m_notDisableClass(entry->FuncName());
            }
            else
                trace.Error("entry on %ld row does not exist", m_notIndex.row());
        }
        else
            trace.Error("disable tracelevel dropdown is not registered");
    }
    void disableTraceLevel(void)
    {
        CFuncTracer trace("QLogFileWidget::disableTraceLevel", m_trace, false);
        if (m_notDisableTraceLevel != nullptr)
        {
            std::optional<CLogEntry> entry = m_Model->getLogEntry(m_notIndex.row());
            if (entry)
            {
                std::string level = entry->Level();
                trace.Trace("level : %s", level.c_str());
                m_notDisableTraceLevel(level);
            }
            else
                trace.Error("entry on %ld row does not exist", m_notIndex.row());
        }
        else
            trace.Error("disable tracelevel dropdown is not registered");
    }

    private:
    int m_tabindex;
    std::string m_FileName;

    QModelIndex m_notIndex;
    QHBoxLayout *m_Layout;
    RichTextDelegate *m_delegate;
    QLogfileTableView *m_View;
    QLogFileModel *m_Model;
    std::shared_ptr<CTracer> m_trace;
    QAction *actNotifyStartDateTime;
    QAction *actNotifyStopDateTime;
    QAction *actNotifyProcessID;
    QAction *actNotifyThreadID;
    QAction *actNotifyMarkToggle;
    QAction *actNotifyDisableClass;
    QAction *actNotifyDisableFunction;
    QAction *actNotifyDisableTraceLevel;

    std::function<void (const std::string&)>    m_notStartTime;
    std::function<void (const std::string&)>    m_notStopTime;
    std::function<void (const std::string&)>    m_notProcId;
    std::function<void (const std::string&)>    m_notThreadId;
    std::function<void (QModelIndex)>           m_notMarkToggle;
    std::function<void (const std::string&)>    m_notDisableClass;
    std::function<void (const std::string&)>    m_notDisableFunc;
    std::function<void (const std::string&)>    m_notDisableTraceLevel;

    void init_createActions(void)
    {
        actNotifyStartDateTime = new QAction(tr("Filter start time "), this);
        connect(actNotifyStartDateTime, &QAction::triggered, this, &QLogFileWidget::notifyStartDateTime);
        actNotifyStopDateTime = new QAction(tr("Filter stop time"), this);
        connect(actNotifyStopDateTime, &QAction::triggered, this, &QLogFileWidget::notifyStopDateTime);
        actNotifyProcessID = new QAction(tr("Filter process ID"), this);
        connect(actNotifyProcessID, &QAction::triggered, this, &QLogFileWidget::notifyProcessId);
        actNotifyThreadID = new QAction(tr("Filter thread ID"), this);
        connect(actNotifyThreadID, &QAction::triggered, this, &QLogFileWidget::notifyThreadID);
        actNotifyMarkToggle = new QAction(tr("Toggle Mark"), this);
        connect(actNotifyMarkToggle, &QAction::triggered, this, &QLogFileWidget::toggleMark);
        actNotifyDisableClass = new QAction(tr("Filter class"), this);
        connect(actNotifyDisableClass, &QAction::triggered, this, &QLogFileWidget::disableClass);
        actNotifyDisableFunction = new QAction(tr("Filter function"), this);
        connect(actNotifyDisableFunction, &QAction::triggered, this, &QLogFileWidget::disablefunction);
        actNotifyDisableTraceLevel = new QAction(tr("Filter tracelevel"), this);
        connect(actNotifyDisableTraceLevel, &QAction::triggered, this, &QLogFileWidget::disableTraceLevel);
    }
    void init_createForm(std::vector<CLogEntry> entries)
    {
        CFuncTracer trace("QLogFileWidget::init_createForm", m_trace);
        try
        {
            m_delegate = new RichTextDelegate(m_trace, this);
            m_View = new QLogfileTableView(this);
            m_View->RegisterRightClick([=](QModelIndex index, QPoint pos){
                m_notIndex = index;
                QMenu menu(this);
                if (actNotifyMarkToggle != nullptr)
                    menu.addAction(actNotifyMarkToggle);
                if (actNotifyStartDateTime != nullptr)
                    menu.addAction(actNotifyStartDateTime);
                if (actNotifyStopDateTime != nullptr)
                    menu.addAction(actNotifyStopDateTime);
                if (actNotifyProcessID != nullptr)
                    menu.addAction(actNotifyProcessID);
                if (actNotifyThreadID != nullptr)
                    menu.addAction(actNotifyThreadID);
                if (actNotifyDisableFunction != nullptr)
                    menu.addAction(actNotifyDisableFunction);
                if (actNotifyDisableTraceLevel != nullptr)
                    menu.addAction(actNotifyDisableTraceLevel);
                if (actNotifyDisableClass != nullptr)
                    menu.addAction(actNotifyDisableClass);
                menu.exec(pos);
            });
            m_Model = new QLogFileModel(m_trace, this);
            m_Layout = new QHBoxLayout(this);


            m_Model->append(entries);
            m_View->setModel(m_Model);
            m_View->setItemDelegate(m_delegate);
            m_View->resizeColumnsToContents();
            m_View->setStyleSheet("QTableView:disabled{color:grey;}QTableView:enabled{color:black;font-weight:normal;}");
            m_View->setAlternatingRowColors(true);
            m_View->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);


            m_Layout->setContentsMargins(0,0,0,0);
            m_Layout->addWidget(m_View);
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
};
