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
    void on_ping_clicked();

    void on_stop_clicked();

    void on_clear_clicked();

    void result();

private:
    Ui::MainWindow *ui;
    QProcess *myprocess;
protected:
    void closeEvent(QCloseEvent *);


};

#endif // MAINWINDOW_H
