#ifndef CUSTOM_WIDGET_HPP
#define CUSTOM_WIDGET_HPP
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QProgressDialog>

class CustomProgressDialog: public QProgressDialog {
public:
    CustomProgressDialog(QWidget *parent = Q_NULLPTR);
};

CustomProgressDialog::CustomProgressDialog(QWidget *parent)
    :QProgressDialog(parent)
{
    QLabel *label = new QLabel(parent);
    label->setFont(QFont("Microsoft YaHei", 10));
    this->setLabel(label);

    QProgressBar *bar = new QProgressBar(parent);
    bar->setStyleSheet(".QProgressBar {"\
                       "background-color: rgb(68, 69, 73);"\
                       "border-radius: 5px;"\
                       "color: rgb(255, 255, 255);"\
                       "height: 40px;"\
                       "font-family: 'Microsoft YaHei';"\
                       "}"
                       ".QProgressBar::chunk {"\
                       "background-color: rgb(38, 190, 200);"\
                       "border-radius: 5px;"\
                       "}");
    bar->setAlignment(Qt::AlignCenter);
    this->setBar(bar);

    QPushButton *button = new QPushButton(parent);
    button->setDisabled(true);
    button->setStyleSheet(".QPushButton {"\
                          "border-radius: 5px;"\
                          "font-family: 'Microsoft YaHei';"\
                          "padding: 10px 18px;"\
                          "color: rgb(0, 0, 0);"\
                          "background-color: rgb(255, 255, 255);"\
                          "border: 1px solid rgb(143, 143, 143);"\
                          "}"\
                          ".QPushButton:hover {"\
                          "background-color: rgba(223, 232, 242, 0.5);"\
                          "}"\
                          ".QPushButton:pressed {"\
                          "background-color: rgb(223, 232, 242);"\
                          "}");
    button->setText(QString::fromLocal8Bit("取消"));
    this->setCancelButton(button);

}

#endif //CUSTOM_WIDGET_HPP
