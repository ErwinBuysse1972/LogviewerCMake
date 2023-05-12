#pragma once
#include <functional>
#include <QWidget>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>

#include "cfunctracer.h"
#include "ctracer.h"


class SearchDlg : public QWidget
{

private slots:
    void onSearchButtonPressed(void);
public:
    explicit SearchDlg(std::function<void (const std::string& searchText)> SearchBtnHandler,
                       std::shared_ptr<CTracer> tracer,
                       QWidget *parent = nullptr);
    ~SearchDlg();
    QTextEdit* GetRichEditText(void){ return textEdit;}

private:
    QPushButton *btnSearch;
    QLineEdit *lineEdit;
    QLabel *lblSearch;
    QTextEdit *textEdit;

    std::shared_ptr<CTracer> m_trace;
    std::function<void (const std::string& searchText)> m_cbSearchPressed;

    void init_create_dialog(void);
    void retranslateUi();
};
