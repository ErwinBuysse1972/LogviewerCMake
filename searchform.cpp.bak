#include <functional>

#include "searchform.h"
#include "ui_searchform.h"
#include "cfunctracer.h"
#include "ctracer.h"

SearchForm::SearchForm(std::function<void (const std::string& searchText, bool bError)> SearchBtnHandler,
                       std::shared_ptr<CTracer> tracer,
                       QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SearchForm)
    , m_trace(tracer)
    , m_cbSearchPressed(SearchBtnHandler)

{
    ui->setupUi(this);
}

SearchForm::~SearchForm()
{
    delete ui;
}

QTextEdit *SearchForm::GetRichEditText(void)
{
    return ui->SelectedText;
}

void SearchForm::on_btnSearch_clicked()
{
    CFuncTracer trace("SearchForm::on_btnSearch_clicked", m_trace, false);
    try
    {
        if (ui->cboSearchText->currentText().isEmpty() == false)
        {
            trace.Info("text : %s", ui->cboSearchText->currentText().toStdString().c_str());
            std::string sText = "Dit is een text waar niet mag aangekomen worden " + ui->cboSearchText->currentText().toStdString();
            sText += " voor de verandering blijf je hier ook vanaf";
            ui->SelectedText->setText(QString::fromStdString(sText));

            QList<QTextEdit::ExtraSelection> selections;
            // Actions to perform a selection of a text in a QTextEdit,
            //      the QTextEdit should be implemented in QTable by the function "setCellWidget"
            //      note : those text edits shall slow down the speed of the table so only the cells that contains search text should be changed!
            QTextCharFormat fmt;
            fmt.setUnderlineStyle(QTextCharFormat::SingleUnderline);
            fmt.setForeground(QBrush(QColor(200,0,0)));
            fmt.setUnderlineColor(QColor(200,0,0));
            fmt.setBackground(QBrush(QColor(230,200,200)));

            QTextCursor cursor = ui->SelectedText->textCursor();

            while (! (cursor = ui->SelectedText->document()->find(ui->cboSearchText->currentText(), cursor)).isNull())
            {
                QTextEdit::ExtraSelection sel = {cursor, fmt };
                selections.append(sel);
            }

            // set, show, go!
            ui->SelectedText->setExtraSelections(selections);

            if (m_cbSearchPressed)
                m_cbSearchPressed(ui->cboSearchText->currentText().toStdString(), false);
        }
        else
        {
            trace.Error("no items in the combo box! (len : %d)", ui->cboSearchText->currentText().length());
            if (m_cbSearchPressed)
                m_cbSearchPressed("", true);
        }
    }
    catch(std::exception& ex)
    {
        trace.Error("Exception occurred : %s", ex.what());
    }
}

