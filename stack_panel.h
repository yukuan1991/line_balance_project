#ifndef STACK_PANEL_H
#define STACK_PANEL_H
#include <QListWidget>
#include <functional>
#include <memory>


class stack_panel : public QListWidget
{
    Q_OBJECT
public:
    template<typename ... ARGS>
    stack_panel(ARGS ... args): QListWidget (std::forward<ARGS> (args) ... )
    {
        this->init ();
    }
    virtual compl stack_panel ()
    {
        //last_item_ = nullptr;
    }
    void set_tight_bound (bool yes_or_no = true) {this->bound_detect_ = yes_or_no;}

    void init ();
    void refresh_current_size ();
    virtual void leaveEvent (QEvent* event) override;
    virtual void mouseMoveEvent (QMouseEvent* event) override;

    virtual void dragMoveEvent(QDragMoveEvent *event) override;
    virtual void dropEvent(QDropEvent *event) override;
    void startDrag (Qt::DropActions supportedActions) override;
    void set_exceed_limit_action (std::function<void ()> func);
    void set_drop_exec (std::function<void()> func)
    {
        drop_exec_ = std::move (func);
    }
private: //static
    static std::function<std::unique_ptr<QListWidgetItem> ()> _move_exec;
    static unsigned u_size__;
    static QListWidgetItem* item_drag;
    static stack_panel* panel_taken;

private:
    std::function<void ()> func_exceed;
    QListWidgetItem* last_item_ = nullptr;
    unsigned current_size = 0;
    bool bound_detect_ = true;
    std::function<void ()> drop_exec_;

};

#endif // STACK_PANEL_H
