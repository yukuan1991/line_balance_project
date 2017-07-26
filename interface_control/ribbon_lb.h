#pragma once
#include "ribbon.h"

class ribbon_lb : public ribbon
{
    Q_OBJECT
signals:
    void help();
    void time();
    void job_content();
    void import();
public:
    ribbon_lb(QWidget * parent = nullptr);
private:
    ribbon_tool* job_content_, * import_;
    ribbon_tool* time_;
    ribbon_tool* help_;
};

