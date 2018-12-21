#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <stdexcept>
/*#include "boost/program_options.hpp"


namespace po = boost::program_options;*/
/*
void options_required(const po::variables_map& vm, const std::vector<std::string>& required_options)
{
    for (const auto& item : required_options) {
        if (vm.count(item) == 0)
            throw std::logic_error(std::string("Option '") + item
                + "' is required.");
    }
}
po::variables_map TryParseCommandLineArguments(int argc, char **argv)
{
    std::ifstream configFile{ "config.ini",std::ifstream::in };
    po::options_description configFileOptions("Allowed options");
    configFileOptions.add_options()
        ("ip", po::value<std::string>(), "POP3 server address")
        ("port", po::value<uint16_t>(), "POP3 server port")
        ("login,l", po::value<std::string>(), "Email login")
        ("pass,p", po::value<std::string>(), "Email password")
        ("delay", po::value<unsigned>(), "Delay in seconds between subsequent email checks")
        ("help,h", "Produce help message")
        ("version,v", "Prints version number")
        ;
    po::variables_map vm;
    auto a = *argv;
    try {
        po::store(po::parse_command_line(argc, argv, configFileOptions), vm);
        po::store(po::parse_config_file(configFile, configFileOptions), vm);
        options_required(vm, { "ip","port","login","pass","delay", });
    }
    catch (const std::logic_error &ex)
    {
        std::cerr << ex.what() << '\n';
        std::cout << configFileOptions << "\n";
        exit(1);
    }
    catch (std::exception)
    {
        std::cerr << "Error: Unrecognized Option!\n";
        exit(2);
    }
    po::notify(vm);
    if (vm.count("help")) {
        std::cout << configFileOptions << "\n";
        exit(0);
    }
    if (vm.count("version"))
    {
        std::cout << "v.1.0.0\n";
        exit(0);
    }
    return vm;
}
*/
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    ftp()
{
    ui->setupUi(this);
    QObject::connect(this->ui->login_button,SIGNAL(clicked()),this,SLOT(onLoginButtonClicked()));
    QObject::connect(this->ui->logout_button, SIGNAL(clicked()),this,SLOT(onLogoutButtonClicked()));
    QObject::connect(this->ui->tree_button, SIGNAL(clicked()),this,SLOT(onPrintTreeClicked()));
    QObject::connect(this->ui->listWidget, SIGNAL(itemPressed(QListWidgetItem*)),this,SLOT(onDirectoryChanged(QListWidgetItem *)));
}

void MainWindow::onLoginButtonClicked()
{
    try {
    ftp.Connect(ADDR, PORT);
    ftp.Login(USR, PASS);
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
