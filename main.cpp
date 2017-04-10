#include <QApplication>
#include "mainwindow.h"
#include "handlemachine.h"
#include "utils.h"
#include <direct.h>
#include <stdlib.h>
#include "debug.h"
#include <QJsonDocument>
#include "global.h"

int main(int arg, char* argv[]) {
    qInstallMessageHandler(debugMessage);
//    QApplication app(arg, argv);
//    MainWindow win;
//    win.show();
//    app.exec();
//    HandleMachine h(":/data/resource/source/20.idf");
//    h.initCityData("Harbin");
//    QVector<QString> s = {"999"};
//    h.operate(":/data/resource/model/base/base_operation.json", "opElectricEquipment", s);
//    h.separate();
    HandleMachine s("output\\20\\base\\base.idf");
    QVector<QString> y = {"66666"};
    s.operate(":/data/resource/model/base/base_operation.json", "opElectricEquipment", y);
    s.save();
    return 0;
}
