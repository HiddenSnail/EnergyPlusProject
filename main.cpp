#include <iostream>
#include <QApplication>
#include "mainwindow.h"
#include "handlemachine.h"
#include "utils.h"
#include <direct.h>
#include <stdlib.h>

int main(int arg, char* argv[]) {
//    QApplication app(arg, argv);
//    MainWindow win;
//    win.show();
//    app.exec();
    HandleMachine baseMachine("source\\25.idf");
    baseMachine.initCityData("beijing");
    std::vector<std::string> v = {DEFAUL_VALUE};
    baseMachine.operate(BASE_OP_FILE, "opElectricEquipment", v);
    baseMachine.separate();
//    std::string workDir = "E:\\WorkSpace\\QT\\build-EnergyPlusProject-Desktop_Qt_5_8_0_MSVC2015_64bit-Debug";
//    std::string targetPath = "E:\\WorkSpace\\QT\\build-EnergyPlusProject-Desktop_Qt_5_8_0_MSVC2015_64bit-Debug\\output\\25";
//    _chdir(targetPath.c_str());
//    char buffer[10000];
//    _getcwd(buffer,10000);
//    std::cout << buffer << std::endl;
//    std::string s1 = workDir + "\\EneryPlus\\EnergyPlusV8-1-0\\ExpandObjects.exe";
//    system(s1.c_str());
//    rename("in.idf","in.idf.original");
//    rename("expanded.idf","in.idf");
//    std::string s2 = workDir + "\\EneryPlus\\EnergyPlusV8-1-0\\EnergyPlus.exe";
//    system(s2.c_str());
    return 0;
}
