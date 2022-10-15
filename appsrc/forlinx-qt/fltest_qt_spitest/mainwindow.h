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
    
private slots:
    void on_send_clicked();

    void on_loop_clicked(bool checked);

    void on_clockphase_clicked(bool checked);

    void on_clockpolarity_clicked(bool checked);

    void on_lsb_clicked(bool checked);

    void on_cshigh_clicked(bool checked);

    void on_wire_clicked(bool checked);

    void on_nocs_clicked(bool checked);

    void on_ready_clicked(bool checked);

    void update_params();

    void on_speed_textChanged(const QString &arg1);

    void on_delay_textChanged(const QString &arg1);

    void on_bits_textChanged(const QString &arg1);
protected:
    void moveEvent(QMoveEvent *);
    void resizeEvent(QResizeEvent *);
    void closeEvent(QCloseEvent *);
private:
    Ui::MainWindow *ui;
    int fd;
};

#endif // MAINWINDOW_H
