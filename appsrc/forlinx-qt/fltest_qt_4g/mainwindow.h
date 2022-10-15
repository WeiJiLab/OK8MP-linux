#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private slots:

    void result();
    void ping_result();
    void on_connect_clicked();
    void on_ping_clicked();

private:
    Ui::MainWindow *ui;
    QProcess *myprocess;
    QProcess *ping_process;
};

#endif // MAINWINDOW_H
