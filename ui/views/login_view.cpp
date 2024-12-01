#include "login_view.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QMessageBox>

LoginView::LoginView(UserService* service, QWidget* parent)
    : QWidget(parent), userService(service) {
    setupUi();
    connectSignals();
}

void LoginView::setupUi() {
    auto* mainLayout = new QVBoxLayout(this);
    
    // Login form
    auto* loginGroup = new QGroupBox("登录", this);
    auto* loginLayout = new QFormLayout(loginGroup);
    
    usernameEdit = new QLineEdit(loginGroup);
    passwordEdit = new QLineEdit(loginGroup);
    passwordEdit->setEchoMode(QLineEdit::Password);
    loginButton = new QPushButton("登录", loginGroup);
    messageLabel = new QLabel(loginGroup);
    messageLabel->setStyleSheet("QLabel { color: red; }");
    
    loginLayout->addRow("用户名:", usernameEdit);
    loginLayout->addRow("密码:", passwordEdit);
    loginLayout->addRow(messageLabel);
    loginLayout->addRow(loginButton);
    
    // Register form
    auto* registerGroup = new QGroupBox("注册", this);
    auto* registerLayout = new QFormLayout(registerGroup);
    
    regUsernameEdit = new QLineEdit(registerGroup);
    regPasswordEdit = new QLineEdit(registerGroup);
    regPasswordEdit->setEchoMode(QLineEdit::Password);
    regConfirmPasswordEdit = new QLineEdit(registerGroup);
    regConfirmPasswordEdit->setEchoMode(QLineEdit::Password);
    registerButton = new QPushButton("注册", registerGroup);
    regMessageLabel = new QLabel(registerGroup);
    regMessageLabel->setStyleSheet("QLabel { color: red; }");
    
    registerLayout->addRow("用户名:", regUsernameEdit);
    registerLayout->addRow("密码:", regPasswordEdit);
    registerLayout->addRow("确认密码:", regConfirmPasswordEdit);
    registerLayout->addRow(regMessageLabel);
    registerLayout->addRow(registerButton);
    
    mainLayout->addWidget(loginGroup);
    mainLayout->addWidget(registerGroup);
    mainLayout->addStretch();
}

void LoginView::connectSignals() {
    connect(loginButton, &QPushButton::clicked,
            this, &LoginView::onLoginClicked);
    connect(registerButton, &QPushButton::clicked,
            this, &LoginView::onRegisterClicked);
            
    // Clear error messages when input changes
    connect(usernameEdit, &QLineEdit::textChanged,
            this, &LoginView::onInputChanged);
    connect(passwordEdit, &QLineEdit::textChanged,
            this, &LoginView::onInputChanged);
    connect(regUsernameEdit, &QLineEdit::textChanged,
            this, &LoginView::onInputChanged);
    connect(regPasswordEdit, &QLineEdit::textChanged,
            this, &LoginView::onInputChanged);
    connect(regConfirmPasswordEdit, &QLineEdit::textChanged,
            this, &LoginView::onInputChanged);
}

void LoginView::onLoginClicked() {
    QString username = usernameEdit->text().trimmed();
    QString password = passwordEdit->text();
    
    if (username.isEmpty() || password.isEmpty()) {
        showError(messageLabel, "请输入用户名和密码");
        return;
    }
    
    try {
        userService->login(username.toStdString(), password.toStdString());
        showSuccess(messageLabel, "登录成功");
        emit loginSuccessful(username);
        clearForms();
    } catch (const AuthenticationError& e) {
        showError(messageLabel, QString::fromStdString(e.what()));
    } catch (const std::exception& e) {
        showError(messageLabel, "登录失败");
    }
}

void LoginView::onRegisterClicked() {
    QString username = regUsernameEdit->text().trimmed();
    QString password = regPasswordEdit->text();
    QString confirmPassword = regConfirmPasswordEdit->text();
    
    if (username.isEmpty() || password.isEmpty() || confirmPassword.isEmpty()) {
        showError(regMessageLabel, "所有字段都必须填写");
        return;
    }
    
    if (password != confirmPassword) {
        showError(regMessageLabel, "两次输入的密码不一致");
        return;
    }
    
    try {
        userService->registerUser(username.toStdString(), password.toStdString());
        showSuccess(regMessageLabel, "注册成功，请登录");
        emit registerSuccessful(username);
        clearForms();
    } catch (const std::exception& e) {
        showError(regMessageLabel, QString::fromStdString(e.what()));
    }
}

void LoginView::onInputChanged() {
    messageLabel->clear();
    regMessageLabel->clear();
}

void LoginView::clearForms() {
    usernameEdit->clear();
    passwordEdit->clear();
    regUsernameEdit->clear();
    regPasswordEdit->clear();
    regConfirmPasswordEdit->clear();
    messageLabel->clear();
    regMessageLabel->clear();
}

void LoginView::showError(QLabel* label, const QString& message) {
    label->setStyleSheet("QLabel { color: red; }");
    label->setText(message);
}

void LoginView::showSuccess(QLabel* label, const QString& message) {
    label->setStyleSheet("QLabel { color: green; }");
    label->setText(message);
}
