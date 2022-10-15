#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include "interface.h"
#include <QList>
#include <QProcess>
#include <QCloseEvent>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private:
    Ui::MainWindow *ui;
    QList<Interface*> ints;
    bool flag;
    QProcess *myprocess;

public slots:
    void on_sel_changed(const QString &text);
    void on_toggled(bool b);
    void on_ok_clicked();
    void refreshInterfaces();
    void setConfigs();
    void state(bool dhcp);

protected:
    void closeEvent(QCloseEvent * evt);

};

#endif // MAINWINDOW_H
