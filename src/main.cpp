#include <QApplication>
#include "./mainwindow/mainwindow.h"
#include "./login/loginwindow.h"
#include "./global/debug.h"
#include "./global/stdafx.h"
#include "./core/processor.h"
#include "./mainwindow/quarter_dialog.h"
#include <string>
using namespace std;

int main(int arg, char* argv[]) {
    qInstallMessageHandler(debugMessage);
    installErrorHandler();
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

    loginWin.login();
    return app.exec();
}
