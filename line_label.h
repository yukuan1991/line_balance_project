#ifndef REDLINE_LABEL_H
#define REDLINE_LABEL_H

#include <QLabel>
#include <QObject>

class line_label : public QLabel
{
public:
    template<typename ... ARGS>
    line_label(ARGS&&... args): QLabel (std::forward<ARGS> (args)...)
    {
        init ();
    }
    void init ();
    virtual void paintEvent (QPaintEvent *event) override;
    void draw_lines (QPainter&);
};

#endif // REDLINE_LABEL_H
