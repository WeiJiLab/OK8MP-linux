#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDate>
#include <QTime>
#include <QPalette>
#include <stdio.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QPalette pal;
    pal.setColor(QPalette::Text,QColor(255,0,0));

    ui->year->setPalette(pal);
    ui->month->setPalette(pal);
    ui->day->setPalette(pal);
    ui->hour->setPalette(pal);
    ui->minute->setPalette(pal);
    ui->second->setPalette(pal);

    QDate d = QDate::currentDate();
    QTime t = QTime::currentTime();

    ui->year->setValue(d.year());
    ui->month->setValue(d.month());
    ui->day->setValue(d.day());
    ui->hour->setValue(t.hour());
    ui->minute->setValue(t.minute());
    ui->second->setValue(t.second());

    ui->year->setEnabled(false);
    ui->month->setEnabled(false);
    ui->day->setEnabled(false);
    ui->hour->setEnabled(false);
    ui->minute->setEnabled(false);
    ui->second->setEnabled(false);



    timer.start(1000,this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::timerEvent(QTimerEvent *event)
{
    QDate d = QDate::currentDate();
    QTime t = QTime::currentTime();

    if(ui->set->text()=="set")
    {
        ui->year->setValue(d.year());
        ui->month->setValue(d.month());
        ui->day->setValue(d.day());
        ui->hour->setValue(t.hour());
        ui->minute->setValue(t.minute());
        ui->second->setValue(t.second());
    }

    QWidget::timerEvent(event);
}

void MainWindow::on_set_clicked()
{
    if(ui->set->text()=="set")
    {
        ui->set->setText("save");
        ui->year->setEnabled(true);
        ui->month->setEnabled(true);
        ui->day->setEnabled(true);
        ui->hour->setEnabled(true);
        ui->minute->setEnabled(true);
        ui->second->setEnabled(true);

        timer.stop();

    }
    else
    {
        char buf[128];
        ui->set->setText("set");

        memset(buf,0,128);

        sprintf(buf,"date -s '%4d-%02d-%02d %02d:%02d:%02d'",ui->year->text().toInt(),ui->month->text().toInt(),ui->day->text().toInt(),
                ui->hour->text().toInt(),ui->minute->text().toInt(),ui->second->text().toInt());

        system(buf);
        system("hwclock -w");

        ui->year->setEnabled(false);
        ui->month->setEnabled(false);
        ui->day->setEnabled(false);
        ui->hour->setEnabled(false);
        ui->minute->setEnabled(false);
        ui->second->setEnabled(false);

        timer.start(1000,this);
    }
}



void MainWindow::closeEvent(QCloseEvent *)
{
    //system("killall matrix_gui");
    //system("/etc/init.d/qt.sh");
    exit(0);
}
