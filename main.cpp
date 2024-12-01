#include <QApplication>
#include <QFile>
#include <QStyleFactory>
#include "ui/views/login_view.h"
#include "services/user_service.h"
#include "services/word_service.h"
#include "services/review_service.h"
#include "services/statistics_service.h"

// Project Structure:
/*
inherit_memorizing_word_system/
├── models/
│   ├── user.cpp/h
│   ├── word.cpp/h
│   └── review_session.cpp/h
├── repositories/
│   ├── base_repository.cpp/h
│   ├── user_repository.cpp/h
│   └── word_repository.cpp/h
├── services/
│   ├── user_service.cpp/h
│   ├── word_service.cpp/h
│   ├── review_service.cpp/h
│   └── statistics_service.cpp/h
├── ui/views/
│   ├── login_view.cpp/h
│   ├── vocabulary_view.cpp/h
│   ├── review_view.cpp/h
│   └── statistics_view.cpp/h
├── resources.qrc
└── main.cpp
*/

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Set application style
    app.setStyle(QStyleFactory::create("Fusion"));
    
    // Load and apply stylesheet
    QFile styleFile(":/styles/dark.qss");
    if (styleFile.open(QFile::ReadOnly)) {
        QString style = QLatin1String(styleFile.readAll());
        app.setStyleSheet(style);
        styleFile.close();
    }
    
    // Initialize services
    auto userService = std::make_unique<UserService>();
    auto wordService = std::make_unique<WordService>();
    auto reviewService = std::make_unique<ReviewService>();
    auto statisticsService = std::make_unique<StatisticsService>();
    
    // Create and show login view
    LoginView loginView(userService.get());
    loginView.show();
    
    return app.exec();
}
