#include <QApplication>
#include "./mainwindow/mainwindow.h"
#include "./login/loginwindow.h"
#include "./global/debug.h"
#include "./global/stdafx.h"
#include "test/ColumnTest.h"


int main(int arg, char* argv[]) {
    qInstallMessageHandler(debugMessage);
    installErrorHandler();
    QApplication app(arg, argv);
    MainWindow win;
    LoginWindow loginWin;
    QObject::connect(&loginWin, &LoginWindow::loginSuccess, [&]() {
        loginWin.close();
        win.show();
    });
    QObject::connect(&loginWin, &LoginWindow::updateLang, &win, &MainWindow::setLanguage);
    loginWin.login();
//    ColumnTest obj;
//    QTest::qExec(&obj);
    return app.exec();
}
