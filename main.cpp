#include "habbit_tracker.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Habbit_tracker w;
    w.show();


    return a.exec();
}
