#include <iostream>
#include <QApplication>
#include "mainwindow.h"
#include "handlemachine.h"

int main(int arg, char* argv[]) {
    QApplication app(arg, argv);
    MainWindow win;
    win.show();
    app.exec();
//    std::cout << "Hello World!" << std::endl;
//    HandleMachine h;
//    h.initData("Harbin", "sourcefiles/25.idf", "cityfiles/Harbin.json");

}
