#include <QApplication>
#include "snipater.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Snipater sapp;
    return a.exec();
}
