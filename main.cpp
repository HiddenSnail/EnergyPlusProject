#include <QApplication>
#include "mainwindow.h"
#include "handlemachine.h"
#include "utils.h"
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
//    // 以下部分解决中文乱码
//    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));

    win.show();
    return app.exec();
}
