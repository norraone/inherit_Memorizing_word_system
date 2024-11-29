#ifndef LOGINVIEW_H
#define LOGINVIEW_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include "../user.h"

class LoginView : public QWidget {
    Q_OBJECT

private:
    QLineEdit* usernameInput;
    QLineEdit* passwordInput;
    QPushButton* loginButton;
    QPushButton* registerButton;
    QLabel* statusLabel;

    void setupUI();
    void connectSignals();

signals:
    void loginSuccess(User* user);
    void switchToRegister();

private slots:
    void handleLoginAttempt();

public:
    explicit LoginView(QWidget* parent = nullptr);
};

#endif // LOGINVIEW_H
