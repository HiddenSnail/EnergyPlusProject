#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H
#include <QMainWindow>
#include <QMessageBox>
#include "./include/mainwindow/mainwindow.h"
#include "usraccount.h"

namespace Ui {
class LoginWindow;
}

class LoginWindow: public QMainWindow
{
     Q_OBJECT
public:
    explicit LoginWindow(QWidget *parent = 0);
    void inputUserId();
    ~LoginWindow();

signals:
    void loginSuccess();
    void updateLang(MainWindow::Language lang);

private slots:
    void on_btn_login_clicked();
    void on_btn_lang_clicked();

private:
    Ui::LoginWindow *ui;

};

#endif // LOGINWINDOW_H
