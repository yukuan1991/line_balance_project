#include "lb_main.h"
#include <QApplication>
#include <base/io/file/file.hpp>
#include <boost/filesystem.hpp>
#include <boost/range/adaptors.hpp>
#include <QStyleFactory>
#include "verification.h"
#include <QTimer>
#include <chrono>

using namespace std::string_literals;
using namespace std::chrono_literals;


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

    if (!verification_process ())
    {
        return -1;
    }

    set_style();
    lb_main w;
    w.show();

    QTimer timer;
    timer.setInterval (1s);
    timer.setSingleShot (true);
    QObject::connect (&timer, &QTimer::timeout, [&] { check_date (); timer.start (); });
    timer.start ();

    return a.exec();

    return a.exec();
}
