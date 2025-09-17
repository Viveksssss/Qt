#include <QApplication>
#include <QCommandLineParser>
#include <QUrl>
#include "mainwindow.h"

int main(int argc,char**argv){
    QCoreApplication::setApplicationName("PDFViewer");

    QApplication app(argc,argv);

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("file","The file to open.");

    MainWindow w;
    w.show();

    if(!parser.positionalArguments().isEmpty()){
        w.open(QUrl::fromLocalFile(parser.positionalArguments().constFirst()));
    }
    app.exec();
}
