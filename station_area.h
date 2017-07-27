#ifndef STATION_AREA_H
#define STATION_AREA_H

#include <functional>
#include <QWidget>
#include <line_label.h>
#include <vector>
#include <stack_panel.h>

struct item_info
{
    unsigned id;
    std::string name;
    float time;
};
using structure_data = std::vector<std::vector<item_info>>;

class station_area : public QWidget
{
    Q_OBJECT
public:
    template<typename ... ARGS>
    explicit station_area (ARGS&& ... args): QWidget (std::forward<ARGS>(args)...)
    {
        init ();
    }
    void set_drop_func (std::function<void()> func)
    {
        drop_func_ = func;
    }

    void init ();
    void set_params (unsigned startline_height, unsigned list_height, int width, int height);
    void set_structure (const structure_data& info, float takt_time, unsigned int list_width = 100, unsigned int spaces = 30);
    void clear_structure ();
    structure_data get_data_structure ();
    bool structure_empty ();

signals:

public slots:

private:
    static constexpr unsigned label_height = 20;
private:
    unsigned list_height_;
    unsigned startline_height_;
    std::vector<QLabel*> station_names_;
    std::vector<line_label*> start_lines_;
    std::vector<stack_panel*> stacks_;
    std::vector<line_label*> bottom_lines_;

    std::function<void()> drop_func_;
};


template<typename T>
struct type_iter
{
    using pointer_type = T*;
    using value_type = T;
    using ref_type = T&;
    using rval_ref_value = T&&;
};

template<typename T>
struct type_iter<T*>
{
    using pointer_type = typename type_iter<T>::pointer_type;
    using value_type = typename type_iter<T>::value_type;
    using ref_type = typename type_iter<T>::ref_type;
    using rval_ref_value = typename type_iter<T>::rval_ref_value;

};

#endif // STATION_AREA_H
