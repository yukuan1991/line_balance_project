#include "lb_widget.h"
#include "ui_lb_widget.h"

lb_widget::lb_widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::lb_widget)
{
    ui->setupUi(this);
}

lb_widget::~lb_widget()
{
    delete ui;
}
