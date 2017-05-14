#include <QApplication>
#include "mainwindow.h"
#include "loginwindow.h"
#include "handlemachine.h"
#include <direct.h>
#include <stdlib.h>
#include "debug.h"
#include <QJsonDocument>
#include "global.h"
#include "pathmanager.h"

int main(int arg, char* argv[]) {
    qInstallMessageHandler(debugMessage);
    QApplication app(arg, argv);


    MainWindow win;
    win.show();
    QFile qssFile(":/style/mainstyle.qss");
    if(qssFile.open(QFile::ReadOnly)) {
        qInfo() << "Qss file open success";
        win.setStyleSheet(qssFile.readAll());
        qssFile.close();
    } else {
        qDebug() << "Qss file open fail";
    }

//    LoginWindow loginWin;
//    loginWin.show();
//    qssFile.setFileName(":/style/login.qss");
//    if(qssFile.open(QFile::ReadOnly)) {
//        qInfo() << "Qss file open success";
//        loginWin.setStyleSheet(qssFile.readAll());
//        qssFile.close();
//    } else {
//        qDebug() << "Qss file open fail";
//    }

//    QObject::connect(&loginWin, &LoginWindow::loginSuccess, [&win](){
//        win.show();
//    });

    return app.exec();
}
