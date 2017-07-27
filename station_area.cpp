#include "station_area.h"
#include <memory>
#include <assert.h>
#include <boost/lexical_cast.hpp>
#include <QMessageBox>
#include "lb_widget.h"

#include <QDebug>


void station_area::init()
{

}

void station_area::set_params(unsigned startline_height, unsigned list_height, int width, int height)
{
    this->startline_height_ = startline_height;
    this->list_height_ = list_height;
    this->resize (width, height);
}

void station_area::set_structure(const structure_data& info, float takt_time, unsigned int list_width, unsigned int spaces)
{
    this->clear_structure ();

    unsigned station_number = info.size ();
    this->resize (spaces + (list_width + spaces) *  station_number, this->startline_height_ + this->list_height_ + 100);

    for (unsigned i = 0; i < station_number; i++)
    {
        auto pos_width = spaces + i * (list_width + spaces);

        auto up_station_names = std::make_unique<typename type_iter<typename decltype (this->station_names_)::value_type>::value_type> (this);
        up_station_names->move (pos_width, startline_height_ - label_height);
        up_station_names->setText ("station " + QString::number (i + 1));
        up_station_names->show ();
        this->station_names_.emplace_back (up_station_names.release ());

        auto up_line = std::make_unique<typename type_iter<typename decltype (this->start_lines_)::value_type>::value_type> (this);
        up_line->resize (list_width + spaces, up_line->height());
        up_line->move (pos_width, startline_height_);
        up_line->show ();
        this->start_lines_.emplace_back (up_line.release ());

        auto up_stack = std::make_unique<typename type_iter<typename decltype (this->stacks_)::value_type>::value_type> (this);
        up_stack->move (pos_width, startline_height_);
        up_stack->resize (list_width, list_height_);
        up_stack->set_drop_exec (this->drop_func_);
        up_stack->set_exceed_limit_action ([this] () {QMessageBox::warning (this, "exceed limit", "无法移动，超过节拍时间限制。");});
        up_stack->show ();
        //add item
        {
            auto& list_content = info[i];

            std::vector<char> vec_buffer;
            vec_buffer.resize (2048, '\0');

            for (auto & iter : list_content)
            {
                memset (vec_buffer.data (), 0x00, vec_buffer.size ());
                auto item = std::make_unique<QListWidgetItem> (QString::number (iter.id) + " " + iter.name.data());
                snprintf (vec_buffer.data (), vec_buffer.size () - 1, "name:<b>%s</b><br/>std time:<b>%.1f</b>", iter.name.data(), iter.time);
                item->setToolTip (vec_buffer.data ());
                item->setSizeHint (QSize{100, static_cast<int>(static_cast<float> (LIST_LENGTH) / takt_time * iter.time)});

                up_stack->addItem (item.release ());
            }
        }
        up_stack->refresh_current_size ();

        this->stacks_.emplace_back (up_stack.release ());

        auto up_bottom_line = std::make_unique<typename type_iter<typename decltype (this->bottom_lines_)::value_type>::value_type> (this);
        up_bottom_line->move (pos_width, startline_height_ + list_height_ - 1);
        up_bottom_line->resize (list_width + spaces, up_bottom_line->height());
        up_bottom_line->show ();
        this->bottom_lines_.emplace_back (up_bottom_line.release());
    }

}

void station_area::clear_structure()
{
    auto clear_container = [] (auto container_input)
    {
        std::for_each (std::begin (container_input), end (container_input), [] (typename decltype (container_input)::value_type it)
        {
            delete it;
        });
        container_input.clear ();
    };

    clear_container (std::move (this->station_names_));
    clear_container (std::move (this->start_lines_));
    clear_container (std::move (this->stacks_));
    clear_container (std::move (this->bottom_lines_));
}

structure_data station_area::get_data_structure()
{
    structure_data current_structure;
    for (unsigned i = 0; i < stacks_.size (); i++)
    {
        current_structure.push_back ({});
        for (int j = 0; j < stacks_[i]->count(); j++)
        {
            auto item = stacks_[i]->item (j);
            std::string str = item->text ().toStdString();
            assert (str.find_first_of (' ') != std::string::npos);
            auto str_num = str.substr (0, str.find_first_of (' '));

            unsigned num = boost::lexical_cast<unsigned> (str_num);
            current_structure.back ().push_back (item_info{num, std::string{}, float{}});
        }
    }

    return current_structure;
}

bool station_area::structure_empty()
{
    return this->stacks_.empty ();
}




