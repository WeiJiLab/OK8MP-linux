#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QFile>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->hostip->setText("192.168.1.111");

    myprocess = new QProcess(this);
    connect(myprocess, SIGNAL(readyReadStandardOutput()),this, SLOT(result()));
    connect(myprocess, SIGNAL(readyReadStandardError()),this, SLOT(result()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::result()
{
    QString abc = myprocess->readAllStandardOutput();
    ui->result->append(abc.trimmed());
    QString efg = myprocess->readAllStandardError();
    if(efg.length()>1)ui->result->append(efg.trimmed());
}



void MainWindow::closeEvent(QCloseEvent *)
{
    myprocess->kill();
    destroy();
    exit(0);
}


void MainWindow::on_connect_clicked()
{
    if(ui->ssid->text() == QString(""))
    {
        QMessageBox::about(this,"error","ssid cannot be empty!");
        return;
    }
    if(ui->pawd->text() == QString(""))
    {
        myprocess->start(tr("/usr/bin/fltest_wifi.sh -i %1 -s %2 -p %3")
                        .arg(ui->comboBox->currentText())
                        .arg(ui->ssid->text())
                        .arg("NONE"));
    }
    else
    {
        myprocess->start(tr("/usr/bin/fltest_wifi.sh -i %1 -s %2 -p %3")
                        .arg(ui->comboBox->currentText())
                        .arg(ui->ssid->text())
                        .arg(ui->pawd->text()));
    }
}

void MainWindow::on_status_clicked()
{
    ui->result->clear();
    myprocess->start(tr("wpa_cli -i%1 status").arg(ui->comboBox->currentText()));
}

void MainWindow::on_ping_clicked()
{
    ui->result->clear();
    if(ui->hostip->text() == QString(""))
    {
        QMessageBox::about(this,"error","hostname cannot be empty!");
        return;
    }

    myprocess->start(tr("ping %1 -w 5").arg(ui->hostip->text()));
    ui->result->append(QString("ping ")+ui->hostip->text());
}
