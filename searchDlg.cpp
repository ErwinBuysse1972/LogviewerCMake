#include "search.h"
#include "searchDlg.h"

#include <QWidget>
#include <QCoreApplication>


SearchDlg::SearchDlg(std::function<void (const std::string& searchText)> SearchBtnHandler,
                     std::shared_ptr<CTracer> tracer,
                     QWidget *parent)
    : QWidget(parent)
    , btnSearch(nullptr)
    , lineEdit(nullptr)
    , lblSearch(nullptr)
    , m_trace(tracer)
    , m_cbSearchPressed(SearchBtnHandler)
{
    init_create_dialog();
}

SearchDlg::~SearchDlg()
{
    if (btnSearch != nullptr)
    {
        delete btnSearch;
        btnSearch = nullptr;
    }
    if (lineEdit != nullptr)
    {
        delete lineEdit;
        lineEdit = nullptr;
    }
    if (lblSearch != nullptr)
    {
        delete lblSearch;
        lblSearch = nullptr;
    }

    // For playing and testing
    if (textEdit != nullptr)
    {
        delete textEdit;
        textEdit = nullptr;
    }
}

void SearchDlg::init_create_dialog(void)
{
    CFuncTracer trace("SearchDlg::init_create_dialog", m_trace);
    try
    {
        btnSearch = new QPushButton(this);
        btnSearch->setObjectName("btnSearch");
        btnSearch->setGeometry(QRect(630, 60, 89, 25));
        lineEdit = new QLineEdit(this);
        lineEdit->setObjectName("lineEdit");
        lineEdit->setGeometry(QRect(90, 60, 521, 25));
        lblSearch = new QLabel(this);
        lblSearch->setObjectName("lblSearch");
        lblSearch->setGeometry(QRect(100, 17, 271, 20));

        // for testing and playing
        textEdit = new QTextEdit(this);
        textEdit->setObjectName("textEdit");
        textEdit->setGeometry(QRect(70,130,621,41));

        retranslateUi();
        connect(btnSearch, &QPushButton::clicked, this, &SearchDlg::onSearchButtonPressed);

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

void SearchDlg::retranslateUi(void)
{
    CFuncTracer trace("SearchDlg::retranslateUi", m_trace);
    try
    {
        setWindowTitle(QCoreApplication::translate("Search", "Search", nullptr));
        btnSearch->setText(QCoreApplication::translate("Dialog", "Search", nullptr));
        lblSearch->setText(QCoreApplication::translate("Dialog", "Please enter the text you want to find", nullptr));
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

void SearchDlg::onSearchButtonPressed(void)
{
    CFuncTracer trace("SearchDlg::onSearchButtonPressed", m_trace, false);
    try
    {
        trace.Info("text : %s", lineEdit->text().toStdString().c_str());
        std::string sText = "Dit is een text waar niet mag aangekomen worden " + lineEdit->text().toStdString();
        sText = " voor de verandering blijf je hier ook vanaf";
        textEdit->setText(QString::fromStdString(sText));

        if (m_cbSearchPressed)
            m_cbSearchPressed(lineEdit->text().toStdString());
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
