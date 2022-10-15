#include <QApplication>
#include "keytest.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Keytest w;
    w.setWindowFlags(w.windowFlags()& ~Qt::WindowMaximizeButtonHint& ~Qt::WindowMinimizeButtonHint);
    w.showMaximized();
    w.show();
    return a.exec();
}
