#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/watchdog.h>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    fd = 0;
    times = 0;
}

MainWindow::~MainWindow()
{
    delete ui;
    ::close(fd);
}

void MainWindow::timerEvent(QTimerEvent *)
{
    int dummy;
    QString str;

    times++;

    if(ui->checkBox->isChecked())
        ::ioctl(fd,WDIOC_KEEPALIVE,&dummy);

    ui->times->setText(str.sprintf("%d",times));
}

void MainWindow::on_pushButton_clicked()
{
	int flags;
    if(ui->pushButton->text() == QString("open watchdog"))
    {
        ui->pushButton->setText("close watchdog");
        ui->checkBox->setDisabled(true);

        fd = ::open("/dev/watchdog",O_WRONLY);
        if(fd < 0)
        {
            QMessageBox::about(this,"error","open watchdog failure");
            exit(-1);
        }
        flags = 10;
        ioctl(fd,WDIOC_SETTIMEOUT,&flags);
        times = 0;
        ui->times->setText("0");
        timer.start(1000,this);
    }
    else
    {
        ui->pushButton->setText("open watchdog");
        ui->checkBox->setEnabled(true);
		
		flags = WDIOS_DISABLECARD;
	    ioctl(fd, WDIOC_SETOPTIONS, &flags);
        
		timer.stop();
        ::close(fd);
    }
}


void MainWindow::closeEvent(QCloseEvent *)
{
    exit(0);
}

