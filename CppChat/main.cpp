#include "mainwindow.h"

#include <QApplication>
#include <QFile>
#include "stylemanager.h"
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // 加载样式表
    auto res = StyleManager::changeStyleSheet(Styles::DARK);

    MainWindow w;
    w.show();

    if(!res){
        QMessageBox::warning(nullptr,"加载异常","qss样式表加载异常,请检查文件是否损坏");
    }
    return a.exec();
}
