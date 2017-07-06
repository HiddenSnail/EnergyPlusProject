#ifndef QUARTER_DIALOG_H
#define QUARTER_DIALOG_H

#include <QDialog>
#include "./global/stdafx.h"

namespace Ui {
class QuarterDialog;
}

class QuarterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit QuarterDialog(QWidget *parent = 0);
    QVector<double> getRentalRateVec();
    ~QuarterDialog();

private:
    Ui::QuarterDialog *ui;
    void init();
    void initValidator();
    void initWidgetState();
    void initStyle(QString styleSheetPath);

};

#endif // QUARTER_DIALOG_H
