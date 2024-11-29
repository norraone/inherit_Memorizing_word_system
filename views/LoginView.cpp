#include "LoginView.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QStyle>

LoginView::LoginView(QWidget* parent) : QWidget(parent) {
    setupUI();
    connectSignals();
    
    // 设置样式
    setStyleSheet(R"(
        QWidget {
            background-color: #f5f5f5;
        }
        QLineEdit {
            padding: 8px;
            border: 1px solid #ddd;
            border-radius: 4px;
            font-size: 14px;
            min-width: 250px;
        }
        QPushButton {
            padding: 8px 16px;
            border: none;
            border-radius: 4px;
            font-size: 14px;
            min-width: 100px;
        }
        QPushButton#loginButton {
            background-color: #4CAF50;
            color: white;
        }
        QPushButton#loginButton:hover {
            background-color: #45a049;
        }
        QPushButton#registerButton {
            background-color: #2196F3;
            color: white;
        }
        QPushButton#registerButton:hover {
            background-color: #1976D2;
        }
        QLabel {
            font-size: 14px;
            color: #333;
        }
    )");
}

void LoginView::setupUI() {
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(50, 50, 50, 50);

    // Logo或标题
    auto* titleLabel = new QLabel("单词记忆系统", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #333; margin-bottom: 20px;");
    mainLayout->addWidget(titleLabel);

    // 用户名输入
    auto* userLayout = new QHBoxLayout();
    auto* userLabel = new QLabel("用户名:", this);
    usernameInput = new QLineEdit(this);
    usernameInput->setPlaceholderText("请输入用户名");
    userLayout->addWidget(userLabel);
    userLayout->addWidget(usernameInput);
    mainLayout->addLayout(userLayout);

    // 密码输入
    auto* passLayout = new QHBoxLayout();
    auto* passLabel = new QLabel("密码:", this);
    passwordInput = new QLineEdit(this);
    passwordInput->setPlaceholderText("请输入密码");
    passwordInput->setEchoMode(QLineEdit::Password);
    passLayout->addWidget(passLabel);
    passLayout->addLayout(passLayout);
    mainLayout->addLayout(passLayout);

    // 按钮布局
    auto* buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(10);

    loginButton = new QPushButton("登录", this);
    loginButton->setObjectName("loginButton");
    registerButton = new QPushButton("注册", this);
    registerButton->setObjectName("registerButton");

    buttonLayout->addWidget(loginButton);
    buttonLayout->addWidget(registerButton);
    mainLayout->addLayout(buttonLayout);

    // 状态标签
    statusLabel = new QLabel(this);
    statusLabel->setAlignment(Qt::AlignCenter);
    statusLabel->setStyleSheet("color: #f44336;"); // 红色错误提示
    mainLayout->addWidget(statusLabel);

    // 添加弹性空间
    mainLayout->addStretch();
}

void LoginView::connectSignals() {
    connect(loginButton, &QPushButton::clicked, this, &LoginView::handleLoginAttempt);
    connect(registerButton, &QPushButton::clicked, this, &LoginView::switchToRegister);
    
    // 按Enter键也可以登录
    connect(usernameInput, &QLineEdit::returnPressed, this, &LoginView::handleLoginAttempt);
    connect(passwordInput, &QLineEdit::returnPressed, this, &LoginView::handleLoginAttempt);
}

void LoginView::handleLoginAttempt() {
    QString username = usernameInput->text().trimmed();
    QString password = passwordInput->text();

    if (username.isEmpty() || password.isEmpty()) {
        statusLabel->setText("用户名和密码不能为空");
        return;
    }

    // 尝试登录
    User::LoginResult result = User::login(username.toStdString(), password.toStdString());
    
    if (result.success) {
        statusLabel->clear();
        emit loginSuccess(result.user);
    } else {
        statusLabel->setText(QString::fromStdString(result.message));
        passwordInput->clear();
        passwordInput->setFocus();
    }
}
