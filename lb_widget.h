#ifndef LB_WIDGET_H
#define LB_WIDGET_H

#include <QWidget>
#include <memory>
#include "interface_control/takt_time_dlg.h"
#include "data_balance.h"

#pragma  GCC diagnostic ignored  "-Wwrite-strings"
constexpr unsigned MAX_ROW = 30;
constexpr unsigned LIST_LENGTH = 500;
constexpr char* result_sheet = "result";
#pragma  GCC diagnostic warning  "-Wwrite-strings"

namespace Ui {
class lb_widget;
}


class lb_widget : public QWidget
{
    Q_OBJECT

public:
    explicit lb_widget(QWidget *parent = 0);
    ~lb_widget();

    void takt_time_exec();
private:
    void init_conn();
private:
    void takt_time_confirm();
    void takt_time_cancel();
private: //functions
    void refresh_data ();
    void read_excel (const QString&);
private:
    std::unique_ptr<takt_time_dlg> takt_time_dlg_;
private:
    Ui::lb_widget *ui;

    data_balance excel_data_;
    float takt_time_ = 0;
    float max_ct_ = 0;
    float balance_rate_ = 0;
    float efficiency_ = 0;
    unsigned station_number_ = 0;
    QString current_excel_ = {};
};

#endif // LB_WIDGET_H
