#ifndef LB_WIDGET_H
#define LB_WIDGET_H

#include <QWidget>

namespace Ui {
class lb_widget;
}

class lb_widget : public QWidget
{
    Q_OBJECT

public:
    explicit lb_widget(QWidget *parent = 0);
    ~lb_widget();

private:
    Ui::lb_widget *ui;
};

#endif // LB_WIDGET_H
