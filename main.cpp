#include <QApplication>
#include <QMessageBox>
#include "mainwindow.h"
#include "database.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    try {
        // 初始化数据库
        Database::initialize();
        
        // 设置应用程序信息
        app.setApplicationName("单词记忆系统");
        app.setApplicationVersion("1.0.0");
        
        // 设置应用程序样式
        app.setStyle("Fusion");
        
        // 创建并显示主窗口
        MainWindow mainWindow;
        mainWindow.show();
        
        return app.exec();
    } catch (const std::exception& e) {
        QMessageBox::critical(nullptr, "错误", 
            QString("程序启动失败: %1").arg(e.what()));
        return 1;
    }
}
