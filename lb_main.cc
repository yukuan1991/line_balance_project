﻿#include "lb_main.h"
#include "ui_lb_main.h"
#include "lb_widget.h"
#include <memory>
#include <QMdiSubWindow>
#include "interface_control/takt_time_dlg.h"
#include <QDebug>

using namespace std;

lb_main::lb_main(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::lb_main)
{
    ui->setupUi(this);
    ui->mdi->setViewMode (QMdiArea::TabbedView);
    setWindowState(Qt::WindowMaximized);
    setMinimumSize(QSize(1000, 800));

    init_conn();
    set_button_enabled();
}

lb_main::~lb_main()
{
    delete ui;
}

void lb_main::init_conn()
{
    connect(ui->widget_ribbon, &ribbon_lb::file_menu_triggered, [this] (const QString & s) { file_operations(s); });

    connect(ui->widget_ribbon, &ribbon_lb::job_content, this, &lb_main::auto_assign);
    connect(ui->widget_ribbon, &ribbon_lb::import, this, &lb_main::import);
    connect(ui->widget_ribbon, &ribbon_lb::export_clicked, this, &lb_main::export_clicked);
    connect(ui->widget_ribbon, &ribbon_lb::time, this, &lb_main::takt_time_exec);
    connect(ui->mdi, &QMdiArea::subWindowActivated, ui->widget_ribbon, &ribbon_lb::set_enabled);
}

void lb_main::set_button_enabled()
{
    const bool state = (active_window() != nullptr);
    ui->widget_ribbon->set_enabled(state);
}

void lb_main::file_operations(const QString &s)
{
    if(s == "新建")
    {
        file_new();
    }
    else if(s == "退出")
    {
        close();
    }
}

void lb_main::file_new()
{
    auto w = create_window ("未命名");
}

void lb_main::auto_assign()
{
    auto w = active_window();
    if(w == nullptr)
    {
        return;
    }

    w->auto_assign();
}

void lb_main::import()
{
    auto w = active_window();
    if(w == nullptr)
    {
        return;
    }

    w->on_excel_open();
}

void lb_main::export_clicked()
{
    auto w = active_window();
    if(w == nullptr)
    {
        return;
    }

    w->on_excel_save();
}

void lb_main::takt_time_exec()
{
    auto w = active_window();
    if(w == nullptr)
    {
        return;
    }

    w->takt_time_exec();
}

not_null<lb_widget *> lb_main::create_window(const QString &title)
{
    auto lb_win = make_unique<lb_widget> ();
    auto ptr_lb_win = lb_win.get();

    lb_win->setAttribute (Qt::WA_DeleteOnClose);
    auto w = ui->mdi->addSubWindow (ptr_lb_win);
    w->setWindowTitle (title);

    w->setWindowState (Qt::WindowMaximized);

    return lb_win.release ();
}

lb_widget *lb_main::active_window()
{
    if(QMdiSubWindow* active_subwindow = ui->mdi->activeSubWindow())
    {
        lb_widget* w = dynamic_cast<lb_widget*>(active_subwindow->widget());
        return w;
    }
    else
    {
        return nullptr;
    }
}
