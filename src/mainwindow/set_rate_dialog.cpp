#include "./mainwindow/set_rate_diaglog.h"
#include "ui_set_rate_dialog.h"
#include "./utils/custom_validator.hpp"

SetRateDialog::SetRateDialog(QWidget *parent):
    QDialog(parent),
    _pUi(new Ui::SetRateDialog )
{
    _pUi->setupUi(this);
    init();
}



SetRateDialog::~SetRateDialog()
{
    delete _pUi;
}

//初始化函数
void SetRateDialog::init()
{
    initCoreData();
    initWidgetState();
    initValidator();
    initStyle(":/res/stylesheet/rate_dialog.qss");

}

void SetRateDialog::initCoreData()
{
    _editList.push_back(_pUi->edit_rate_1);
    _editList.push_back(_pUi->edit_rate_2);
    _editList.push_back(_pUi->edit_rate_3);
    _editList.push_back(_pUi->edit_rate_4);
    _editList.push_back(_pUi->edit_rate_5);
    _editList.push_back(_pUi->edit_rate_6);
    _editList.push_back(_pUi->edit_rate_7);
    _editList.push_back(_pUi->edit_rate_8);
    _editList.push_back(_pUi->edit_rate_9);
    _editList.push_back(_pUi->edit_rate_10);
    _editList.push_back(_pUi->edit_rate_11);
    _editList.push_back(_pUi->edit_rate_12);
    _editList.push_back(_pUi->edit_rate_13);
    _editList.push_back(_pUi->edit_rate_14);
    _editList.push_back(_pUi->edit_rate_15);
    _editList.push_back(_pUi->edit_rate_16);
    _editList.push_back(_pUi->edit_rate_17);
    _editList.push_back(_pUi->edit_rate_18);
    _editList.push_back(_pUi->edit_rate_19);
    _editList.push_back(_pUi->edit_rate_20);
    _editList.push_back(_pUi->edit_rate_21);
    _editList.push_back(_pUi->edit_rate_22);
    _editList.push_back(_pUi->edit_rate_23);
    _editList.push_back(_pUi->edit_rate_24);

    QFile inRoomRateFile(PathManager::instance()->getPath("ProfileDir") + "/gudu_profile.json");
    if (!inRoomRateFile.open(QFile::ReadOnly))
    {
        qFatal("Can't read the in room rate profile!");
    }
    QTextStream iRRFStream(&inRoomRateFile);
    iRRFStream.setCodec("UTF-8");
    QJsonDocument iRRFDoc = QJsonDocument::fromJson(iRRFStream.readAll().toUtf8());
    inRoomRateFile.close();
    if (iRRFDoc.isEmpty() || iRRFDoc.isNull())
    {
        qFatal("The in room rate profile maybe broken!");
    }
    QJsonObject iRRFRoot = iRRFDoc.object();
    for (int i = 0; i < 24; i++)
    {
        double inRoomRate = iRRFRoot[QString::number(i+1)].toDouble();
        inRoomRate = inRoomRate <= 1 ? inRoomRate : 1;
        _defaultValueList.push_back(inRoomRate);
    }
}

void SetRateDialog::initWidgetState()
{
    resetValues();
    connect(_pUi->btn_srd_default, &QPushButton::clicked, this, &SetRateDialog::resetValues);
    connect(_pUi->btn_srd_cancel, &QPushButton::clicked, this, [=]()
    {
        resetValues();
        this->hide();
    });
    connect(_pUi->btn_srd_ok, &QPushButton::clicked, this, &SetRateDialog::hide);

}

void SetRateDialog::initValidator()
{
    DoubleValidator *p0To1Valdtor = new DoubleValidator(0.00, 1.00, 2, this);
    for (auto &edit: _editList)
    {
        edit->setValidator(p0To1Valdtor);
    }
}

void SetRateDialog::initStyle(QString styleSheetPath)
{
    QFile qssFile(styleSheetPath);
    if(qssFile.open(QFile::ReadOnly))
    {
        this->setStyleSheet(qssFile.readAll());
        qssFile.close();
    }
    else
    {
        qFatal("Qss file open fail");
    }
}

/**
 * @brief SetRateDialog::resetValues >> 恢复在室系数默认值
 */
void SetRateDialog::resetValues()
{
    for (int i = 0; i < _defaultValueList.size(); i++)
    {
        _editList[i]->setText(QString::number(_defaultValueList[i]));
    }
}


QList<double>SetRateDialog::getInRoomRateList()
{
    QList<double> rateList;
    for (auto &edit: _editList)
    {
        rateList.push_back(edit->text().toDouble());
    }
    return rateList;
}

