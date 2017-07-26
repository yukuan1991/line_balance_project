#ifndef LB_MAIN_H
#define LB_MAIN_H

#include <QWidget>

namespace Ui {
class lb_main;
}

class lb_main : public QWidget
{
    Q_OBJECT

public:
    explicit lb_main(QWidget *parent = 0);
    ~lb_main();

private:
    Ui::lb_main *ui;
};

#endif // LB_MAIN_H
