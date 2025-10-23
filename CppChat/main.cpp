#include "mainwindow.h"
#include "stylemanager.h"
#include "Properties/global.h"

#include <QApplication>
#include <QFile>
#include <QDir>
#include <QSettings>
#include <QMessageBox>



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // 加载样式表
    auto res = StyleManager::changeStyleSheet(Styles::BLUE);

    QString configName = "config.ini";
    QString app_path = QDir::currentPath();
    QString configPath = QDir::toNativeSeparators(app_path+QDir::separator() + configName);
    QSettings settings(configPath,QSettings::IniFormat);
    QString gate_host = settings.value("GateWayServer/host").toString();
    QString gate_port = settings.value("GateWayServer/port").toString();
    gate_url_prefix = "http://"+gate_host+":"+gate_port;

    MainWindow w;
    w.show();
    if(!res){
        QMessageBox::warning(nullptr,"加载异常","qss样式表加载异常,请检查文件是否损坏");
    }
    return a.exec();
}
