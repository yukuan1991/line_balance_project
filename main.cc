#include "lb_main.h"
#include <QApplication>
#include <base/io/file/file.hpp>
#include <boost/filesystem.hpp>
#include <boost/range/adaptors.hpp>
#include <QStyleFactory>



void set_style ()
{
    using namespace boost::filesystem;

    auto rng = boost::make_iterator_range (directory_iterator ("."), directory_iterator ());

    std::string qss;
    for (auto & it : rng)
    {
        if (it.path ().extension ().string () == ".css")
        {
            auto res = file::read_all (it.path ().string ().data ());
            if (res)
            {
                qss += res.value ();
            }
        }
    }
    qApp->setStyle (QStyleFactory::create ("fusion"));

    qApp->setStyleSheet (QString::fromStdString (qss));
    qApp->setFont (QFont ("宋体", 11));
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    set_style();
    lb_main w;
    w.show();


    return a.exec();
}
