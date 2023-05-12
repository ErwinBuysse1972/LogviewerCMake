#include "logviewer.h"

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QWidget>

#include <QDebug>
#include <QtGui>
#include <QFileDialog>
#include <QMenuBar>
#include <QMenu>
#include <QTextEdit>
#include <QRegularExpression>
#include <QTime>
#include <cfunctracer.h>
#include <ctracer.h>
#include <cscopedtimer.h>
#include <clogfile.h>
#include <cconversion.h>

#include "QLogFileWidget.h"
#include <clogfile.h>
#include <cconversion.h>
#include "searchform.h"
#include "logentryview.h"


LogViewer::LogViewer(std::shared_ptr<CTracer> tracer, QWidget *parent)
    : QWidget(parent)
    , m_hlayout(nullptr)
    , m_vlayout(nullptr)
    , menuBar(nullptr)
    , fileMenu(nullptr)
    , actOpen(nullptr)
    , actClose(nullptr)
    , actSave(nullptr)
    , toolsMenu(nullptr)
    , actToggleMark(nullptr)
    , actGotoNextMark(nullptr)
    , actClearAllMarks(nullptr)
    , actSearchText(nullptr)
    , actFindNextText(nullptr)
    , tabWidget(nullptr)
    , gbxLevel(nullptr)
    , lblLevel(nullptr)
    , cboLevel(nullptr)
    , cbo_levelModel(nullptr)
    , btnLevelFiltering(nullptr)
    , gbxFunctionFiltering(nullptr)
    , cboClass(nullptr)
    , cbo_classModel(nullptr)
    , lblClass(nullptr)
    , lblFunction(nullptr)
    , cboFunction(nullptr)
    , cboFunctionModel(nullptr)
    , btnFunctionFilter(nullptr)
    , lblPID(nullptr)
    , cboPID(nullptr)
    , cboPIDModel(nullptr)
    , lblTID(nullptr)
    , cboTID(nullptr)
    , cboTIDModel(nullptr)
    , gbxFilter(nullptr)
    , cbxCaseSensitive(nullptr)
    , lblSearch(nullptr)
    , cbxInversesearch(nullptr)
    , btnFilter(nullptr)
    , btnClearFilter(nullptr)
    , lblStartTime(nullptr)
    , lblEndTime(nullptr)
    , dtEndTime(nullptr)
    , cbxWordOnly(nullptr)
    , cbxSearch(nullptr)
    , m_searchDialog(nullptr)
    , m_logEntryView(nullptr)
    , keyCtrlM(nullptr)
    , keyF3(nullptr)
    , keyCtrlF(nullptr)
    , m_trace(tracer)
    , m_bWordOnly(false)
    , m_bSearchActive(false)
{
    CFuncTracer trace("LogViewer::LogViewer", m_trace);
    CScopeTimer timer("LogViewer::LogViewer", 0, [=, &trace](const std::string& msg)
                      {
                          trace.Info("Timings : ");
                          trace.Info("   %s", msg.c_str());
                      });
    try
    {
        timer.SetTime("start");
        m_vlayout = new QVBoxLayout();
        m_hlayout = new QHBoxLayout();


        init_createActions();
        timer.SetTime("created actions");
        init_createMenus();
        timer.SetTime("create menus");
        init_createLevelFilteringGroupBox();
        timer.SetTime("create level group box");
        init_createFunctionFilteringGroupBox();
        timer.SetTime("create function group box");
        init_createFilterGroupBox();
        timer.SetTime("create filter group box");
        init_createShortCut();
        timer.SetTime("Create shortcuts");

        init_retranslateUi();
        init_styles();
        timer.SetTime("Styles");

        tabWidget = new QTabWidget(this);
        m_vlayout->addLayout(m_hlayout);
        m_vlayout->addWidget(tabWidget);

        setLayout(m_vlayout);
        timer.SetTime("Create tabwidget and add to layout");

        connect(cbxWordOnly, &QCheckBox::stateChanged, this, &LogViewer::on_cbxWordOnly_stateChanged);
        connect(cbxCaseSensitive, &QCheckBox::stateChanged, this, &LogViewer::on_cbxCaseSensitive_stateChanged);
        connect(cbxInversesearch, &QCheckBox::stateChanged, this, &LogViewer::on_cbxInversesearch_stateChanged);
        connect(tabWidget, &QTabWidget::currentChanged, this, &LogViewer::onTabChanged);
        connect(btnLevelFiltering, &QPushButton::clicked, this, &LogViewer::onLevelFilter_clicked);
        connect(btnFunctionFilter, &QPushButton::clicked, this, &LogViewer::onFunctionFilter_clicked);
        connect(btnFilter, &QPushButton::clicked,this, &LogViewer::onSearchFilter_clicked);
        connect(btnClearFilter, &QPushButton::clicked, this, &LogViewer::onClearFilter_clicked);
        timer.SetTime("connect signals");
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
LogViewer::~LogViewer()
{
    CFuncTracer trace("LogViewer::~LogViewer", m_trace);
    try
    {
        if (m_hlayout != nullptr)
        {
            delete m_hlayout;
            m_hlayout = nullptr;
        }
        if (m_vlayout != nullptr)
        {
            delete m_vlayout;
            m_vlayout = nullptr;
        }
        if (menuBar != nullptr)
        {
            delete menuBar;
            menuBar = nullptr;
        }
        if (fileMenu != nullptr)
        {
            delete fileMenu;
            fileMenu = nullptr;
        }
        if (actOpen != nullptr)
        {
            delete actOpen;
            actOpen = nullptr;
        }
        if (actClose != nullptr)
        {
            delete actClose;
            actClose = nullptr;
        }
        if (actSave != nullptr)
        {
            delete actSave;
            actSave = nullptr;
        }
        if (toolsMenu != nullptr)
        {
            delete toolsMenu;
            toolsMenu = nullptr;
        }
        if (actToggleMark != nullptr)
        {
            delete actToggleMark;
            actToggleMark= nullptr;
        }
        if (actGotoNextMark != nullptr)
        {
            delete actGotoNextMark;
            actGotoNextMark = nullptr;
        }
        if (actClearAllMarks != nullptr)
        {
            delete actClearAllMarks;
            actClearAllMarks= nullptr;
        }
        if (actSearchText != nullptr)
        {
            delete actSearchText;
            actSearchText = nullptr;
        }
        if (actFindNextText != nullptr)
        {
            delete actFindNextText;
            actFindNextText = nullptr;
        }

        if (tabWidget != nullptr)
        {
            delete tabWidget;
            tabWidget = nullptr;
        }
        if (lblLevel != nullptr)
        {
            delete lblLevel;
            lblLevel = nullptr;
        }
        if (cboLevel != nullptr)
        {
            delete cboLevel;
            cboLevel = nullptr;
        }
        if (cbo_levelModel != nullptr)
        {
            delete cbo_levelModel;
            cbo_levelModel = nullptr;
        }
        if (btnLevelFiltering != nullptr)
        {
            delete btnLevelFiltering;
            btnLevelFiltering = nullptr;
        }
        if (gbxLevel != nullptr)
        {
            delete gbxLevel;
            gbxLevel = nullptr;
        }
        if (cboClass != nullptr)
        {
            delete cboClass;
            cboClass = nullptr;
        }
        if (cbo_classModel != nullptr)
        {
            delete cbo_classModel;
            cbo_classModel = nullptr;
        }
        if (lblClass != nullptr)
        {
            delete lblClass;
            lblClass = nullptr;
        }
        if (lblFunction != nullptr)
        {
            delete lblFunction;
            lblFunction = nullptr;
        }
        if (cboFunctionModel != nullptr)
        {
            delete cboFunctionModel;
            cboFunctionModel = nullptr;
        }
        if (cboFunction != nullptr)
        {
            delete cboFunction;
            cboFunction = nullptr;
        }
        if (cboFunctionModel != nullptr)
        {
            delete cboFunctionModel;
            cboFunctionModel = nullptr;
        }
        if (lblPID != nullptr)
        {
            delete lblPID;
            lblPID = nullptr;
        }
        if (cboPIDModel != nullptr)
        {
            delete cboPIDModel;
            cboPIDModel = nullptr;
        }
        if (cboPID != nullptr)
        {
            delete cboPID;
            cboPID = nullptr;
        }
        if (lblTID != nullptr)
        {
            delete lblTID;
            lblTID = nullptr;
        }
        if (cboTIDModel != nullptr)
        {
            delete cboTIDModel;
            cboTIDModel = nullptr;
        }
        if (cboTID != nullptr)
        {
            delete cboTID;
            cboTID = nullptr;
        }
        if (btnFunctionFilter != nullptr)
        {
            delete btnFunctionFilter;
            btnFunctionFilter = nullptr;
        }
        if (gbxFunctionFiltering != nullptr)
        {
            delete gbxFunctionFiltering;
            gbxFunctionFiltering = nullptr;
        }
        if (cbxCaseSensitive != nullptr)
        {
            delete cbxCaseSensitive;
            cbxCaseSensitive = nullptr;
        }
        if (lblSearch != nullptr)
        {
            delete lblSearch;
            lblSearch = nullptr;
        }
        if (cbxInversesearch != nullptr)
        {
            delete cbxInversesearch;
            cbxInversesearch = nullptr;
        }
        if (btnFilter != nullptr)
        {
            delete btnFilter;
            btnFilter = nullptr;
        }
        if (btnClearFilter != nullptr)
        {
            delete btnClearFilter;
            btnClearFilter = nullptr;
        }
        if (lblStartTime != nullptr)
        {
            delete lblStartTime;
            lblStartTime = nullptr;
        }
        if (lblEndTime != nullptr)
        {
            delete lblEndTime;
            lblEndTime = nullptr;
        }
        if (dtEndTime != nullptr)
        {
            delete dtEndTime;
            dtEndTime = nullptr;
        }
        if (cbxWordOnly != nullptr)
        {
            delete cbxWordOnly;
            cbxWordOnly = nullptr;
        }
        if (cbxSearch != nullptr)
        {
            delete cbxSearch;
            cbxSearch = nullptr;
        }
        if (gbxFilter != nullptr)
        {
            delete gbxFilter;
            gbxFilter = nullptr;
        }
        if (m_searchDialog)
        {
            delete m_searchDialog;
            m_searchDialog = nullptr;
        }
        if (m_logEntryView)
        {
            delete m_logEntryView;
            m_logEntryView = nullptr;
        }
        if (keyCtrlM != nullptr)
        {
            delete keyCtrlM;
            keyCtrlM = nullptr;
        }
        if (keyF3 != nullptr)
        {
            delete keyF3;
            keyF3 = nullptr;
        }
        if (keyCtrlF != nullptr)
        {
            delete keyCtrlF;
            keyCtrlF = nullptr;
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
void LogViewer::init_styles(void)
{
    CFuncTracer trace("LogViewer::init_styles", m_trace);
    try
    {
        gbxFilter->setStyleSheet("QGroupBox:disabled{color:grey;}QGroupBox:enabled{color: black;font-weight:bold;}");
        gbxFunctionFiltering->setStyleSheet("QGroupBox:disabled{color:grey;}QGroupBox:enabled{color: black;font-weight:bold;}");
        gbxLevel->setStyleSheet("QGroupBox:disabled{color:grey;}QGroupBox:enabled{color: black;font-weight:bold;}");
        cbxWordOnly->setStyleSheet("QCheckBox:disabled{ color: grey; }QCheckBox:enabled{color: black;font-weight:bold;}");
        cbxCaseSensitive->setStyleSheet("QCheckBox:disabled{ color: grey; }QCheckBox:enabled{color: black;font-weight:bold;}");
        cbxInversesearch->setStyleSheet("QCheckBox:disabled{ color: grey; }QCheckBox:enabled{color: black;font-weight:bold;}");
        lblClass->setStyleSheet("QLabel:disabled{color:grey;}QLabel:enabled{color:black;font-weight:bold;}");
        lblEndTime->setStyleSheet("QLabel:disabled{color:grey;}QLabel:enabled{color:black;font-weight:bold;}");
        lblFunction->setStyleSheet("QLabel:disabled{color:grey;}QLabel:enabled{color:black;font-weight:bold;}");
        lblLevel->setStyleSheet("QLabel:disabled{color:grey;}QLabel:enabled{color:black;font-weight:bold;}");
        lblSearch->setStyleSheet("QLabel:disabled{color:grey;}QLabel:enabled{color:black;font-weight:bold;}");
        lblStartTime->setStyleSheet("QLabel:disabled{color:grey;}QLabel:enabled{color:black;font-weight:bold;}");
        lblPID->setStyleSheet("QLabel:disabled{color:grey;}QLabel:enabled{color:black;font-weight:bold;}");
        lblTID->setStyleSheet("QLabel:disabled{color:grey;}QLabel:enabled{color:black;font-weight:bold;}");
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
void LogViewer::init_createActions(void)
{
    CFuncTracer trace("LogViewer::init_createActions", m_trace);
    try
    {
        actOpen = new QAction(tr("&Open"), this);
        actOpen->setShortcut(QKeySequence::Open);
        actOpen->setStatusTip(tr("Opens a log file or analyse"));
        connect(actOpen, &QAction::triggered, this, &LogViewer::open);

        actClose = new QAction(tr("&Close"), this);
        actClose->setShortcut(QKeySequence::Close);
        actClose->setStatusTip(tr("Closes the log file"));
        connect(actClose, &QAction::triggered, this, &LogViewer::close);

        actSave = new QAction(tr("&Save"), this);
        actSave->setShortcut(QKeySequence::Save);
        actSave->setStatusTip(tr("Save the filtered log file"));
        connect(actSave, &QAction::triggered, this, &LogViewer::save);

        actToggleMark = new QAction(tr("Toggle Mark \tCtrl+M"));
        actToggleMark->setStatusTip(tr("Add/Delete mark at that line"));
        connect(actToggleMark, &QAction::triggered, this, &LogViewer::on_toggle_mark);

        actGotoNextMark = new QAction(tr("Find next Mark \tF3"));
        actGotoNextMark->setStatusTip(tr("Jump to next mark"));
        connect(actGotoNextMark, &QAction::triggered, this, &LogViewer::on_goto_next_mark);

        actClearAllMarks = new QAction(tr("Exit Mark/Search mode \tESC"));
        actClearAllMarks->setStatusTip(tr("Remove all marks and search text signs"));
        connect(actClearAllMarks, &QAction::triggered, this, &LogViewer::on_exit_mode);

        actSearchText = new QAction(tr("Mark text \tCtrl+F"));
        actSearchText->setStatusTip(tr("Find text in the description and mark it"));
        connect(actSearchText, &QAction::triggered, this, &LogViewer::on_search_text);

        actFindNextText = new QAction(tr("Find next text \tF2"));
        actFindNextText->setStatusTip(tr("Find the next text search item in the file and jumpt to it"));
        connect(actFindNextText, &QAction::triggered, this, &LogViewer::on_goto_next_required_text);


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
void LogViewer::init_createMenus(void)
{
    CFuncTracer trace("LogViewer::init_createMenus", m_trace);
    try
    {
        menuBar = new QMenuBar();
        fileMenu = new QMenu(tr("&File"));
        menuBar->addMenu(fileMenu);
        fileMenu->addAction(actOpen);
        fileMenu->addAction(actClose);
        fileMenu->addAction(actSave);

        toolsMenu = new QMenu(tr("&Tools"));
        menuBar->addMenu(toolsMenu);
        toolsMenu->addAction(actToggleMark);
        toolsMenu->addAction(actGotoNextMark);
        toolsMenu->addAction(actSearchText);
        toolsMenu->addAction(actClearAllMarks);

        m_vlayout->setMenuBar(menuBar);
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
void LogViewer::init_createFilterGroupBox(void)
{
    CFuncTracer trace("LogViewer::init_createFilterGroupBox", m_trace);
    try
    {
        gbxFilter = new QGroupBox(this);
        gbxFilter->setObjectName("gbxFilter");
        gbxFilter->setEnabled(true);
        gbxFilter->setGeometry(QRect(970, 10, 901, 141));
        gbxFilter->setMinimumSize(901,141);
        cbxCaseSensitive = new QCheckBox(gbxFilter);
        cbxCaseSensitive->setObjectName("cbxCaseSensitive");
        cbxCaseSensitive->setGeometry(QRect(150, 40, 131, 23));
        lblSearch = new QLabel(gbxFilter);
        lblSearch->setObjectName("lblSearch");
        lblSearch->setGeometry(QRect(10, 80, 51, 25));
        cbxInversesearch = new QCheckBox(gbxFilter);
        cbxInversesearch->setObjectName("cbxInversesearch");
        cbxInversesearch->setGeometry(QRect(280, 40, 121, 23));
        btnFilter = new QPushButton(gbxFilter);
        btnFilter->setObjectName("btnFilter");
        btnFilter->setGeometry(QRect(750, 110, 130, 25));
        btnClearFilter = new QPushButton(gbxFilter);
        btnClearFilter->setObjectName("btnClearFilter");
        btnClearFilter->setGeometry(QRect(670, 110, 75, 25));
        lblStartTime = new QLabel(gbxFilter);
        lblStartTime->setObjectName("lblStartTime");
        lblStartTime->setGeometry(QRect(440, 40, 81, 26));
        dtStartTime = new QTimeEdit(gbxFilter);
        dtStartTime->setObjectName("dtStartTime");
        dtStartTime->setGeometry(QRect(530, 40, 118, 26));
        lblEndTime = new QLabel(gbxFilter);
        lblEndTime->setObjectName("lblEndTime");
        lblEndTime->setGeometry(QRect(680, 40, 71, 26));
        dtEndTime = new QTimeEdit(gbxFilter);
        dtEndTime->setObjectName("dtEndTime");
        dtEndTime->setGeometry(QRect(760, 40, 118, 26));
        cbxWordOnly = new QCheckBox(gbxFilter);
        cbxWordOnly->setObjectName("cbxWordOnly");
        cbxWordOnly->setEnabled(true);
        cbxWordOnly->setGeometry(QRect(10, 40, 131, 23));
        cbxWordOnly->setAutoFillBackground(true);
        cbxSearch = new QComboBox(gbxFilter);
        cbxSearch->setObjectName("cbxSearch");
        cbxSearch->setGeometry(QRect(80, 80, 771, 25));
        cbxSearch->setEditable(true);
        m_hlayout->addWidget(gbxFilter);
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
void LogViewer::init_createFunctionFilteringGroupBox(void)
{
    CFuncTracer trace("LogViewer::init_createFunctionFilteringGroupBox", m_trace);
    try
    {
        gbxFunctionFiltering = new QGroupBox(this);
        gbxFunctionFiltering->setObjectName("gbxFunctionFiltering");
        gbxFunctionFiltering->setEnabled(true);
        gbxFunctionFiltering->setGeometry(QRect(320, 10, 641, 141));
        gbxFunctionFiltering->setMinimumSize(641,141);
        cboClass = new QComboBox(gbxFunctionFiltering);
        cboClass->setObjectName("cboClass");
        cboClass->setGeometry(QRect(110, 40, 501, 25));
        cbo_classModel = new QCheckableModel(m_trace, "cbo_classModel");
        cboClass->setModel(cbo_classModel);
        connect(cbo_classModel, &QCheckableModel::dataChanged, this, &LogViewer::on_cboClass_checkbox_changed);

        lblClass = new QLabel(gbxFunctionFiltering);
        lblClass->setObjectName("lblClass");
        lblClass->setGeometry(QRect(20, 40, 67, 25));
        lblFunction = new QLabel(gbxFunctionFiltering);
        lblFunction->setObjectName("lblFunction");
        lblFunction->setGeometry(QRect(20, 70, 100, 25));
        cboFunction = new QComboBox(gbxFunctionFiltering);
        cboFunction->setObjectName("cboFunction");
        cboFunction->setGeometry(QRect(110, 70, 501, 25));
        cboFunctionModel = new QCheckableModel(m_trace, "cboFunctionModel");
        cboFunction->setModel(cboFunctionModel);
        connect(cboFunctionModel, &QCheckableModel::dataChanged, this, &LogViewer::on_cboFunction_checkbox_changed);

        lblPID = new QLabel(gbxFunctionFiltering);
        lblPID->setObjectName("lblPID");
        lblPID->setGeometry(QRect(20, 100, 50, 25));
        cboPID = new QComboBox(gbxFunctionFiltering);
        cboPID->setObjectName("cboPID");
        cboPID->setGeometry(QRect(110, 100, 100, 25));
        cboPIDModel = new QCheckableModel(m_trace, "cboPIDModel");
        cboPID->setModel(cboPIDModel);
        connect(cboPIDModel, &QCheckableModel::dataChanged, this, &LogViewer::on_cboPID_checkbox_changed);

        lblTID = new QLabel(gbxFunctionFiltering);
        lblTID->setObjectName("lblTID");
        lblTID->setGeometry(QRect(215, 100, 10, 25));
        cboTID= new QComboBox(gbxFunctionFiltering);
        cboTID->setObjectName("cboTID");
        cboTID->setGeometry(QRect(230, 100, 100, 25));
        cboTIDModel = new QCheckableModel(m_trace, "cboTIDModel");
        cboTID->setModel(cboTIDModel);
        connect(cboTIDModel, &QCheckableModel::dataChanged, this, &LogViewer::on_cboTID_checkbox_changed);



        btnFunctionFilter = new QPushButton(gbxFunctionFiltering);
        btnFunctionFilter->setObjectName("btnFunctionFilter");
        btnFunctionFilter->setGeometry(QRect(470, 100, 141, 25));
        btnClearFunctionFilter = new QPushButton(gbxFunctionFiltering);
        btnClearFunctionFilter->setObjectName("btnClearFunctionFilter");
        btnClearFunctionFilter->setGeometry(QRect(380,100, 75,25));
        m_hlayout->addWidget(gbxFunctionFiltering);
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
void LogViewer::init_createLevelFilteringGroupBox(void)
{
    CFuncTracer trace("LogViewer::init_createLevelFilteringGroupBox", m_trace);
    try
    {
        gbxLevel = new QGroupBox(this);
        gbxLevel->setObjectName("gbxLevel");
        gbxLevel->setEnabled(true);
        gbxLevel->setGeometry(QRect(20, 10, 291, 141));
        gbxLevel->setMinimumSize(291,141);
        QFont font1;
        font1.setBold(false);
        gbxLevel->setFont(font1);
        gbxLevel->setInputMethodHints(Qt::ImhNone);
        lblLevel = new QLabel(gbxLevel);
        lblLevel->setObjectName("lblLevel");
        lblLevel->setGeometry(QRect(10, 70, 67, 25));
        cboLevel = new QComboBox(gbxLevel);
        cboLevel->setObjectName("cboLevel");
        cboLevel->setGeometry(QRect(70, 70, 201, 25));
        cbo_levelModel = new QCheckableModel(m_trace, "cbo_levelModel");
        cboLevel->setModel(cbo_levelModel);
        connect(cbo_levelModel, &QCheckableModel::dataChanged,
                this, &LogViewer::on_cboLevel_checkbox_changed);

        btnLevelFiltering = new QPushButton(gbxLevel);
        btnLevelFiltering->setObjectName("btnLevelFiltering");
        btnLevelFiltering->setGeometry(QRect(130, 100, 141, 25));
        btnClearLevelFiltering = new QPushButton(gbxLevel);
        btnClearLevelFiltering->setObjectName("btnClearFilter");
        btnClearLevelFiltering->setGeometry(QRect(50,100,75,25));
        m_hlayout->addWidget(gbxLevel);
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
void LogViewer::init_createShortCut(void)
{
    CFuncTracer trace("LogViewer::init_createShortCut", m_trace);
    try
    {
        keyCtrlM = new QShortcut(this);
        keyCtrlM->setKey(Qt::CTRL|Qt::Key_M);

        keyF2 = new QShortcut(this);
        keyF2->setKey(Qt::Key_F2);

        keyF3 = new QShortcut(this);
        keyF3->setKey(Qt::Key_F3);

        keyCtrlF = new QShortcut(this);
        keyCtrlF->setKey(Qt::CTRL|Qt::Key_F);

        keyEsc = new QShortcut(this);
        keyEsc->setKey(Qt::Key_Escape);


        connect(keyCtrlM, &QShortcut::activated, this, &LogViewer::on_toggle_mark);
        connect(keyEsc, &QShortcut::activated, this, &LogViewer::on_exit_mode);
        connect(keyF2, &QShortcut::activated, this, &LogViewer::on_goto_next_required_text);
        connect(keyF3, &QShortcut::activated, this, &LogViewer::on_goto_next_mark);
        connect(keyCtrlF, &QShortcut::activated, this, &LogViewer::on_search_text);
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
void LogViewer::init_retranslateUi(void)
{
    CFuncTracer trace("LogViewer::init_retranslateUi", m_trace);
    try
    {
        setWindowTitle(QCoreApplication::translate("LogViewer", "LogViewer", nullptr));
        gbxFilter->setTitle(QCoreApplication::translate("LogViewer", "Text Filtering", nullptr));
        cbxCaseSensitive->setText(QCoreApplication::translate("LogViewer", "Case Sensitive", nullptr));
        lblSearch->setText(QCoreApplication::translate("LogViewer", "Search", nullptr));
        cbxInversesearch->setText(QCoreApplication::translate("LogViewer", "Inverse search", nullptr));
        btnFilter->setText(QCoreApplication::translate("LogViewer", "Text Filter", nullptr));
        btnClearFilter->setText(QCoreApplication::translate("LogViewer", "Clear", nullptr));
        lblStartTime->setText(QCoreApplication::translate("LogViewer", "Start time", nullptr));
        lblEndTime->setText(QCoreApplication::translate("LogViewer", "End time", nullptr));
        dtEndTime->setDisplayFormat(QCoreApplication::translate("LogViewer", "HH:mm:ss", nullptr));
        dtStartTime->setDisplayFormat(QCoreApplication::translate("LogViewer", "HH:mm:ss", nullptr));
        cbxWordOnly->setText(QCoreApplication::translate("LogViewer", "Word Only", nullptr));

        gbxFunctionFiltering->setTitle(QCoreApplication::translate("LogViewer", "Function Filtering", nullptr));
        lblClass->setText(QCoreApplication::translate("LogViewer", "Class", nullptr));
        lblFunction->setText(QCoreApplication::translate("LogViewer", "Text Function", nullptr));
        btnFunctionFilter->setText(QCoreApplication::translate("LogViewer", "Function Filter", nullptr));
        btnClearFunctionFilter->setText(QCoreApplication::translate("LogViewer", "Clear", nullptr));
        gbxLevel->setTitle(QCoreApplication::translate("LogViewer", "Level Filtering", nullptr));
        lblLevel->setText(QCoreApplication::translate("LogViewer", "Level", nullptr));
        lblPID->setText(QCoreApplication::translate("LogViewer", "PID : TID", nullptr));
        lblTID->setText(QCoreApplication::translate("LogViewer", ":", nullptr));
        btnLevelFiltering->setText(QCoreApplication::translate("LogViewer", "Level Filter", nullptr));
        btnClearLevelFiltering->setText(QCoreApplication::translate("LogViewer", "Clear", nullptr));

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

void LogViewer::update_classFilter(void)
{
    CFuncTracer trace("LogViewer::update_classFilter", m_trace);
    CScopeTimer timer("LogViewer::update_classFilter", 0, [=, &trace](const std::string& msg){
        trace.Info("Timings:");
        trace.Info("    %s", msg.c_str());
    });
    try
    {
        timer.SetTime("Start");
        if (m_currentLogFile.get() != nullptr)
        {
            timer.SetTime("Get the current log file");
            std::map<std::string, bool> classes = m_currentLogFile->GetClasses();
            timer.SetTime("Classes get");
            // clear combo box
            cboClass->clear();
            timer.SetTime("clear the classes");
            // Add to combo box
            cbo_classModel->append(classes);
            timer.SetTime("classes append to the model");
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
void LogViewer::update_functionFilter(void)
{
    CFuncTracer trace("LogViewer::update_functionFilter", m_trace);
    try
    {
        if (m_currentLogFile.get() != nullptr)
        {
            std::map<std::string, bool> functions = m_currentLogFile->GetFunctions();

            // Clear combo box
            cboFunction->clear();
            // Add to combo box
            cboFunctionModel->append(functions);
        }
        else
            trace.Error("m_currentLogFile = nullptr");

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
void LogViewer::update_traceLevels(void)
{
    CFuncTracer trace("LogViewer::update_traceLevels", m_trace);
    try
    {
        if (m_currentLogFile.get() != nullptr)
        {
            std::map<std::string, bool> tracelevels = m_currentLogFile->GetTracelevels();

            //clear combo box
            cboLevel->clear();
            //Add to combo box
            cbo_levelModel->append(tracelevels);
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
void LogViewer::update_tidFilter(void)
{
    CFuncTracer trace("LogViewer::update_tidFilter", m_trace);
    try
    {
        if (m_currentLogFile.get() != nullptr)
        {
            std::map<std::string, bool> tids = m_currentLogFile->GetTIDs();

            //clear combo box
            cboTID->clear();
            //Add to combo box
            cboTIDModel->append(tids);
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
void LogViewer::update_pidFilter(void)
{
    CFuncTracer trace("LogViewer::update_pidFilter", m_trace);
    try
    {
        if (m_currentLogFile.get() != nullptr)
        {
            std::map<std::string, bool> pids = m_currentLogFile->GetPIDs();

            //clear combo box
            cboPID->clear();
            //Add to combo box
            cboPIDModel->append(pids);
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
void LogViewer::update_current_tab(void)
{
    CFuncTracer trace("LogViewer::update_current_tab", m_trace);
    CScopeTimer timer("LogViewer::update_current_tab", 0, [=, &trace](const std::string& msg){
        trace.Info("Timings:");
        trace.Info("    %s", msg.c_str());
    });
    try
    {
        timer.SetTime("Start");
        QLogFileWidget *current_tab = dynamic_cast<QLogFileWidget*>(tabWidget->widget(m_currentTabIdx));
        timer.SetTime("Get current tab");
        std::vector<CLogEntry> lines = m_currentLogFile->GetEntries();
        timer.SetTime("Get the lines");
        trace.Trace("Lines : %ld", lines.size());
        current_tab->GetModel()->clear();
        timer.SetTime("clear the model");
        current_tab->GetModel()->append(lines);
        timer.SetTime("Append lines to model");
        current_tab->GotoTopOfTable();

        if (lines.size() >= 1)
        {
            dtStartTime->setTime(QTime(lines[0].GetTimeHours(),
                                       lines[0].GetTimeMinutes(),
                                       lines[0].GetTimeSeconds(),
                                       lines[0].GetTimeMilliseconds()));
            dtEndTime->setTime(QTime(lines[0].GetTimeHours(),
                                     lines[0].GetTimeMinutes(),
                                     lines[0].GetTimeSeconds(),
                                     lines[0].GetTimeMilliseconds()));
            if (lines.size() >= 2)
            {
                dtEndTime->setTime(QTime(lines[(lines.size() - 1)].GetTimeHours(),
                                         lines[(lines.size() - 1)].GetTimeMinutes(),
                                         lines[(lines.size() - 1)].GetTimeSeconds(),
                                         lines[(lines.size() - 1)].GetTimeMilliseconds()));
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
void LogViewer::update_config_settings(std::shared_ptr<CConfigSettings>& configSettings)
{
    CFuncTracer trace("LogViewer::update_config_settings", m_trace);
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

void LogViewer::on_cbxWordOnly_stateChanged(int /*arg1*/)
{
    CFuncTracer trace("LogViewer::on_cbxWordOnly_stateChanged", m_trace, false);
    try
    {
        trace.Trace("state : %s",
                    (cbxWordOnly->checkState() == Qt::Checked) ? "CHECKED" : "NOT CHECKED");
        if (cbxWordOnly->checkState() == Qt::Checked)
        {
            m_bWordOnly = true;
        } else
        {
            m_bWordOnly = false;
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
void LogViewer::on_cbxCaseSensitive_stateChanged(int /*arg1*/)
{
    CFuncTracer trace("LogViewer::on_cbxCaseSensitive_stateChanged", m_trace, false);
    try
    {
        trace.Trace("state : %s", (cbxCaseSensitive->checkState() == Qt::Checked)? "CHECKED" : "NOT CHECKED");

        if (cbxCaseSensitive->checkState() == Qt::Checked)
            m_bCaseSensitive = true;
        else
            m_bCaseSensitive = false;
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
void LogViewer::on_cbxInversesearch_stateChanged(int /*arg1*/)
{
    CFuncTracer trace("LogViewer::on_cbxInversesearch_stateChanged", m_trace, false);
    try
    {
        trace.Trace("state : %s", (cbxInversesearch->checkState() == Qt::Checked)? "CHECKED" : "NOT CHECKED");
        if (cbxInversesearch->checkState() == Qt::Checked)
        {
            cbxSearch->setStyleSheet("QComboBox:enabled{color:red;font-weight:bold;}");
            m_bInverseSearch = true;
        }
        else
        {
            cbxSearch->setStyleSheet("QComboBox:enabled{color:black;font-weight:bold;}");
            m_bInverseSearch = false;
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
void LogViewer::onFunctionFilter_clicked(void)
{
    CFuncTracer trace("LogViewer::onFunctionFilter_clicked", m_trace);
    try
    {
        m_currentLogFile->ClearFilter();
        m_currentLogFile->SetClassNameFilter();
        m_currentLogFile->SetFunctionFilter();

        update_current_tab();

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
void LogViewer::onLevelFilter_clicked(void)
{
    CFuncTracer trace("LogViewer::onLevelFilter_clicked", m_trace);
    try
    {
        m_currentLogFile->ClearFilter();
        m_currentLogFile->SetLevelFilter();

        // Update GUI
        update_current_tab();
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
void LogViewer::onSearchFilter_clicked(void)
{
    CFuncTracer trace("LogViewer::onSearchFilter_clicked", m_trace);
    try
    {
        std::vector<std::string> textfilters;
        std::string combotext = cbxSearch->currentText().toStdString();
        textfilters = CConversion::split(combotext, ',', textfilters, true);

        if (!m_bInverseSearch)
            m_currentLogFile->SetDescriptionFilter(textfilters, m_bCaseSensitive, m_bWordOnly);
        else
            m_currentLogFile->SetInverseDescriptionFilter(textfilters, m_bCaseSensitive, m_bWordOnly);


        cbxSearch->addItem(cbxSearch->currentText());

        // Update the GUI
        update_current_tab();
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
void LogViewer::onProcThreadIdFilter_clicked(void)
{
    CFuncTracer trace("LogViewer::onProcThreadIdFilter_clicked", m_trace);
    try
    {

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
void LogViewer::onClearFilter_clicked(void)
{
    CFuncTracer trace("LogViewer::onClearFilter_clicked", m_trace);
    try
    {
        m_currentLogFile->ClearFilter();

        onFunctionFilter_clicked();
        onLevelFilter_clicked();
        // clear cbo text and add filter item in the combo box
        // update the GUI
        update_current_tab();
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
void LogViewer::onClearLevelFilter_clicked(void)
{
    CFuncTracer trace("LogViewer::onClearLevelFilter_clicked", m_trace);
    try
    {
        m_currentLogFile->ClearFilter();
        onFunctionFilter_clicked();
        onSearchFilter_clicked();

        // update the GUI
        update_current_tab();
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
void LogViewer::onClearFunctionFilter_clicked(void)
{
    CFuncTracer trace("LogViewer::onClearFunctionFilter_clicked", m_trace);
    try
    {
        m_currentLogFile->ClearFilter();
        onSearchFilter_clicked();
        onLevelFilter_clicked();

        // update the GUI
        update_current_tab();
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

void LogViewer::onTabChanged(int index)
{
    CFuncTracer trace("LogViewer::onTabChanged", m_trace);
    CScopeTimer timer("LogViewer::onTabChanged", 0, [=, &trace](const std::string& msg){
        trace.Info("Timings:");
        trace.Info("    %s", msg.c_str());
    });
    try
    {
        trace.Info("index : %ld", index);
        m_currentTabIdx = index;
        timer.SetTime("00");
        QLogFileWidget *current_tab = dynamic_cast<QLogFileWidget*>(tabWidget->widget(index));
        if (current_tab != nullptr)
        {
            auto it = m_mpLogFiles.find(current_tab->GetFilename());
            if (it != m_mpLogFiles.end())
            {
                timer.SetTime("tab is found");
                m_currentLogFile = it->second;
                update_classFilter();
                timer.SetTime("classFilter is updated!");
                update_functionFilter();
                timer.SetTime("functionFilter is updated!");
                update_traceLevels();
                timer.SetTime("traceLevels are updated!");
                update_pidFilter();
                timer.SetTime("pids are updated");
                update_tidFilter();
                timer.SetTime("tids are updated");
                update_current_tab();
                timer.SetTime("Current tab is updated");
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
void LogViewer::on_toggle_mark(void)
{
    CFuncTracer trace("LogViewer::on_toggle_mark", m_trace);
    try
    {
        QLogFileWidget *current_tab = dynamic_cast<QLogFileWidget*>(tabWidget->widget(m_currentTabIdx));
        if (current_tab != nullptr)
        {
            bool bMark = false;
            long long id = current_tab->ToggleMark(current_tab->GetCurrentIndex(), bMark);
            m_currentLogFile->SetMark(id, bMark);
        }
        else
            trace.Warning("Current tab is not found");
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
void LogViewer::on_goto_next_mark(void)
{
    CFuncTracer trace("LogViewer::on_goto_next_mark", m_trace);
    try
    {
        QLogFileWidget *current_tab = dynamic_cast<QLogFileWidget*>(tabWidget->widget(m_currentTabIdx));
        if (current_tab != nullptr)
            current_tab->GotoToNextMark();
        else
            trace.Warning("Current tab is not found");
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
void LogViewer::on_search_text(void)
{
    CFuncTracer trace("LogViewer::on_search_text", m_trace);
    try
    {
        if (m_searchDialog == nullptr)
        {
            m_searchDialog = new SearchForm([=, &trace](const std::string& text, bool err)
                                                    {
                                                        trace.Info("search text : %s", text.c_str());

                                                          if (  (text.empty() == false)
                                                              &&(err == false))
                                                          {
                                                              trace.Info("text : %s", text.c_str());
                                                              QLogFileWidget *current_tab = dynamic_cast<QLogFileWidget*>(tabWidget->widget(m_currentTabIdx));
                                                              if (current_tab != nullptr)
                                                              {
                                                                  std::list<long long> ids = current_tab->GetModel()->IndicateSearchText(text);
                                                                  if (ids.size() > 0)
                                                                  {
                                                                      std::for_each(ids.begin(), ids.end(), [=, &text](long long& id){
                                                                          m_currentLogFile->SetRequiredText(id, text, true);
                                                                      });
                                                                      m_bSearchActive = true;
                                                                  }
                                                              }
                                                              else
                                                                  trace.Error("Current tab not found");
                                                          }
                                                          else
                                                          {
                                                              trace.Error("Failed to search text (%s, Error = %s)", text.c_str(), (err == true)?"TRUE":"FALSE");
                                                              m_bSearchActive = false;
                                                          }
                                                          m_searchDialog->close();

                                                    }, m_trace);
        }
        m_searchDialog->show();
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
void LogViewer::on_exit_mode(void)
{
    CFuncTracer trace("LogViewer::on_exit_mode", m_trace);
    try
    {
        if (m_bSearchActive == true)
        {
            m_bSearchActive = false;
            m_currentLogFile->SetRequiredText(-1, "", false);
            update_current_tab();
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
void LogViewer::on_goto_next_required_text(void)
{
    CFuncTracer trace("LogViewer::on_goto_next_required_text", m_trace);
    try
    {
        QLogFileWidget *current_tab = dynamic_cast<QLogFileWidget*>(tabWidget->widget(m_currentTabIdx));
        if (current_tab != nullptr)
            current_tab->GotoNextRequiredText();
        else
            trace.Warning("Current tab is not found");
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
void LogViewer::on_cboClass_checkbox_changed(const QModelIndex& topLeft, const QModelIndex& , QList<int> )
{
    CFuncTracer trace("LogViewer::on_cboClass_checkbox_changed", m_trace);
    CScopeTimer timer("LogViewer::on_cboClass_checkbox_changed", 0, [=, &trace](const std::string& msg){
        trace.Info("Timings:");
        trace.Info("    %s", msg.c_str());
    });
    try
    {
        timer.SetTime("Start");
        if (cboClass != nullptr)
        {
            std::string sName = cboClass->itemText(topLeft.row()).toStdString();
            bool bChecked = (cboClass->itemData(topLeft.row(), Qt::CheckStateRole) == Qt::Checked);
            timer.SetTime("GetChecked state");
            if (bChecked == false)
            {
                // Set all the functions that contains this classname to false
                m_currentLogFile->UpdateClassFunctions(true, sName);
            }
            else
            {
                // Set all the functions that contains this classname to true
                m_currentLogFile->UpdateClassFunctions(false, sName);
            }
            timer.SetTime("Update class Function");

            // Update of the function filter is necessary to update the different states of the checkboxes
            //    inside the combo box of the function.
            update_functionFilter();
            timer.SetTime("Update function filter");
        }
        else
            trace.Warning("cboClass is nullptr!");
        timer.SetTime("Finish");
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
void LogViewer::on_cboFunction_checkbox_changed(const QModelIndex& topLeft, const QModelIndex&, QList<int> )
{
    CFuncTracer trace("LogViewer::on_cboFunction_checkbox_changed", m_trace);
    try
    {
        if (cboFunction != nullptr)
        {
            std::string sFunctionName = cboFunction->itemText(topLeft.row()).toStdString();
            bool bChecked = (cboFunction->itemData(topLeft.row(), Qt::CheckStateRole)
                             == Qt::Checked);
            if (bChecked == false)
                m_currentLogFile->UpdateFunctionName(true, sFunctionName);
            else
                m_currentLogFile->UpdateFunctionName(false, sFunctionName);
        }
        else
            trace.Warning("cboFunction is nullptr!");

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
void LogViewer::on_cboLevel_checkbox_changed(const QModelIndex& topLeft, const QModelIndex&, QList<int> )
{
    CFuncTracer trace("LogViewer::on_cboLevel_checkbox_changed", m_trace);
    try
    {
        if (cboLevel != nullptr)
        {
            std::string sLevel = cboLevel->itemText(topLeft.row()).toStdString();
            bool bChecked = (cboLevel->itemData(topLeft.row(), Qt::CheckStateRole) == Qt::Checked);
            if (bChecked == false)
                m_currentLogFile->UpdateLevel(true, sLevel);
            else
                m_currentLogFile->UpdateLevel(false, sLevel);
        }
        else
            trace.Warning("cboLevel is nullptr");

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
void LogViewer::on_cboPID_checkbox_changed(const QModelIndex& topLeft, const QModelIndex&, QList<int> )
{
    CFuncTracer trace("LogViewer::on_cboPID_checkbox_changed", m_trace);
    try
    {
        if (cboPID != nullptr)
        {
            std::string sPid = cboPID->itemText(topLeft.row()).toStdString();
            bool bChecked = (cboPID->itemData(topLeft.row(), Qt::CheckStateRole) == Qt::Checked);
            if (bChecked == false)
                m_currentLogFile->UpdatePID(true, sPid);
            else
                m_currentLogFile->UpdatePID(false, sPid);
        }
        else
            trace.Warning("cboPID is nullptr");
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
void LogViewer::on_cboTID_checkbox_changed(const QModelIndex& topLeft, const QModelIndex&, QList<int> )
{
    CFuncTracer trace("LogViewer::on_cboTID_checkbox_changed", m_trace);
    try
    {
        if (cboTID != nullptr)
        {
            std::string sTid = cboTID->itemText(topLeft.row()).toStdString();
            bool bChecked = (cboTID->itemData(topLeft.row(), Qt::CheckStateRole) == Qt::Checked);
            if (bChecked == false)
                m_currentLogFile->UpdateTID(true, sTid);
            else
                m_currentLogFile->UpdateTID(false, sTid);
        }
        else
            trace.Warning("cboTID is nullptr");
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

void LogViewer::on_double_click(const CLogEntry& entry)
{
    CFuncTracer trace("LogViewer::on_double_click", m_trace);
    try
    {
        if (m_logEntryView)
        {
            delete m_logEntryView;
            m_logEntryView = nullptr;
        }
        m_logEntryView = new LogEntryView(entry,
                                          m_trace,
                                          this);
        m_logEntryView->show();
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

void LogViewer::open()
{
    CFuncTracer trace("LogViewer::open", m_trace);
    CScopeTimer timer("LogViewer::open", 0, [=, &trace](const std::string& msg)
                      {
                          trace.Info("Timings : ");
                          trace.Info("   %s", msg.c_str());
                      });
    try
    {
        QString filename = QFileDialog::getOpenFileName(this, "Open log file", QDir::currentPath(),"All files (*.*) ;; Log files (*.log)");
        if (!filename.isNull())
        {
            timer.SetTime("Start filename get from dialog");
            trace.Trace("Filename : %s" , filename.toStdString().c_str());
            std::shared_ptr<CLogFile> file = std::make_shared<CLogFile>(filename.toStdString().c_str(), m_trace);
            timer.SetTime("constructed CLogfile");
            std::vector<CLogEntry> lines = file->GetEntries();
            trace.Trace("Lines : %ld", lines.size());
            timer.SetTime("Get lines of the file");
            QWidget *viewer = new QLogFileWidget(m_trace, lines, this);
            timer.SetTime("create widget with lines");
            if (m_mpLogFiles.find(file->Name()) == m_mpLogFiles.end())
            {
                m_mpLogFiles.insert(std::make_pair(file->Name(), file));
                timer.SetTime("Add file to mpLogFile");
                if (m_mpLogFiles.size() == 1)
                {
                    m_currentLogFile = file;
                    m_currentTabIdx = 0;
                }
                timer.SetTime("Update current tab");
            }
            QLogFileWidget* logFileWidget = dynamic_cast<QLogFileWidget*>(viewer);
            logFileWidget->SetFileName(filename.toStdString());
            timer.SetTime("Set filename into widget");
            tabWidget->addTab(viewer, filename);
            timer.SetTime("Add tab to widget");
            logFileWidget->SetTabIndex(tabWidget->indexOf(viewer));
            timer.SetTime("Select correct tab");
            connect(logFileWidget, SIGNAL(RowDoubleClicked(const CLogEntry& entry)), this, SLOT(LogViewer::on_double_click));
            timer.SetTime("Add double row click connection");
            // Add drop down registrations
            logFileWidget->RegisterDropDown_StartTime([=, &trace](const std::string& starttime){
                trace.Trace("Drop down start time selected (time: %s)", starttime.c_str());
                std::string hours = starttime.substr(0, starttime.find_first_of(":"));
                std::string minutes = starttime.substr(starttime.find_first_of(":") + 1, starttime.find_last_of(":"));
                std::string seconds = starttime.substr(starttime.find_last_of(":") + 1, starttime.find_first_of("."));
                std::string milliseconds = starttime.substr(starttime.find_last_of(".") + 1);

                dtStartTime->setTime(QTime(std::atoi(hours.c_str()),
                                           std::atoi(minutes.c_str()),
                                           std::atoi(seconds.c_str()),
                                           std::atoi(milliseconds.c_str())));
                std::string endtime = dtEndTime->text().toStdString();
                m_currentLogFile->SetTimeFilter(starttime, endtime);

                // Update GUI
                update_current_tab();
            });
            logFileWidget->RegisterDropDown_StopTime([=, &trace](const std::string& stoptime){
                trace.Trace("Drop down stop time selected (time: %s)", stoptime.c_str());

                std::string hours = stoptime.substr(0, stoptime.find_first_of(":"));
                std::string minutes = stoptime.substr(stoptime.find_first_of(":") + 1, stoptime.find_last_of(":"));
                std::string seconds = stoptime.substr(stoptime.find_last_of(":") + 1, stoptime.find_first_of("."));
                std::string milliseconds = stoptime.substr(stoptime.find_last_of(".") + 1);

                dtEndTime->setTime(QTime(std::atoi(hours.c_str()),
                                           std::atoi(minutes.c_str()),
                                           std::atoi(seconds.c_str()),
                                         std::atoi(milliseconds.c_str())));
                std::string starttime = dtStartTime->text().toStdString();
                m_currentLogFile->SetTimeFilter(starttime, stoptime);

                // Update GUI
                update_current_tab();
            });
            logFileWidget->RegisterDropDown_ProcID([=, &trace](const std::string& procID){
                trace.Trace("Drop down pocess id selected (process id : %s)", procID.c_str());
                std::map<std::string, bool> pids = m_currentLogFile->GetPIDs();
                if (pids.find(procID) != pids.end())
                {
                    pids[procID] = false;
                    m_currentLogFile->UpdatePID(true, procID);
                    // clear combo box
                    cboPID->clear();
                    // Add to combo box
                    cboPIDModel->append(pids);

                    // Perform the filtering
                    m_currentLogFile->ClearFilter();
                    m_currentLogFile->SetProcIdFilter();

                    // Update GUI
                    update_current_tab();
                }
            });
            logFileWidget->RegisterDropDown_ThreadID([=, &trace](const std::string& threadID){
                trace.Trace("Drop down thread id selectd (thread id : %s)", threadID.c_str());
                std::map<std::string, bool> tids = m_currentLogFile->GetTIDs();
                if (tids.find(threadID) != tids.end())
                {
                    tids[threadID] = false;
                    m_currentLogFile->UpdateTID(true, threadID);
                    // clear combo box
                    cboTID->clear();
                    // Add to combo box
                    cboTIDModel->append(tids);

                    // Perform the filtering
                    m_currentLogFile->ClearFilter();
                    m_currentLogFile->SetThreadIdFilter();

                    // Update GUI
                    update_current_tab();
                }
            });
            logFileWidget->RegisterDropDown_MarkToggle([=, &trace](QModelIndex index){
                QLogFileWidget *current_tab = dynamic_cast<QLogFileWidget*>(tabWidget->widget(m_currentTabIdx));
                if (current_tab != nullptr)
                {
                    bool bMark = false;
                    long long id = current_tab->ToggleMark(index, bMark);
                    m_currentLogFile->SetMark(id, bMark);
                }
                else
                    trace.Warning("Current tab is not found");
            });
            logFileWidget->RegisterDropDown_DisableClass([=, &trace](const std::string& cls){
                trace.Trace("Drop down disable class pushed (class : %s)", cls.c_str());
                std::map<std::string, bool> classes = m_currentLogFile->GetClasses();
                if (classes.find(cls) != classes.end())
                {
                    classes[cls] = false;
                    m_currentLogFile->UpdateClassFunctions(true, cls);
                    // clear combo box
                    cboClass->clear();
                    // Add to combo box
                    cbo_classModel->append(classes);

                    // update the function checkboxes
                    std::map<std::string, bool> functions = m_currentLogFile->GetFunctions();
                    // Clear combo box
                    cboFunction->clear();
                    // Add to combo box
                    cboFunctionModel->append(functions);

                    // Perform the filtering
                    m_currentLogFile->ClearFilter();
                    m_currentLogFile->SetClassNameFilter();
                    m_currentLogFile->SetFunctionFilter();

                    // Update GUI
                    update_current_tab();
                }
            });
            logFileWidget->RegisterDropDown_DisableFunc([=, &trace](const std::string& func){
                trace.Trace("Drop down disable func pushed (func : %s)", func.c_str());
                std::map<std::string, bool> functions = m_currentLogFile->GetFunctions();
                if (functions.find(func) != functions.end())
                {
                    functions[func] = false;
                    m_currentLogFile->UpdateFunctionName(true, func);
                    // Clear combo box
                    cboFunction->clear();
                    // Add to combo box
                    cboFunctionModel->append(functions);

                    // Perform the filtering
                    m_currentLogFile->ClearFilter();
                    m_currentLogFile->SetClassNameFilter();
                    m_currentLogFile->SetFunctionFilter();

                    // Update GUI
                    update_current_tab();
                }
                else
                    trace.Error("Function is not found in the list");

            });
            logFileWidget->RegisterDropDown_DisableTraceLevel([=, &trace](const std::string& lvl){
                trace.Trace("Drop down disable tracelevel pushed (level : %s)", lvl.c_str());
                std::map<std::string, bool> tracelevels = m_currentLogFile->GetTracelevels();
                if (tracelevels.find(lvl) != tracelevels.end())
                {
                    if (tracelevels[lvl] == true)
                    {
                        // update GUI
                        tracelevels[lvl] = false;
                        m_currentLogFile->UpdateLevel(true, lvl);
                        //clear combo box
                        cboLevel->clear();
                        //Add to combo box
                        cbo_levelModel->append(tracelevels);

                        // Perform filtering
                        m_currentLogFile->ClearFilter();
                        m_currentLogFile->SetLevelFilter();

                        // Update GUI
                        update_current_tab();
                    }
                    else
                        trace.Error("tracelevel is already disabled");
                }
                else
                    trace.Error("Tracelevel %s is not found!", lvl.c_str());
            });
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
void LogViewer::save()
{
    CFuncTracer trace("LogViewer::save", m_trace);
    try
    {
/*        QString filename = QFileDialog::getSaveFileName(this, "Save log file", QDir::currentPath(),"");
        if (!filename.isNull())
        {
            trace.Trace("Filename : %s", filename.toStdString().c_str());
            QLogFileWidget *QLogFile = dynamic_cast<QLogFileWidget *>(tabWidget[m_currentTabIdx]);
            if (QLogFile != nullptr)
            {
                auto itFile = m_mpLogFiles.find(QLogFile->GetFilename());
                if (itFile != m_mpLogFiles.end())
                {
                    // Get filtered entries
                    std::vector<CLogEntry> entries = itFile->GetEntries();
                    std::string m_saveFileName = filename.toStdString();
                    std::string file_extension = m_saveFileName.substr(m_saveFileName.find_last_of(".") + 1);
                    std::transform(file_extension.begin(), file_extension.end(), file_extension.begin(), ::lowercase);
                    if (file_extension == "xml")
                    {
                        saveXmlBegin();
                    }

                    std::for_each(entries.begin(), entries.end(), [=, &file_extension](const CLogEntry& entry){
                        switch(file_extension)
                        {
                            case "csv": saveCsvEntry(); break;
                            case "json": saveJsonEntry(); break;
                            case "xml": saveXmlEntry(); break;
                            case "log":
                            case "txt":
                                saveTextEntry();
                                break;

                            default:
                                break;
                        }
                    });
                }
            }
        }*/
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
void LogViewer::close()
{
    CFuncTracer trace("LogViewer::close", m_trace);
    try
    {

        QLogFileWidget *QLogFile = dynamic_cast<QLogFileWidget*>(tabWidget->widget(m_currentTabIdx));
        if (QLogFile != nullptr)
        {
            auto itFile = m_mpLogFiles.find(m_currentLogFile->Name());
            if (itFile != m_mpLogFiles.end())
            {
                if (QLogFile->GetFilename() == m_currentLogFile->Name())
                {
                    int tab_index = QLogFile->GetTabIndex();
                    tabWidget->removeTab(tab_index);
                    m_mpLogFiles.erase(itFile);
                }
                else
                    trace.Error("QLogfile :%s is not equal to current log file : %s", QLogFile->GetFilename().c_str(), m_currentLogFile->Name().c_str());
            }
            else
                trace.Error("Currentlogfile %s is not found inside the map", m_currentLogFile->Name());
        }
        else
            trace.Error("QLogFile is not of the correct type");
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
void LogViewer::saveXmlBegin(void)
{
    CFuncTracer trace("",m_trace);
    try
    {

    }
    catch(std::exception& ex)
    {
        trace.Error("Exception occurred :%s", ex.what());
    }
    catch(...)
    {
        trace.Error("Exception occurred");
    }
}
void LogViewer::saveCsvEntry(void)
{
    CFuncTracer trace("",m_trace);
    try
    {

    }
    catch(std::exception& ex)
    {
        trace.Error("Exception occurred :%s", ex.what());
    }
    catch(...)
    {
        trace.Error("Exception occurred");
    }
}
void LogViewer::saveJsonEntry(void)
{
    CFuncTracer trace("",m_trace);
    try
    {

    }
    catch(std::exception& ex)
    {
        trace.Error("Exception occurred :%s", ex.what());
    }
    catch(...)
    {
        trace.Error("Exception occurred");
    }
}
void LogViewer::saveXmlEntry(void)
{
    CFuncTracer trace("",m_trace);
    try
    {

    }
    catch(std::exception& ex)
    {
        trace.Error("Exception occurred :%s", ex.what());
    }
    catch(...)
    {
        trace.Error("Exception occurred");
    }
}
void LogViewer::saveTextEntry(void)
{
    CFuncTracer trace("LogViewer::saveTextEntry",m_trace);
    try
    {

    }
    catch(std::exception& ex)
    {
        trace.Error("Exception occurred :%s", ex.what());
    }
    catch(...)
    {
        trace.Error("Exception occurred");
    }
}
