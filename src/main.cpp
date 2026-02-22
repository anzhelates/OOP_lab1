#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h"
#include <QApplication>
#include "GUI/MainWindow.h"

int main(int argc, char** argv) {
    QApplication a(argc, argv);

    doctest::Context context;
    context.applyCommandLine(argc, argv);
    int testResult = context.run();
    if (context.shouldExit()) {
        return testResult;
    }

    MainWindow window;
    window.show();
    return a.exec();
}