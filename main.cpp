#include "mainwindow.h"
#include <QApplication>
#include <iostream>
#include "FTPClient.h"
#include <thread>
#include <chrono>
using namespace std::chrono_literals;


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
