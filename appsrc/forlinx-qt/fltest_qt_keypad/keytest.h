#ifndef KEYTEST_H
#define KEYTEST_H

#include <QWidget>

#define  KEYMAXNUM  2

class QSocketNotifier; //need this class

namespace Ui
{
class Keytest;
}

#define KEY_V_UP	103
#define KEY_V_DOWN      108

class Keytest : public QWidget
{
    Q_OBJECT
    
public:
    explicit Keytest(QWidget *parent = 0);
    ~Keytest();
     QWidget *parent;

protected:
  void paintEvent(QPaintEvent* e);


private:
    Ui::Keytest *ui;
    volatile int m_fd;
    QSocketNotifier* m_notifyObject;
    bool m_oldButtonsState[KEYMAXNUM];

private slots:
  void keyEvent();

};

#endif // KEYTEST_H
