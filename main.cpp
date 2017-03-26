#include <iostream>
#include <QApplication>
#include "mainwindow.h"
#include "handlemachine.h"
#include "utils.h"

int main(int arg, char* argv[]) {
//    QApplication app(arg, argv);
//    MainWindow win;
//    win.show();
//    app.exec();
//    HandleMachine baseMachine("E:\\WorkSpace\\sourcefiles\\25.idf");
//    baseMachine.initCityData("Harbin", "E:\\WorkSpace\\city\\harbin.json");
//    baseMachine.configure("E:\\WorkSpace\\base\\base_config.json");

//    std::vector<std::string> dataVec = {"225.4"};
//    std::vector<std::string> dataVec2 = {"363.9"};
//    baseMachine.operate("E:\\WorkSpace\\base\\base_operation.json","opElectricEquipment",dataVec);
//    baseMachine.operate("E:\\WorkSpace\\base\\base_operation.json","opLights", dataVec2);
//    if (baseMachine.separate()) {
//        HandleMachine nrModelMachine("E:\\WorkSpace\\output\\25_nr.idf");
//        HandleMachine rModelMachine("E:\\WorkSpace\\output\\25_r.idf");
//        HandleMachine rpModelMachine("E:\\WorkSpace\\output\\25_rp.idf");

//        nrModelMachine.configure("E:\\WorkSpace\\model\\nr\\nr_config.json");
//        rModelMachine.configure();
//        rpModelMachine.configure();
//        rpModelMachine.operate();
//    }
//    HandleMachine nrModelMachine("E:\\WorkSpace\\output\\25_nr.idf");
//    nrModelMachine.configure("E:\\WorkSpace\\model\\nr\\nr_config.json");
//    nrModelMachine.output();
//    std::vector<std::string> data = {"Until: 99:00", "Until: 100:00", "90"};
//    HandleMachine rpModelMachine("E:\\WorkSpace\\output\\25_rp.idf");
//    rpModelMachine.configure("E:\\WorkSpace\\model\\rp\\rp_config.json");
//    rpModelMachine.operate("E:\\WorkSpace\\model\\rp\\rp_operation.json", "opSchComCool", data);
//    rpModelMachine.output();
    std::cout << Utils::getFileName("E:\\WorkSpace\\output\\25_rp.idf") << std::endl;
}
