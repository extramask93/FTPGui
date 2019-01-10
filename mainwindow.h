#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include "FTPClient.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private slots:
    void onLoginButtonClicked();
    void onLogoutButtonClicked();
    void onUpdateCurrentFolder();
    void onPrintTreeClicked();
    void onGetDirectories();
    void onChangeDirectory();
    void onDirectoryChanged(QListWidgetItem *);

private:
    std::string login = "";
    std::string password = "";
    std::string address = "";
    uint16_t port;
    Ui::MainWindow *ui;
    QString currentDir;
    QStringList folders;
    FTPClient ftp;
    void readSettings();
};

#endif // MAINWINDOW_H
