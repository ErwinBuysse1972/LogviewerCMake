#include "QLogFileWidget.h"
#include "cfunctracer.h"
#include "cscopedtimer.h"

QLogFileWidget::QLogFileWidget(std::shared_ptr<CTracer> tracer, std::vector<IData *> entries, QWidget *parent)
    : QWidget(parent)
    , m_tabindex(-1)
    , m_Layout(nullptr)
    , m_delegate(nullptr)
    , m_View(nullptr)
    , m_Model(nullptr)
    , m_container(nullptr)
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
        if (m_container)
        {
            delete m_container;
            m_container = nullptr;
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
    if (m_Model != nullptr)
    {
        return m_Model->rowToggleMark(index, bMark);
    }
    else if (m_cashModel != nullptr)
    {
        return m_cashModel->rowToggleMark(index, bMark);
    }
    return -1;
}
void QLogFileWidget::GotoToNextMark(void)
{
    CFuncTracer trace("QLogFileWidget::GotoToNextMark", m_trace);
    try
    {
        // Find next mark
        QModelIndex index = m_View->currentIndex();
        if (m_Model != nullptr)
        {
            int row = m_Model->rowGetNextToggleMark(index);
            if (row >= 0)
            {
                QModelIndex nextMark = m_Model->CreateIndex(row, 0);
                m_View->setCurrentIndex(nextMark);
                return;
            }
        }
        else if (m_cashModel != nullptr)
        {
            int row = m_cashModel->rowGetNextToggleMark(index);
            if (row >= 0)
            {
                QModelIndex nextMark = m_cashModel->CreateIndex(row, 0);
                if (m_cashModel->CashCurrentPosition(row) < 0)
                    trace.Error("Couldn't cash the position %ld", row);
                else
                    m_View->setCurrentIndex(nextMark);
                return;
            }
        }
        QMessageBox::warning(this, "WARNING - No Marks", "No next marks were found",
                             QMessageBox::StandardButton::Ok, QMessageBox::StandardButton::NoButton);
        trace.Error("No next Mark found!");
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
    CFuncTracer trace("QLogFileWidget::GotoNextRequiredText", m_trace, false);
    try
    {
        // Find next mark
        QModelIndex index = m_View->currentIndex();
        if (m_Model != nullptr)
        {
            if (index.row() == -1)
                index = m_Model->CreateIndex(0,0);

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
        else if (m_cashModel != nullptr)
        {
            if (index.row() == -1)
                index = m_cashModel->CreateIndex(0,0);

            trace.Info("current index : %ld(row), %ld(column)", index.row(), index.column());
            int row = m_cashModel->rowGetNextSearchFoundItem(index);
            if (row >= 0)
            {
                QModelIndex nextMark = m_cashModel->CreateIndex(row, 0);
                if (m_cashModel->CashCurrentPosition(row) < 0)
                    trace.Error("Could not cash position %ld", row);
                else
                {
                    trace.Info("Set CurrentIndex: %ld (row), %ld (column)", nextMark.row(), nextMark.column());
                    m_View->setCurrentIndex(nextMark);
                }
            }
            else
            {
                trace.Error("No next search found!");
            }
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
        QModelIndex top;
        if (m_Model != nullptr)
        {
            top = m_Model->CreateIndex(0,0);
            m_View->setCurrentIndex(top);
        }
        else if (m_cashModel != nullptr)
        {
            top = m_cashModel->CreateIndex(0,0);
            if (m_cashModel->CashCurrentPosition(top.row()) < 0)
                trace.Error("Could not cash position %ld", top.row());
            else
                m_View->setCurrentIndex(top);
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
void QLogFileWidget::row_double_click(const QModelIndex &  index)
{
    CFuncTracer trace("QLogFileWidget::row_double_click", m_trace);
    try
    {
        trace.Trace("index : %ld", index);
        if (GetModel() != nullptr)
        {
            IData *entry = GetModel()->getLogEntry(index.row());
            if (entry != nullptr)
            {
                CLogEntry* lpEntry = reinterpret_cast<CLogEntry*>(entry);
                emit RowDoubleClicked(lpEntry);
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
        IData* entry;
        if (m_Model != nullptr)
            entry = m_Model->getLogEntry(m_notIndex.row());
        if (m_cashModel != nullptr)
            entry = m_cashModel->getLogEntry(m_notIndex.row());

        if (entry != nullptr)
        {
            CLogEntry* lpEntry = reinterpret_cast<CLogEntry *>(&entry);
            std::string time = lpEntry->Time();
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
        IData* entry;
        if (m_Model != nullptr)
            entry = m_Model->getLogEntry(m_notIndex.row());
        if (m_cashModel != nullptr)
            entry = m_cashModel->getLogEntry(m_notIndex.row());

        if (entry != nullptr)
        {
            CLogEntry* lpEntry = reinterpret_cast<CLogEntry *>(entry);
            if (lpEntry != nullptr)
            {
                std::string time = lpEntry->Time();
                trace.Trace("time : %s", time.c_str());
                m_notStopTime(time);
            }
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
        IData* entry;
        if (m_Model != nullptr)
            entry = m_Model->getLogEntry(m_notIndex.row());
        if (m_cashModel != nullptr)
            entry = m_cashModel->getLogEntry(m_notIndex.row());
        if (entry != nullptr)
        {
            CLogEntry* lpEntry = reinterpret_cast<CLogEntry *>(entry);
            if (lpEntry != nullptr)
            {
                std::string id = lpEntry->ProcID();
                trace.Trace("id : %s", id.c_str());
                m_notProcId(id);
            }
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
        IData* entry;
        if( m_Model != nullptr)
            entry = m_Model->getLogEntry(m_notIndex.row());
        if ( m_cashModel != nullptr)
            entry = m_cashModel->getLogEntry(m_notIndex.row());
        if (entry != nullptr)
        {
            CLogEntry* lpEntry = reinterpret_cast<CLogEntry *>(entry);
            if (lpEntry != nullptr)
            {
                std::string id = lpEntry->ThreadID();
                trace.Trace("id : %s", id.c_str());
                m_notThreadId(id);
            }
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
        IData* entry;
        if (m_Model != nullptr)
            entry = m_Model->getLogEntry(m_notIndex.row());
        if (m_cashModel != nullptr)
            entry = m_cashModel->getLogEntry(m_notIndex.row());
        if (entry != nullptr)
        {
            CLogEntry* lpEntry = reinterpret_cast<CLogEntry *>(entry);
            if (lpEntry != nullptr)
            {
                std::string cls = lpEntry->Class();
                trace.Trace("Class : %s", cls.c_str());
                m_notDisableClass(cls);
            }
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
        IData* entry;
        if (m_Model != nullptr)
            entry = m_Model->getLogEntry(m_notIndex.row());
        if (m_cashModel != nullptr)
            entry = m_cashModel->getLogEntry(m_notIndex.row());
        if (entry != nullptr)
        {
            CLogEntry* lpEntry = reinterpret_cast<CLogEntry *>(entry);
            if (lpEntry != nullptr)
            {
                std::string funcName = lpEntry->FuncName();
                trace.Trace("funcname : %s", funcName.c_str());
                m_notDisableClass(lpEntry->FuncName());
            }
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
        IData* entry;
        if (m_Model != nullptr)
            entry = m_Model->getLogEntry(m_notIndex.row());
        if (m_cashModel != nullptr)
            entry = m_cashModel->getLogEntry(m_notIndex.row());

        if (entry != nullptr)
        {
            CLogEntry* lpEntry = reinterpret_cast<CLogEntry *>(entry);
            if (lpEntry != nullptr)
            {
                std::string level = lpEntry->Level();
                trace.Trace("level : %s", level.c_str());
                m_notDisableTraceLevel(level);
            }
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
void QLogFileWidget::init_createForm(std::vector<IData *> entries)
{
    CFuncTracer trace("QLogFileWidget::init_createForm", m_trace);
    CScopeTimer timer("QLogFileWidget::init_createForm", 0, [=, &trace](const std::string& msg)
                      {
                          trace.Info("Timings : ");
                          trace.Info("   %s", msg.c_str());
                      });

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

//        m_Model = new QLogFileModel(m_trace, this);

        m_container = new CLogEntryContainer(m_trace);
        m_cashModel = new QCashFileModel(m_trace,
                                         m_container,
                                         this);
        m_Layout = new QHBoxLayout(this);


        if (m_Model != nullptr)
        {
            if (entries.size() > 0)
                m_Model->append(entries);
            m_View->setModel(m_Model);
        }
        else if (m_cashModel != nullptr)
        {
            if (entries.size() > 0)
                m_cashModel->append(entries);
            m_View->setModel(m_cashModel);
        }

        m_View->setItemDelegate(m_delegate);
        m_View->setStyleSheet("QTableView:disabled{color:grey;}QTableView:enabled{color:black;font-weight:normal;}");
        m_View->setAlternatingRowColors(true);
        m_View->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
        m_View->horizontalHeader()->setStretchLastSection(true);
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
