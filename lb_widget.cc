#include "lb_widget.h"
#include "ui_lb_widget.h"
#include <QMessageBox>
#include <regex>
#include <QtXlsx/QtXlsx>
#include <QFileDialog>
#include "assert.h"

lb_widget::lb_widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::lb_widget)
{
    ui->setupUi(this);

    takt_time_dlg_ = std::make_unique<takt_time_dlg> (this);
    init();
    init_conn();
//    ui->scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
//    ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

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
    takt_time_dlg_->set_takt_time(QString::number(takt_time_));
}

void lb_widget::on_excel_open()
{
    if (takt_time_ < 0.000001)
    {
        QMessageBox::warning (this, "empty takt time", "请先设置节拍时间");
        return;
    }

    auto qstr_xlsx = QFileDialog::getOpenFileName (this, "Excel Files", ".", tr ("*.xlsx"));
    if (qstr_xlsx.length () == 0)
    {
        return;
    }

    this->current_excel_ = std::move (qstr_xlsx);

    this->read_excel (this->current_excel_);
}

void lb_widget::on_excel_save()
{
    if (this->excel_data_.get().empty())
    {
        QMessageBox::information (this, "excel", "请先打开存储作业内容信息的Excel文档。");
        return;
    }
    auto current_structure = ui->widget->get_data_structure ();

    auto lamda_get_precedence = [&current_structure, this] (int element)
    {
        auto ref_data = this->excel_data_.get ();

        auto iter = ref_data.find (element);
        assert (iter != ref_data.end ());

        auto ret = std::get<2> (iter->second);

        static_assert (std::is_same<decltype (ret), std::set<int>>::value, "type violation");

        return ret;
    };

    std::set<int> white_list_set;

    for (auto &iter : current_structure)
    {
        for (auto & iter_ : iter)
        {
            white_list_set.insert (iter_.id);
        }

        for (auto & iter_ : iter)
        {
            auto precedence_set = lamda_get_precedence (iter_.id);
            auto if_found = std::find_if (precedence_set.begin(), precedence_set.end(), [&white_list_set] (int key)
            {
                return white_list_set.find (key) == white_list_set.end ();
            });

            if (if_found != precedence_set.end() and !precedence_set.empty())
            {
                QMessageBox::warning (this, "sequence error",
                                      "作业内容序号：" + QString::number (*if_found) + "在作业内容序号" + QString::number (iter_.id)+"之后，与Excel表格中定义矛盾！");
                return;
            }
        }
    }

    QXlsx::Document excel {current_excel_};

    excel.deleteSheet (result_sheet);
    excel.addSheet (result_sheet);

    excel.selectSheet (result_sheet);


    while (1)
    {
        auto iter = current_structure.rbegin ();
        if (iter == current_structure.rend () or !iter->empty())
        {
            break;
        }
        current_structure.pop_back ();
    }

    QXlsx::Format fmt;
    fmt.setHorizontalAlignment (QXlsx::Format::AlignHCenter);

    for (int i = 0; static_cast<unsigned> (i) < current_structure.size (); i ++)
    {
        excel.mergeCells (QXlsx::CellRange {1, i * 3 + 1, 1, i * 3 + 3});
        excel.write (1, i * 3 + 1, "station " + QString::number (i + 1));
        excel.setRowFormat (1, 1, fmt);

        excel.write (2, i * 3 + 1, "number");
        excel.write (2, i * 3 + 2, "content");
        excel.write (2, i * 3 + 3, "std time");
    }

    auto i = 0;
    for (auto & iter : current_structure)
    {
        auto j = 0;
        for (auto & iter_ : iter)
        {
            j++;

            auto found = this->excel_data_.get ().find (iter_.id);
            assert (found != this->excel_data_.get ().end ());
            excel.write (j + 2, i * 3 + 1, /*QString::number (iter_.id)*/QVariant {iter_.id});
            excel.write (j + 2, i * 3 + 2, tr (std::get<0>(found->second).data()));
            excel.write (j + 2, i * 3 + 3, QString::number (std::get<1> (found->second), 'f', 1));
        }
        i++;
    }

    assert (!current_structure.empty ());
    unsigned max_length = 0;
    for (auto &iter : current_structure)
    {
        if (iter.size () > max_length)
        {
            max_length = iter.size();
        }
    }

    //auto
    excel.write (2 + 3 + 0 + max_length, 1, tr ("Takt Time"));
    excel.write (2 + 3 + 0 + max_length, 2, QString::number (takt_time_, 'f', 1));

    excel.write (2 + 3 + 1 + max_length, 1, tr ("Max CT"));
    excel.write (2 + 3 + 1 + max_length, 2, QString::number (max_ct_, 'f', 1));

    excel.write (2 + 3 + 2 + max_length, 1, tr ("Balance Rate"));
    excel.write (2 + 3 + 2 + max_length, 2, QString::number (balance_rate_ * 100, 'f', 1) + "%");

    excel.write (2 + 3 + 3 + max_length, 1, tr ("Efficiency"));
    excel.write (2 + 3 + 3 + max_length, 2, QString::number (efficiency_ * 100, 'f', 1) + "%");

    excel.write (2 + 3 + 4 + max_length, 1, tr ("Total Station Number"));
    excel.write (2 + 3 + 4 + max_length, 2, QString::number (current_structure.size()));


    excel.save ();
    QMessageBox::information (this, "save", "保存到了打开的Excel文档中的result工作表");
}

