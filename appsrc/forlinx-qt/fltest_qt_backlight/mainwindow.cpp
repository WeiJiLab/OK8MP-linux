#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    fd = ::open("/sys/class/backlight/dsi_backlight/brightness", O_RDWR|O_NONBLOCK);
	ui->switch_backlight->setText("dsi_backlight");
	lvds_backlight = 1;
	
    ui->slider->setValue(100);
	
	ui->switch_backlight->setDefault(true);
    connect(ui->switch_backlight, SIGNAL(pressed(bool)), this, SLOT(on_switch_backlight_clicked()));
    connect(ui->slider,SIGNAL(valueChanged(int)),this,SLOT(sliderchanged(int)));

}

MainWindow::~MainWindow()
{
    delete ui;
    if (fd > 0)
        ::close(fd);
}

void MainWindow::sliderchanged(int v)
{
    char buf[20];
	if (v < 1)
		v = 1;
    sprintf(buf,"%d",v);
    ::write(fd,buf,sizeof(buf));
}

void MainWindow::on_switch_backlight_clicked()
{
	if(lvds_backlight == 1)
	{
		ui->switch_backlight->setText("lvds_backlight");
		lvds_backlight = 0;
		if (fd > 0) {
			::close(fd);
			fd = -1;
		}
		fd = ::open("/sys/class/backlight/lvds_backlight/brightness",O_RDWR|O_NONBLOCK);
	}
	else if (lvds_backlight == 0)
	{
		ui->switch_backlight->setText("dsi_backlight");
		lvds_backlight = 1;
		if (fd > 0) {
			::close(fd);
			fd = -1;
		}
		fd = ::open("/sys/class/backlight/dsi_backlight/brightness",O_RDWR|O_NONBLOCK);
	}
}


