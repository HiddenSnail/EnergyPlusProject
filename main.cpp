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
    win.show();
    return app.exec();
}
