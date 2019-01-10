#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <QMessageBox>
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
    QObject::connect(this->ui->currentFolderEdit, SIGNAL(returnPressed()),this,SLOT(onChangeDirectory()));

}

void MainWindow::onLoginButtonClicked()
{
    try {
    ftp.Connect(address, port);
    ftp.Login(login, password);
    onUpdateCurrentFolder();
    onGetDirectories();
    ui->logout_button->setEnabled(1);
    ui->tree_button->setEnabled(1);
    }
    catch(std::runtime_error &er) {
        QMessageBox msgBox;
        msgBox.setText(er.what());
        msgBox.exec();
    }


}

void MainWindow::onLogoutButtonClicked()
{
    try {
        ftp.Quit();
    }
    catch(std::runtime_error& er) {
        QMessageBox msgBox;
        msgBox.setText(er.what());
        msgBox.exec();
    }
    ui->currentFolderEdit->setText("");
    ui->listWidget->clear();
    ui->login_button->setEnabled(1);
    ui->logout_button->setEnabled(0);
    ui->tree_button->setEnabled(0);


}

void MainWindow::onUpdateCurrentFolder()
{
    try {
    auto pwd = ftp.Pwd();
    auto pwd_vec = ftp.Split(pwd,"\"");
    currentDir = QString::fromStdString(pwd_vec.at(1));
    ui->currentFolderEdit->setText(currentDir);
    }
    catch(std::runtime_error& er)
    {
        QMessageBox msgBox;
        msgBox.setText(er.what());
        msgBox.exec();
    }
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
        QMessageBox msgBox;
        msgBox.setText(er.what());
        msgBox.exec();
    }
}

void MainWindow::onGetDirectories()
{
    QStringList qlist;
    try {
    auto dirs = ftp.Nlst(currentDir.toStdString());
    for(auto file : dirs)
    {
        auto split = ftp.Split(file,"/");
        qlist.push_back(QString::fromStdString("/"+split.back()));
    }
    qlist.push_front("..");
    ui->listWidget->clear();
    ui->listWidget->addItems(qlist);
    }
    catch(std::runtime_error &er)
    {
        QMessageBox msgBox;
        msgBox.setText(er.what());
        msgBox.exec();
    }
}

void MainWindow::onChangeDirectory()
{
    try {
        if(ui->currentFolderEdit->text() == "..")
            ftp.Cdup();
        else {
            auto cd = ui->currentFolderEdit->text();
            ftp.Cwd(cd.toStdString());
        }
        onUpdateCurrentFolder();
        onGetDirectories();
    }
    catch(std::runtime_error& er) {

    }
}

void MainWindow::onDirectoryChanged(QListWidgetItem *dir)
{

    try {
        if(dir->text() == "..")
            ftp.Cdup();
        else {
            auto cd = currentDir + dir->text();
            ftp.Cwd(cd.toStdString());
        }
        onUpdateCurrentFolder();
        onGetDirectories();
    }
    catch(std::runtime_error& er) {

    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
