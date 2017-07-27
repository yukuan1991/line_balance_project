#ifndef LB_MAIN_H
#define LB_MAIN_H

#include <QWidget>
#include <base/lang/not_null.h>

namespace Ui {
class lb_main;
}

class lb_widget;

class lb_main : public QWidget
{
    Q_OBJECT

public:
    explicit lb_main(QWidget *parent = 0);
    ~lb_main();

private:
    void init_conn();
    void set_button_enabled();
private:
    void file_operations(const QString& s);
    void file_new ();

private:
    void auto_assign();
    void import();
    void export_clicked();
    void takt_time_exec();
    void help_advice();
private:
    not_null<lb_widget*> create_window (const QString & title);
    lb_widget* active_window ();
private:
    Ui::lb_main *ui;
};

#endif // LB_MAIN_H
