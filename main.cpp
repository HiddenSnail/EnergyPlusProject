#include <iostream>
#include <QApplication>
#include "mainwindow.h"
#include "handlemachine.h"
#include "util.h"

int main(int arg, char* argv[]) {
//    QApplication app(arg, argv);
//    MainWindow win;
//    win.show();
//    app.exec();
    HandleMachine h;
    h.initCityData("Harbin", "E:\\WorkSpace\\sourcefiles\\25.idf", "E:\\WorkSpace\\cityfiles\\harbin.json");
//    checkJsonFile("E:\\WorkSpace\\cityfiles\\xharbin.json");

}
