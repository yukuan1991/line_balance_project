#include "ribbon_lb.h"
#include <base/lang/move.hpp>

ribbon_lb::ribbon_lb(QWidget *parent)
    : ribbon(parent)
{
    {
        std::array<ui_group, 1> edit;

        button_cell b;


        b.add ("节拍时间", QPixmap ("png/节拍时间.png"), time_);
        b.set_title("属性设置");

        edit[0] = ::move (b);

        add_tab(edit, "设置");
    }
    {
        std::array<ui_group, 2> edit;

        button_cell b;

        b.add ("导入", QPixmap ("png/导入.png"), import_);
        b.add ("导出", QPixmap ("png/导出.png"), export_);
        b.set_title("导入EXCEL");

        edit[0] = ::move (b);

        b.add ("作业内容", QPixmap ("png/作业内容.png"), job_content_);
        b.set_title("自动分配");

        edit[1] = ::move (b);

        add_tab(edit, "编辑");
    }


    connect(time_, &ribbon_tool::clicked, this, &ribbon_lb::time);
    connect(job_content_, &ribbon_tool::clicked, this, &ribbon_lb::job_content);
    connect(import_, &ribbon_tool::clicked, this, &ribbon_lb::import);
    connect(export_, &ribbon_tool::clicked, this, &ribbon_lb::export_clicked);

    connect(this, &ribbon_lb::set_enabled, import_, &ribbon_tool::setEnabled);
    connect(this, &ribbon_lb::set_enabled, export_, &ribbon_tool::setEnabled);
    connect(this, &ribbon_lb::set_enabled, job_content_, &ribbon_tool::setEnabled);
    connect(this, &ribbon_lb::set_enabled, time_, &ribbon_tool::setEnabled);
}
