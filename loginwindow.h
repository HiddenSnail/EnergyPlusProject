#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H
#include <QMainWindow>
#include <QMessageBox>

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

private slots:
    void on_btn_login_clicked();

private:
    Ui::LoginWindow *ui;
};

#endif // LOGINWINDOW_H