void lb_widget::auto_assign()
{
    if (this->excel_data_.get().empty())
    {
        QMessageBox::information (this, "data", "请先打开存储作业内容信息的Excel文档。");
        return;
    }

    structure_data structure;
    float max_ct, rate, efficiency;
    unsigned station_size;
    std::tie (structure, rate, max_ct, efficiency, station_size) = this->excel_data_.auto_balance (this->takt_time_);
    ui->widget->set_structure (structure, this->takt_time_);


    ui->label_maxct->setText (QString::number (max_ct) + "   ");
    this->max_ct_ = max_ct;

    ui->label_balance_rate->setText (QString::number (rate * 100, 'f', 1) + "%   ");
    this->balance_rate_ = rate;

    ui->label_efficiency->setText (QString::number (efficiency * 100, 'f', 1) +  "%   ");
    this->efficiency_ = efficiency;

    ui->label_station_number->setText (QString::number (station_size) + "   ");
    this->station_number_ = station_size;

    ui->list_unassigned->clear ();
}

void lb_widget::init()
{
    this->setWindowTitle (tr ("Auto Line Balancing"));

    ui->widget->set_params (50, LIST_LENGTH, 1000, 1000);
    ui->list_unassigned->set_tight_bound (false);

    auto lamda_drop_func = [this] ()
    {
        auto data = ui->widget->get_data_structure ();

        for (unsigned i = 0; i < data.size (); i ++)
        {
            for (unsigned j = 0; j < data[i].size (); j ++)
            {
                //data[i][j].time =
                //auto value_found = this->excel_data_.get().find (data[i][j].id);
                //assert (value_found != this->excel_data_.get().end());

                std::string name; float time;
                std::tie (name, time, std::ignore) = this->excel_data_.get().find (data[i][j].id)->second;

                data[i][j].name = std::move (name);
                data[i][j].time = time;
            }
        }


        unsigned station_size = data.size ();
        for (unsigned i = data.size (); i != 0; i --)
        {
            station_size = i;
            qDebug () << "i = " << i;
            if (!data[i - 1].empty())
            {
                break;
            }
        }

        float sigma_ct = 0;
        float max_ct = 0;
        for (auto & iter : data)
        {
            decltype (this->takt_time_) tmp_maxct {};
            for (auto & iter_ : iter)
            {
                tmp_maxct += iter_.time;
                sigma_ct += iter_.time;
            }
            if (tmp_maxct > max_ct)
            {
                max_ct = tmp_maxct;
            }
        }
        qDebug () << "sigma_ct" << sigma_ct;
        qDebug () << "max_rate" << max_ct;
        qDebug () << "station_size" << station_size;

        auto rate = sigma_ct / max_ct / static_cast<float> (station_size);
        auto efficiency = sigma_ct / takt_time_ / static_cast<float> (station_size);

        qDebug () << "rate" << rate;

        this->ui->label_maxct->setText (QString::number (max_ct) + "   ");
        this->max_ct_ = max_ct;

        this->ui->label_balance_rate->setText (QString::number (rate * 100, 'f', 1) + "%   ");
        this->balance_rate_ = rate;

        this->ui->label_efficiency->setText (QString::number (efficiency * 100, 'f', 1) +  "%   ");
        this->efficiency_ = efficiency;

        this->ui->label_station_number->setText (QString::number (station_size) + "   ");
        this->station_number_ = station_size;

    };

    ui->widget->set_drop_func (lamda_drop_func);
    ui->list_unassigned->set_drop_exec (lamda_drop_func);
}

void lb_widget::refresh_data()
{
    ui->widget->clear_structure ();
    ui->list_unassigned->clear ();

    auto data = this->excel_data_.get ();
    std::vector<char> vec;

    for (auto & iter : data)
    {
        vec.resize (2048, '\0');
        auto up_item = std::make_unique<QListWidgetItem> (QString::number (iter.first) + " " + std::get<0> (iter.second).data());
        snprintf (vec.data (), vec.size () - 1, "name:<b>%s</b><br/>std time:<b>%.1f</b>\n", std::get<0>(iter.second).data(), std::get<1>(iter.second));

        up_item->setToolTip (vec.data ());
        up_item->setSizeHint (QSize {100, static_cast<int>(static_cast<float> (LIST_LENGTH) / takt_time_ * std::get<1> (iter.second))});
        ui->list_unassigned->addItem (up_item.release ());

    }
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
