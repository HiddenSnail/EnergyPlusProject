#include "./mainwindow/quarter_dialog.h"
#include "ui_quarter_dialog.h"
#include "./utils/custom_validator.hpp"

QuarterDialog::QuarterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QuarterDialog)
{
    ui->setupUi(this);
    init();
}

QuarterDialog::~QuarterDialog()
{
    delete ui;
}

void QuarterDialog::init()
{
    initStyle(":/res/stylesheet/quarter_dialog.qss");
    initWidgetState();
    initValidator();
}

void  QuarterDialog::initWidgetState()
{
    ui->edit_quarter1->setText("0");
    ui->edit_quarter2->setText("0");
    ui->edit_quarter3->setText("0");
    ui->edit_quarter4->setText("0");
    connect(ui->btn_ok, &QPushButton::clicked, this, &QuarterDialog::hide);
}

void QuarterDialog::initValidator()
{
    DoubleValidator *p0To100Valdtor = new DoubleValidator(0.00, 100.00, 2, this);
    ui->edit_quarter1->setValidator(p0To100Valdtor);
    ui->edit_quarter2->setValidator(p0To100Valdtor);
    ui->edit_quarter3->setValidator(p0To100Valdtor);
    ui->edit_quarter4->setValidator(p0To100Valdtor);
}

void QuarterDialog::initStyle(QString styleSheetPath)
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

QVector<double> QuarterDialog::getRentalRateVec()
{
    QVector<double> rentalRateVec;
    rentalRateVec.push_back(ui->edit_quarter1->text().toDouble()/100.0);
    rentalRateVec.push_back(ui->edit_quarter2->text().toDouble()/100.0);
    rentalRateVec.push_back(ui->edit_quarter3->text().toDouble()/100.0);
    rentalRateVec.push_back(ui->edit_quarter4->text().toDouble()/100.0);
    return rentalRateVec;
}
