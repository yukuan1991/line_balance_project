#include "lb_main.h"
#include "ui_lb_main.h"

lb_main::lb_main(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::lb_main)
{
    ui->setupUi(this);
}

lb_main::~lb_main()
{
    delete ui;
}
