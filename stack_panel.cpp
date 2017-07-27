#include "stack_panel.h"
#include <QDebug>
#include <assert.h>
#include <QMimeData>
#include <QToolTip>
#include <QMouseEvent>

static const char* qss =
        R"(
        QListWidget
        {
        border: 0px;
        }
        QWidget

        QListWidget:Item::hover
        {
        background-color:#D5E1F2;
        border-width: 1px;
        border-color: red;
        }

        QListWidget:Item
        {
        border: 1px solid black;
        }
        QListWidget:Item::selected
        {
        color: black;
        }

        )";
//background-color:#C4D79B;

std::function<std::unique_ptr<QListWidgetItem> ()> stack_panel::_move_exec = nullptr;
unsigned stack_panel::u_size__ = 0;
QListWidgetItem* stack_panel::item_drag = nullptr;
stack_panel* stack_panel::panel_taken;


void stack_panel::init()
{
    this->setAttribute (Qt::WA_TranslucentBackground, true);
    this->setStyleSheet (qss);
    setMouseTracking (true);
    this->setDragEnabled(true);
    this->setDropIndicatorShown (true);
    this->setDragDropMode(QListWidget::DragDrop);
    this->setDefaultDropAction(Qt::CopyAction);

}

void stack_panel::refresh_current_size()
{
    current_size = 0;
    for (int i = 0; i < this->count (); i++)
    {
        current_size += this->item (i)->sizeHint ().height ();
    }
}

void stack_panel::leaveEvent (QEvent *)
{
    if (last_item_ == nullptr)
    {
        return;
    }

}

void stack_panel::mouseMoveEvent(QMouseEvent *event)
{
    QListWidget::mouseMoveEvent (event);

    auto item = itemAt (event->pos ());
    if (item == last_item_)
    {
        return;
    }

    last_item_ = item;

    if (item == nullptr)
    {
        return;
    }

    QToolTip::showText (event->globalPos (), item->toolTip (), this);
}

void stack_panel::dragMoveEvent(QDragMoveEvent *event)
{
    event->setDropAction (Qt::CopyAction);
    QListWidget::dragMoveEvent (event);
}

void stack_panel::dropEvent(QDropEvent *event)
{
    assert (item_drag != nullptr);
    auto height_add = item_drag->sizeHint ().height ();
    unsigned total_height = this->size ().height ();
    assert (height_add > 0);
    assert (total_height > 0);

    if (bound_detect_ and panel_taken != this and static_cast<unsigned>(height_add) + current_size > total_height)
    {

        event->ignore ();

        if (this->func_exceed != nullptr)
        {
            this->func_exceed ();
        }

        return;
    }

    event->setDropAction (Qt::CopyAction);
    QListWidget::dropEvent (event);

    this->current_size += height_add;

    if (_move_exec != nullptr)
    {
        stack_panel::_move_exec ();
    }

    if (this->drop_exec_!= nullptr)
    {
        this->drop_exec_ ();
    }
}

void stack_panel::startDrag(Qt::DropActions supportedActions)
{
    panel_taken = this;

    item_drag = this->item (currentRow ());

    auto lamda_take_item = [this] ()
    {
        this->current_size -= item_drag->sizeHint ().height ();
        return std::unique_ptr<QListWidgetItem> {item_drag};
    };

    stack_panel::_move_exec = std::move (lamda_take_item);

    supportedActions = Qt::CopyAction;
    QListWidget::startDrag (supportedActions);
}

void stack_panel::set_exceed_limit_action(std::function<void ()> func)
{
    this->func_exceed = std::move (func);
}
