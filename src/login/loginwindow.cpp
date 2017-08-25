#include "./login/loginwindow.h"
#include "ui_loginwindow.h"


LoginWindow::LoginWindow(QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::LoginWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("DELITEK");
    QImage backGround(":/res/icons/background.jpg");
    ui->centralwidget->setAutoFillBackground(true);   // 这个属性一定要设置
    QPalette pal;
    pal.setBrush(QPalette::Window, QBrush(backGround.scaled(size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation)));
    ui->centralwidget->setPalette(pal);
    ui->edit_user_name->setReadOnly(true);
    QFile qssFile(":/res/stylesheet/login.qss");
    if(qssFile.open(QFile::ReadOnly))
    {
        this->setStyleSheet(qssFile.readAll());
        qssFile.close();
    }
    inputUserId();
}

void LoginWindow::inputUserId()
{
    MacAddressList macAddressList;
    if (MachineInfo::getMacAddressList(macAddressList) == MachineInfo::SUCCESS)
    {
        MacAddress macAddress(macAddressList.back());
        QString userId = UsrAccount::createUsrId(macAddress);
        ui->edit_user_name->setText(userId);
    }
    else
    {
        qFatal("Your computer may not have network card, can't create id for you!");
    }
}

LoginWindow::~LoginWindow()
{
    delete ui;
}

void LoginWindow::on_btn_login_clicked()
{
    QString userId = ui->edit_user_name->text();
    QString password = ui->edit_password->text();
    if (UsrAccount::checkPassword(userId, password))
    {
        UsrAccount::rememberPassword(password);
        this->hide();
        emit loginSuccess();
    } else {
        QMessageBox::warning(this,QString::fromLocal8Bit("提示"),QString::fromLocal8Bit("密码错误，登录失败！"));
    }
}

void LoginWindow::login()
{
    QString password;
    if (UsrAccount::getPassword(password) == 0)
    {
        ui->edit_password->setText(password);
        this->show();
    }
    else
    {
        this->show();
    }
}

void LoginWindow::on_btn_check_clicked()
{
    static bool isCheck = true;
    if (isCheck)
    {
        ui->btn_check->setIcon(QIcon(":/res/icons/checkbox-off.png"));
        isCheck = false;
    }
    else
    {
        ui->btn_check->setIcon(QIcon(":/res/icons/checkbox-on.png"));
        isCheck = true;
    }
}
