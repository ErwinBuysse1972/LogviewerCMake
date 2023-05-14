#include "QLogFileWidget.h"
#include "cfunctracer.h"

QLogFileWidget::QLogFileWidget(std::shared_ptr<CTracer> tracer, std::vector<CLogEntry> entries, QWidget *parent)
    : QWidget(parent)
    , m_tabindex(-1)
    , m_Layout(nullptr)
    , m_delegate(nullptr)
    , m_View(nullptr)
    , m_Model(nullptr)
    , m_cashModel(nullptr)
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

QLogFileWidget::~QLogFileWidget()
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
        if (m_cashModel != nullptr)
        {
            delete m_cashModel;
            m_cashModel = nullptr;
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

QLogFileModel *QLogFileWidget::GetModel(void)
{
    CFuncTracer trace("QLogFileWidget::GetModel", m_trace);
    return m_Model;
}

QCashFileModel *QLogFileWidget::GetCashModel()
{
    CFuncTracer trace("QLogFileWidget::GetCashModel", m_trace);
    return m_cashModel;
}
void QLogFileWidget::SetTabIndex(int index)
{
    CFuncTracer trace("QLogFileWidget::setTabIndex", m_trace);
    m_tabindex = index;
}
int QLogFileWidget::GetTabIndex(void)
{
    CFuncTracer trace("QLogFileWidget::GetTabIndex", m_trace);
    return m_tabindex;
}
void QLogFileWidget::SetFileName(std::string file)
{
    CFuncTracer trace("QLogFileWidget::SetFileName", m_trace);
    m_FileName = file;
}
std::string QLogFileWidget::GetFilename(void)
{
    CFuncTracer trace("QLogFileWidget::GetFileName", m_trace);
    return m_FileName;
}
QModelIndex QLogFileWidget::GetCurrentIndex(void)
{
    CFuncTracer trace("QLogFileWidget::GetCurrentIndex", m_trace);
    return m_View->currentIndex();
}
long long QLogFileWidget::ToggleMark(QModelIndex index, bool& bMark)
{
    CFuncTracer trace("QLogFileWidget::ToggleMark", m_trace);
    return m_Model->rowToggleMark(index, bMark);
}
void QLogFileWidget::GotoToNextMark(void)
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
void QLogFileWidget::GotoNextRequiredText(void)
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
void QLogFileWidget::GotoTopOfTable(void)
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
void QLogFileWidget::row_double_click(const QModelIndex &  index)
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
void QLogFileWidget::row_click(__attribute__((unused)) const QModelIndex &  index)
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
void QLogFileWidget::notifyStartDateTime(void)
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
void QLogFileWidget::notifyStopDateTime(void)
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
void QLogFileWidget::notifyProcessId(void)
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
void QLogFileWidget::notifyThreadID(void)
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
void QLogFileWidget::toggleMark(void)
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
void QLogFileWidget::disableClass(void)
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
void QLogFileWidget::disablefunction(void)
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
void QLogFileWidget::disableTraceLevel(void)
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

void QLogFileWidget::init_createActions(void)
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
void QLogFileWidget::init_createForm(std::vector<CLogEntry> entries)
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
