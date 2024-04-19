#include <QApplication>
#include "spicewindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SpiceWindow w;
    w.show();

    return a.exec();
}
