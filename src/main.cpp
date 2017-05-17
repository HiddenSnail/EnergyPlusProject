#include <QApplication>
#include "./include/mainwindow/mainwindow.h"
#include "./include/login/loginwindow.h"
#include "./include/global/debug.h"

int main(int arg, char* argv[]) {
    qInstallMessageHandler(debugMessage);
    QApplication app(arg, argv);

    MainWindow win;
    QFile qssFile(":/res/stylesheet/main.qss");
    if(qssFile.open(QFile::ReadOnly)) {
        qInfo() << "Qss file open success";
        win.setStyleSheet(qssFile.readAll());
        qssFile.close();
    } else {
        qFatal("Qss file open fail");
    }

    LoginWindow loginWin;
    qssFile.setFileName(":/res/stylesheet/login.qss");
    if(qssFile.open(QFile::ReadOnly)) {
        qInfo() << "Qss file open success";
        loginWin.setStyleSheet(qssFile.readAll());
        qssFile.close();
    } else {
        qFatal("Qss file open fail");
    }

    QObject::connect(&loginWin, &LoginWindow::loginSuccess, [&](){
        loginWin.close();
        win.show();
    });

    QObject::connect(&loginWin, &LoginWindow::updateLang, &win, &MainWindow::setLanguage);
    loginWin.show();
    return app.exec();
}
