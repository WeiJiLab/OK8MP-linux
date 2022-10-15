#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
//    ui->hostname->setText("192.168.1.1");
    myprocess = NULL;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_ping_clicked()
{
    if(ui->hostname->text() == QString(""))
    {
        QMessageBox::about(this,"error","hostname cannot be empty!");
        return;
    }

    if(myprocess)
        delete myprocess;

    myprocess = new QProcess(this);
    connect(myprocess, SIGNAL(readyReadStandardOutput()),this, SLOT(result()));
    connect(myprocess, SIGNAL(readyReadStandardError()),this, SLOT(result()));
    myprocess->start(QString("ping ")+ui->hostname->text());
    ui->result->append(QString("ping ")+ui->hostname->text());
    //myprocess->waitForFinished();
}

void MainWindow::result()
{
    QString abc = myprocess->readAllStandardOutput();
    ui->result->append(abc.trimmed());
    QString efg = myprocess->readAllStandardError();
    if(efg.length()>1)ui->result->append(efg.trimmed());
}

void MainWindow::on_stop_clicked()
{
    delete myprocess;
    myprocess = NULL;
}

void MainWindow::on_clear_clicked()
{
    ui->result->clear();
}

void MainWindow::closeEvent(QCloseEvent *)
{
    myprocess->kill();
    destroy();
    exit(0);
}




