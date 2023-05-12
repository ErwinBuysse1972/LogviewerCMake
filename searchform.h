#ifndef SEARCHFORM_H
#define SEARCHFORM_H

#include <QWidget>
#include <QTextEdit>
#include <functional>
#include "cfunctracer.h"
#include "ctracer.h"

namespace Ui {
class SearchForm;
}

class SearchForm : public QWidget
{
    Q_OBJECT

public:
    explicit SearchForm(std::function<void (const std::string& searchText, bool bError)> SearchBtnHandler,
                        std::shared_ptr<CTracer> tracer,
                        QWidget *parent = nullptr);
    ~SearchForm();

    QTextEdit *GetRichEditText(void);
private slots:
    void on_btnSearch_clicked();

private:
    Ui::SearchForm *ui;
    std::shared_ptr<CTracer> m_trace;
    std::function<void (const std::string& searchText, bool bErr)> m_cbSearchPressed;

};

#endif // SEARCHFORM_H
