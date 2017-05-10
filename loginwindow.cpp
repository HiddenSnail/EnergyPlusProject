#include "loginwindow.h"
#include "ui_login.h"
#include "userid_creator.hpp"
#include "passowrd_checker.hpp"


LoginWindow::LoginWindow(QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::LoginWindow)
{
    ui->setupUi(this);
    QImage backGround(":/icons/login.png");
    ui->centralwidget->setAutoFillBackground(true);   // 这个属性一定要设置
    QPalette pal;
    pal.setBrush(QPalette::Window, QBrush(backGround.scaled(size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation)));
    ui->centralwidget->setPalette(pal);
    ui->edit_user_name->setReadOnly(true);
    inputUserId();
}

void LoginWindow::inputUserId()
{
    MACAddress address = GetNetBiosMacAddresses().front();
    std::string userId = createUserId(address);
    ui->edit_user_name->setText(QString(userId.c_str()));
}

LoginWindow::~LoginWindow()
{
    delete ui;
}

void LoginWindow::on_btn_login_clicked()
{
    QString userId = ui->edit_user_name->text();
    QString password = ui->edit_password->text();
    if (checkPassword(userId.toStdString(), password.toStdString()))
    {
        this->hide();
        emit loginSuccess();
    } else {
        QMessageBox::warning(this,QString::fromLocal8Bit("提示"),QString::fromLocal8Bit("密码错误，登录失败！"));
    }
}
