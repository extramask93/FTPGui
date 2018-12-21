#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <QSettings>
#include <QDir>

void MainWindow::readSettings()
{
    QSettings settings(QDir::current().filePath("config.ini"), QSettings::IniFormat);
    address = settings.value("ip","localhost").toString().toStdString();
    port = static_cast<uint16_t>(settings.value("port", 21).toUInt());
    login = settings.value("login").toString().toStdString();
    password =settings.value("password").toString().toStdString();
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    ftp()
{
    ui->setupUi(this);
    readSettings();
    QObject::connect(this->ui->login_button,SIGNAL(clicked()),this,SLOT(onLoginButtonClicked()));
    QObject::connect(this->ui->logout_button, SIGNAL(clicked()),this,SLOT(onLogoutButtonClicked()));
    QObject::connect(this->ui->tree_button, SIGNAL(clicked()),this,SLOT(onPrintTreeClicked()));
    QObject::connect(this->ui->listWidget, SIGNAL(itemPressed(QListWidgetItem*)),this,SLOT(onDirectoryChanged(QListWidgetItem *)));
}

void MainWindow::onLoginButtonClicked()
{
    try {
    ftp.Connect(address, port);
    ftp.Login(login, password);
    onUpdateCurrentFolder();
    onGetDirectories();
    }
    catch(std::runtime_error &er) {
        std::cerr<<er.what();
    }
    ui->logout_button->setEnabled(1);
    ui->tree_button->setEnabled(1);

}

void MainWindow::onLogoutButtonClicked()
{
    try {
        ftp.Quit();
    }
    catch(std::runtime_error& er) {
        std::cerr<<er.what();
    }
    ui->label_2->setText("");
    ui->listWidget->clear();
    ui->login_button->setEnabled(1);
    ui->logout_button->setEnabled(0);
    ui->tree_button->setEnabled(0);


}

void MainWindow::onUpdateCurrentFolder()
{
    auto pwd = ftp.Pwd();
    auto pwd_vec = ftp.Split(pwd,"\"");
    currentDir = QString::fromStdString(pwd_vec.at(1));
    ui->label_2->setText(currentDir);
}

void MainWindow::onPrintTreeClicked()
{
    std::list<std::string> list;
    QStringList qlist;
    try {
        ftp.ExploreRecursively(currentDir.toStdString(),list);
        ui->listWidget->clear();
        ui->listWidget->addItem("..");
        for(const auto& it: list) {
            qlist.push_back(QString::fromStdString(it));
        }
        ui->listWidget->addItems(qlist);
    }
    catch(std::runtime_error& er) {
        std::cerr<<er.what();
    }
}

void MainWindow::onGetDirectories()
{
    QStringList qlist;
    auto dirs = ftp.Nlst(currentDir.toStdString());
    qlist.push_back("..");
    for(const auto& it: dirs) {
        qlist.push_back(QString::fromStdString(it));
    }
    ui->listWidget->clear();
    ui->listWidget->addItems(qlist);
}

void MainWindow::onDirectoryChanged(QListWidgetItem *dir)
{

    try {
        if(dir->text() == "..")
            ftp.Cdup();
        else
            ftp.Cwd(dir->text().toStdString());
        onUpdateCurrentFolder();
        onGetDirectories();
    }
    catch(std::runtime_error& ex) {
        std::cerr<<ex.what();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
