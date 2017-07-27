#include "takt_time_dlg.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>


takt_time_dlg::takt_time_dlg(QWidget *parent, Qt::WindowFlags f)
    : QDialog (parent, f)
{
    QVBoxLayout* layout = new QVBoxLayout;
    QHBoxLayout* hlayout1 = new QHBoxLayout;
    QHBoxLayout* hlayout2 = new QHBoxLayout;
    label_ = new QLabel("请设置节拍时间:", this);
    takt_time_ = new QLineEdit(this);
    confirm_ = new QPushButton("确定", this);
    cancel_ = new QPushButton("取消", this);

    hlayout1->addWidget(label_);
    hlayout1->addWidget(takt_time_);

    hlayout2->addWidget(confirm_);
    hlayout2->addWidget(cancel_);

    layout->addLayout(hlayout1);
    layout->addLayout(hlayout2);

    setLayout(layout);
    setFixedSize(220, 80);

//    connect(unit_, &QComboBox::currentTextChanged, this, &unit_dlg::currenttextchanged);
    connect(confirm_, &QPushButton::clicked, this, &takt_time_dlg::confirm);
    connect(cancel_, &QPushButton::clicked, this, &takt_time_dlg::cancel);

}
