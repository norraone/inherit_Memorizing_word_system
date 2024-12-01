#ifndef LOGIN_VIEW_H
#define LOGIN_VIEW_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include "../../services/user_service.h"

class LoginView : public QWidget {
    Q_OBJECT

private:
    UserService* userService;
    
    // Login form
    QLineEdit* usernameEdit;
    QLineEdit* passwordEdit;
    QPushButton* loginButton;
    QLabel* messageLabel;
    
    // Register form
    QLineEdit* regUsernameEdit;
    QLineEdit* regPasswordEdit;
    QLineEdit* regConfirmPasswordEdit;
    QPushButton* registerButton;
    QLabel* regMessageLabel;
    
    void setupUi();
    void connectSignals();
    void clearForms();
    void showError(QLabel* label, const QString& message);
    void showSuccess(QLabel* label, const QString& message);

private slots:
    void onLoginClicked();
    void onRegisterClicked();
    void onInputChanged();

signals:
    void loginSuccessful(const QString& username);
    void registerSuccessful(const QString& username);

public:
    explicit LoginView(UserService* service, QWidget* parent = nullptr);
    ~LoginView() override = default;
};

#endif // LOGIN_VIEW_H
