#include "ribbon_lb.h"
#include <base/lang/move.hpp>

ribbon_lb::ribbon_lb(QWidget *parent)
    : ribbon(parent)
{
    {
        std::array<ui_group, 2> edit;

        button_cell b;

        b.add ("导入", QPixmap ("png/导入.png"), import_);
        b.set_title("导入EXCEL");

        edit[0] = ::move (b);

        b.add ("作业内容", QPixmap ("png/作业内容.png"), job_content_);
        b.set_title("自动分配");

        edit[1] = ::move (b);

        add_tab(edit, "编辑");
    }
    {
        std::array<ui_group, 1> edit;

        button_cell b;


        b.add ("节拍时间", QPixmap ("png/节拍时间.png"), time_);
        b.set_title("属性设置");

        edit[0] = ::move (b);

        add_tab(edit, "设置");
    }
    {
        std::array<ui_group, 1> help;

        button_cell b;
        b.add ("帮助", QPixmap ("png/帮助.png"), help_);
        b.set_title("联系我们");

        help[0] = ::move(b);

        add_tab(help, "帮助");
    }

    connect(help_, &ribbon_tool::clicked, this, &ribbon_lb::help);
    connect(time_, &ribbon_tool::clicked, this, &ribbon_lb::time);
    connect(job_content_, &ribbon_tool::clicked, this, &ribbon_lb::job_content);
    connect(import_, &ribbon_tool::clicked, this, &ribbon_lb::import);


}
