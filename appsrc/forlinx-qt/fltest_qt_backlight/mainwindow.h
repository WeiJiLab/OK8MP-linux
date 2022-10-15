#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    int lvds_backlight = 1;
    
private:
    Ui::MainWindow *ui;
    int fd;

private slots:
    void sliderchanged(int v);

private slots:
    void on_switch_backlight_clicked();

};

#endif // MAINWINDOW_H
