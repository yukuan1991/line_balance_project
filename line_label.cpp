#include "line_label.h"
#include <QPaintEvent>
#include <QPainter>


void line_label::init()
{

}


void line_label::paintEvent (QPaintEvent *)
{
    QPainter qp;
    qp.begin(this);
    this->draw_lines (qp);
    qp.end();
}

void line_label::draw_lines(QPainter &qp)
{
    auto width = this->size().width();
    qp.drawLine(0, 0, width, 0);
}
