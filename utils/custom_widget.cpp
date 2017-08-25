#include "custom_widget.h"

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

void CustomProgressDialog::update(int stepId, QString detail)
{
    QTimer *timer = new QTimer();
    int toValue = 0;
    switch (stepId) {
    case 0:
        toValue = 10;
        break;
    case 1:
        toValue = 55;
        break;
    case 2:
        toValue = 85;
        break;
    case 3:
        toValue = 100;
        break;
    default:
    {
        exit (9090);
        break;
    }
    }

    connect(timer, &QTimer::timeout, [=](){
        int value = this->value();
        if (value < toValue - 1)
        {
            this->setValue(value + 1);
        }
        else
        {
            timer->stop();
            delete timer;
            this->setValue(value + 1);
            this->setLabelText(detail);
            switch (stepId) {
            case 0:
                emit lSignal();
                break;
            case 1:
                emit zSignal();
                break;
            case 2:
                emit mSignal();
                break;
            case 3:
                emit showSignal();
                break;
            default:
                break;
            }
        }
    });
    timer->start(50);
}
