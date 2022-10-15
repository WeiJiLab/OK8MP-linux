#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QBasicTimer>
#include <QTimerEvent>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
protected:
   void timerEvent(QTimerEvent *event);
   void closeEvent(QCloseEvent *);
private slots:
    void on_set_clicked();

private:
    Ui::MainWindow *ui;
    QBasicTimer timer;
};

#endif // MAINWINDOW_H
