#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QFile>
#include <unistd.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->hostip->setText("www.forlinx.com");

    myprocess = new QProcess(this);
    connect(myprocess, SIGNAL(readyReadStandardOutput()),this, SLOT(result()));
    connect(myprocess, SIGNAL(readyReadStandardError()),this, SLOT(result()));

    ping_process = new QProcess(this);
    connect(ping_process, SIGNAL(readyReadStandardOutput()),this, SLOT(ping_result()));
    connect(ping_process, SIGNAL(readyReadStandardError()),this, SLOT(ping_result()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::result()
{
    QString abc = myprocess->readAllStandardOutput();
    if(abc.length()>1)ui->result->append(abc.trimmed());
    QString efg = myprocess->readAllStandardError();
    if(efg.length()>1)ui->result->append(efg.trimmed());
}

void MainWindow::ping_result()
{
    QString abc = ping_process->readAllStandardOutput();
    if(abc.length()>1)ui->result->append(abc.trimmed());
    QString efg = ping_process->readAllStandardError();
    if(efg.length()>1)ui->result->append(efg.trimmed());
}

void MainWindow::on_connect_clicked()
{
    if (ui->comboBox->currentText() == "ME909"){
	if (myprocess->state() == QProcess::NotRunning)
        	myprocess->start("/usr/bin/fltest_me909s.sh &");
		setWindowTitle("4G");
    }
    else if (ui->comboBox->currentText() == "EC20"){
    	if (myprocess->state() == QProcess::NotRunning)
        	myprocess->start("/usr/bin/fltest_ec20.sh &");
		setWindowTitle("4G");
    } else {
    	if (myprocess->state() == QProcess::NotRunning)
        	myprocess->start("/usr/bin/fltest_rm500.sh &");
		setWindowTitle("5G");
	}
}

void MainWindow::on_ping_clicked()
{
    ui->result->clear();
    if(ui->hostip->text() == QString(""))
    {
        QMessageBox::about(this,"error","hostname cannot be empty!");
        return;
    }

    if (ping_process->state() == QProcess::NotRunning){
	ping_process->start(QString("ping ")+ui->hostip->text());
    	ui->result->append(QString("ping ")+ui->hostip->text());
    }
}
