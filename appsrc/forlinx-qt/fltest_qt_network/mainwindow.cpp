/**
 ** Copyright Forlinx BeiJing ltd.
 ** Autohor: duyahui
 ** Email: duyahui@forlinx.com
 **/

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "stdio.h"
#include "string.h"

// ifconfig -a|grep -E "eth[0-9]|wlan[0-9]"|cut -d' ' -f 1
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    flag = false;
    connect(ui->cb_interface, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(on_sel_changed(QString)));
    //connect(ui->ok, SIGNAL(clicked()), this, SLOT(on_ok_clicked()));
    connect(ui->radio_dhcp, SIGNAL(toggled(bool)), this, SLOT(on_toggled(bool)));
    connect(ui->radio_static, SIGNAL(toggled(bool)), this, SLOT(on_toggled(bool)));

    refreshInterfaces();
    setConfigs();

}

MainWindow::~MainWindow()
{
    delete ui;
    foreach(Interface *i,ints)
        delete i;
}

void MainWindow::state(bool dhcp)
{
    if(dhcp)
    {
        ui->radio_dhcp->setChecked(true);
        ui->radio_static->setChecked(false);
        ui->edt_ip->setDisabled(true);
        ui->edt_mask->setDisabled(true);
        ui->edt_gateway->setDisabled(true);
        ui->edt_dns->setDisabled(true);
    }
    else
    {
        ui->radio_dhcp->setChecked(false);
        ui->radio_static->setChecked(true);
        ui->edt_ip->setDisabled(false);
        ui->edt_mask->setDisabled(false);
        ui->edt_gateway->setDisabled(false);
        ui->edt_dns->setDisabled(false);
    }
}

void MainWindow::refreshInterfaces()
{
    QStringList sl;

    /*过滤读eth[0-9] wlan[0-9]*/
    ::system("ifconfig -a|grep -E \"eth[0-9]\"|cut -d' ' -f 1 >/tmp/interfaces");
    QFile f("/tmp/interfaces");
    if(f.open(QFile::ReadOnly))
    {
        QTextStream t(&f);
        while(!t.atEnd())
        {
            QString str=t.readLine();
            if(str.size()>0)
            {
                //QMessageBox::about(this,"aaa",str);
                ints.append(new Interface(str));
                sl<<str;
            }
        }
    }
    f.close();

    ui->cb_interface->addItems(sl);
}


void MainWindow::setConfigs()
{
    foreach(Interface *i,ints)
    {
                i->dhcp = false;
                i->ip = QString("192.168.1.1");
                i->mask = QString("255.255.255.0");
                i->gateway = QString("192.168.1.1");
                i->dns = QString("192.168.1.1");
    }
}
unsigned int ip2int(QString ip)
{
    QStringList sl = ip.split('.');
    unsigned int r = 0;
    foreach(QString s,sl)
    {
        r <<= 8;
        r |= s.toUInt();
    }

    return r;
}

QString int2ip(unsigned int ip)
{
    return QString::number((ip >> 24) & 0xff) + "." + QString::number((ip >> 16) & 0xff)
            + "." + QString::number((ip >> 8) & 0xff) + "." + QString::number(ip & 0xff);
}


void MainWindow::on_toggled(bool b)
{
    Interface *i = NULL;
    foreach(i,ints)
    {
        if(i->name == ui->cb_interface->currentText())
            break;
    }

    state(ui->radio_dhcp->isChecked());

    if(ui->radio_dhcp->isChecked())
    {
        /*ui->edt_ip->clear();
        ui->edt_mask->clear();
        ui->edt_gateway->clear();
        ui->edt_dns->clear();*/
    }
    else
    {
        ui->edt_ip->setText(i->ip);
        ui->edt_mask->setText(i->mask);
        ui->edt_gateway->setText(i->gateway);
        ui->edt_dns->setText(i->dns);
    }
}

