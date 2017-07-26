#include "lb_main.h"
#include "ui_lb_main.h"
#include "interface_control/about_us_dlg.h"
#include "lb_widget.h"
#include <memory>
#include <QMdiSubWindow>

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
}

lb_main::~lb_main()
{
    delete ui;
}

void lb_main::init_conn()
{
    connect(ui->widget_ribbon, &ribbon_lb::file_menu_triggered, [this] (const QString & s) { file_operations(s); });

    connect(ui->widget_ribbon, &ribbon_lb::help, this, &lb_main::help_advice);
}

void lb_main::file_operations(const QString &s)
{
    if(s == "新建")
    {
        file_new();
    }
//    else if(s == "打开")
//    {
//        file_open();
//    }
//    else if(s == "保存")
//    {
//        file_save();
//    }
//    else if(s == "另存为")
//    {
//        file_save_as();
//    }
    else if(s == "退出")
    {
        close();
    }
}

void lb_main::file_new()
{
    auto w = create_window ("未命名");
}

void lb_main::help_advice()
{
    const QString text = R"(<html><head/><body><p>如果您有任何需求与改进建议，</p><p>请随时联系IEToolkit君qq3350436646</p>
                         <p><span style=" font-weight:600; color:red">加好友请扫右边二维码---&gt;</span></p></body></html>)";
    const QString qr_code = R"( <a href = "www.shionto.com"> <img src="./png/about-us.png" width="300" height = "400"/></a>)";
    about_us_dlg::show_info(text, qr_code);
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
