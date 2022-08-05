#include "mainwindow.h"
#include <QLabel>
#include <QMenuBar>
#include <QMenu>
#include <QAction>

MainWindow::MainWindow(QWidget * parent) : QMainWindow(parent)
{
    this->setWindowTitle("Mesh Inspector");
}
