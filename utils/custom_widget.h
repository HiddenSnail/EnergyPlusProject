#ifndef CUSTOM_WIDGET_H
#define CUSTOM_WIDGET_H
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QProgressDialog>
#include <QTimer>

class CustomProgressDialog: public QProgressDialog {
    Q_OBJECT
public:
    CustomProgressDialog(QWidget *parent = Q_NULLPTR);
    void update(int stepId, QString detail);
signals:
    void lSignal();
    void zSignal();
    void mSignal();
    void showSignal();
};

#endif //CUSTOM_WIDGET_HPP
