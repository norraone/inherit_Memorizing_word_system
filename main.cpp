#include <QApplication>
#include <QMainWindow>
#include <QMessageBox>
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    try {
        MainWindow mainWindow;
        mainWindow.show();
        return app.exec();
    } catch (const std::exception& e) {
        QMessageBox::critical(nullptr, "Error", 
            QString("An error occurred: %1").arg(e.what()));
        return 1;
    }
}
