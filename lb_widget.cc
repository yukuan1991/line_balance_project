#include "lb_widget.h"
#include "ui_lb_widget.h"
#include <QMessageBox>
#include <regex>
#include <QtXlsx/QtXlsx>


lb_widget::lb_widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::lb_widget)
{
    ui->setupUi(this);

    takt_time_dlg_ = std::make_unique<takt_time_dlg> (this);

}

lb_widget::~lb_widget()
{
    delete ui;
}

void lb_widget::takt_time_exec()
{
    takt_time_dlg_->exec();
}

void lb_widget::init_conn()
{
    connect(takt_time_dlg_.get(), &takt_time_dlg::confirm, this, &lb_widget::takt_time_confirm);
    connect(takt_time_dlg_.get(), &takt_time_dlg::cancel, this, &lb_widget::takt_time_cancel);

}

void lb_widget::takt_time_confirm()
{
    if (!this->excel_data_.get ().empty ())
    {
        auto ret = QMessageBox::information (this, "takt time", "节拍时间重置后，所有未保存的修改会丢失，你确定吗？", QMessageBox::Ok | QMessageBox::Cancel);
        if (ret != QMessageBox::Ok)
        {
            return;
        }
    }

    bool is_ok;
    auto tmp = takt_time_dlg_->text ().toFloat (&is_ok);

    if (!is_ok)
    {
        QMessageBox::information (this, "wrong format", "节拍时间输入格式错误！");
        return;
    }

    this->takt_time_ = tmp;
    this->ui->label_show_takt->setText (QString::number (takt_time_) + "   ");

    this->read_excel (this->current_excel_);

    ui->widget->clear_structure ();
    takt_time_dlg_->hide();
}

void lb_widget::takt_time_cancel()
{
    takt_time_dlg_->hide();
}

void lb_widget::refresh_data()
{

}

void lb_widget::read_excel(const QString &qstr_xlsx)
{
    QXlsx::Document xlsx_doc {qstr_xlsx};
    QXlsx::Format fmt;

    unsigned n_start_row = 2;
    decltype (this->excel_data_) tmp_data;

    for (unsigned i = 0;; i++)
    {
        if (i > MAX_ROW)
        {
            QMessageBox::information (this, "max row", "这个Excel超出了试用版最大范围行数 (" + QString::number (MAX_ROW) + ")");
            return;
        }

        auto qstr_num = xlsx_doc.read (n_start_row + i, 1).toString ();
        if (qstr_num.length () == 0)
        {
            break;
        }
        bool is_ok;
        int num = qstr_num.toInt (&is_ok);
        if (!is_ok)
        {
            QMessageBox::warning (this, "excel error", "excel format error:row "
                                  + QString::number (i + n_start_row) + "第一列不是数字格式");
            return;
        }

        auto qstr_name = xlsx_doc.read (n_start_row + i, 2).toString ();

        auto qstr_time = xlsx_doc.read (n_start_row + i, 3).toString ();
        float f_time = qstr_time.toFloat (&is_ok);

        if (!is_ok)
        {
            QMessageBox::warning (this, "excel error", "excel format error:row "
                                  + QString::number (i + n_start_row) + "第三列不是数字格式");
            return;
        }
        auto qstr_precedence = xlsx_doc.read (n_start_row + i, 4).toString ();

        std::set<int> precedence;

        std::regex expression ("\\d+");
        std::smatch hit;

        auto lamda_number_search = [&] (const std::string& str_search)
        {
            std::string::const_iterator start = str_search.begin ();
            std::string::const_iterator stop = str_search.end ();
            while (std::regex_search(start, stop, hit, expression))
            {
                precedence.insert (QString (hit[0].str().data()).toInt ());
                start = hit[0].second;
            }
        };

        lamda_number_search (qstr_precedence.toStdString ());

        if (f_time > this->takt_time_)
        {
            QMessageBox::information (this, "takt time", "作业内容序号" + QString::number (num) + "的标准工时" + QString::number (f_time) + "大于节拍时间");
            return;
        }

        tmp_data.add_operation (num, qstr_name.toStdString (), f_time, precedence);
    }

    tmp_data.print ();


    std::list<int> loop;
    if (tmp_data.find_loop (loop) == true)
    {
        QString qstr_loop;
        for_each (std::begin (loop), std::end (loop), [&qstr_loop] (int i)
        {
            qstr_loop += QString::number (i) + " ";
        });
        QMessageBox::warning (this, "loop detected", "在工艺流程中存在环流矛盾:" + qstr_loop + "。（返修流程不作为考虑对象）");
        return;
    }

    float f_sigma = 0.000F;

    for (auto &iter : tmp_data.get())
    {
        f_sigma += std::get<1> (iter.second);
    }

    this->excel_data_ = std::move (tmp_data);

    this->refresh_data ();

    ui->widget->set_params (50, LIST_LENGTH, 1000, 1000);
}
