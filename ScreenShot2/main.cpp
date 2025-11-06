#include <QApplication>
#include <QClipboard>
#include <QPainter>
#include "snipater.h"


int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    // testClipboardBasic();
    Snipater sapp;
    return a.exec();
}
