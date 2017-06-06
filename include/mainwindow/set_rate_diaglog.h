#ifndef SET_RATE_DIALOG_H
#define SET_RATE_DIALOG_H
#include <QDialog>
#include <QGridLayout>
#include <QLineEdit>
#include <QLabel>
#include <QFile>
#include <QLineEdit>
#include <QList>
#include "./global/stdafx.h"

namespace Ui {
class SetRateDialog;
}

class SetRateDialog: public QDialog
{
    Q_OBJECT
public:
    explicit SetRateDialog(QWidget *parent = 0);
    ~SetRateDialog();
    QList<double> getInRoomRateList();

private:
    Ui::SetRateDialog *_pUi;
    QList<double> _defaultValueList;
    QList<QLineEdit*> _editList;

    void init();
    void initCoreData();
    void initWidgetState();
    void initValidator();
    void initStyle(QString styleSheetPath);

    void resetValues();
};

#endif //SET_RATE_DIALOG_H
