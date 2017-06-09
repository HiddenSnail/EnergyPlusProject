#include <QApplication>
#include "./mainwindow/mainwindow.h"
#include "./login/loginwindow.h"
#include "./global/debug.h"
#include "./global/stdafx.h"
#include "./mainwindow/handlemachine.h"

int sum(int a, ...)
{
    return a;
}

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

//    HandleMachine h(PathManager::instance()->getPath("OutPutDir") + "/60/base/base.idf");
//    h.operateTest(PathManager::instance()->getPath("ModelDir") + "/operation.json",
//                  OperateFactory::opSchComCoolOrHeat(14, false));
//    h.save();

//    QStringList l;
//    l << "Until: 6:00" << "20" << "Until: 22:00" << "23" << "Until: 24:00" << "20";
//    qDebug() << OperateFactory::opSchComCoolOrHeat(QTime(13,0), 4, 3, false).second(l);

    return app.exec();
}
