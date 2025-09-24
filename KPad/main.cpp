#include "kpad.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    // Create a QApplication object:
    QApplication a(argc, argv);
    // Create a Kpad object:
    Kpad w;
    // Widgets are not visible by default. Use show()
    w.show();
    // Enter the event loop
    return a.exec();
}