void MainWindow::on_sel_changed(const QString &text)
{
    Interface *i = NULL;
    foreach(i,ints)
    {
        if(i->name == text)
            break;
    }

    state(i->dhcp);

    if(i->dhcp)
    {
        ui->edt_ip->clear();
        ui->edt_mask->clear();
        ui->edt_gateway->clear();
        ui->edt_dns->clear();
    }
    else
    {
        ui->edt_ip->setText(i->ip);
        ui->edt_mask->setText(i->mask);
        ui->edt_gateway->setText(i->gateway);
        ui->edt_dns->setText(i->dns);
    }
}

void MainWindow::on_ok_clicked()
{
    int mask[4],j;
    int mask_num,count;
    int mask_comp[9] = {0,128,192,224,240,248,252,254,255};
    Interface *i = NULL;
    bool is_connected=true;
    foreach(i,ints)
    {
        if(i->name == ui->cb_interface->currentText())
            break;
    }

    i->dhcp = ui->radio_dhcp->isChecked();
    i->ip = ui->edt_ip->text();
    i->mask = ui->edt_mask->text();
    i->gateway = ui->edt_gateway->text();
    i->dns = ui->edt_dns->text();

//  writeConfigs();
    ::system("rm /etc/systemd/network/10-eth.network");

    QFile rf("/etc/systemd/network/10-eth.network");//for dns

        if(!rf.exists()){
           // printf("create /etc/resolv.conf\n");
            ::system("touch /etc/systemd/network/10-eth.network");
        }else{
            ::system("echo > /etc/systemd/network/10-eth.network");
           // printf("/etc/resolv.conf exists\n");
        }
     QTextStream *t2 = NULL;

     if(rf.open(QFile::ReadWrite))
     {
         t2 = new QTextStream(&rf);

     }else{
         printf("open 10-eth.network failed!");
     }
    *t2<<QString("[Match]\n");
    *t2<<QString("Name=")<<i->name<<QString("\n");
    *t2<<QString("KernelCommandLine=!root=/dev/nfs\n\n");
    *t2<<QString("[Network]\n");
    QString cm=NULL;
    QString cm2=NULL;
    flag = true;

    this->setDisabled(true);

    if(i->dhcp)
    {
        cm="/sbin/udhcpc -i "+i->name;
        myprocess = NULL;
        if(myprocess)
            delete myprocess;
        myprocess = new QProcess(this);
        myprocess->start(QString(cm));
        is_connected = myprocess->waitForFinished(8000);
        delete myprocess;
    	*t2<<QString("DHCP=yes\n");
	}
    else
    {
        mask[0] = atoi(i->mask.toLatin1().data());
        mask[1] = atoi(strchr(i->mask.toLatin1().data(),'.') + 1);
        mask[2] = atoi(strchr(strchr(i->mask.toLatin1().data() + 1,'.'),'.') + 1);
        mask[3] = atoi(strrchr(i->mask.toLatin1().data(),'.') + 1);
        for(mask_num = 0;mask_num < 4;mask_num++)
        {
           if(mask[mask_num] == 255)
           {
               continue;
           }
           else
           {
                for(j = 0;j < 9;j++)
                {
                    if(mask[mask_num] == mask_comp[j])
                        break;
                }
                break;
           }
        }
        count = mask_num * 8 + j;
        *t2<<QString("Address=")<<i->ip<<QString("/")<<count<<QString("\n");
        *t2<<QString("Gateway=")<<i->gateway<<QString("\n");
        *t2<<QString("DNS=")<<i->dns<<QString("\n");

        cm="ifconfig "+i->name+" "+i->ip+" netmask "+i->mask;
        cm2="route add default gateway "+i->gateway+" "+i->name;
        ::system(cm.toLatin1().data());
        ::system(cm2.toLatin1().data());
    }
    delete t2;
    rf.close();
    this->setDisabled(false);
    flag = false;
    if(is_connected)
        QMessageBox::about(this,"info","network restart ok!");
    else
        QMessageBox::about(this,"info","fail! please check the network");

}

void MainWindow::closeEvent(QCloseEvent * evt)
{
    if(flag)
    {
        evt->ignore();
        QMessageBox::about(this,"info","network is restarting,please wait");
    }
    else
    {
        destroy();
        exit(0);
    }
}






