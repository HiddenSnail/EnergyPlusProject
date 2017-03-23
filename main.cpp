#include <iostream>
#include <QApplication>
#include "mainwindow.h"
#include "handlemachine.h"

int main(int arg, char* argv[]) {
//    QApplication app(arg, argv);
//    MainWindow win;
//    win.show();
//    app.exec();
    std::cout << "Hello World!" << std::endl;
    HandleMachine h;
    std::string i = "love u";
    h.initData("Harbin", "E:\\WorkSpace\\sourcefiles\\25.idf", "E:\\WorkSpace\\cityfiles\\Harbin.json");

}
