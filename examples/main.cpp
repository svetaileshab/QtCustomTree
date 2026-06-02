#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QFile>
#include <QDebug>
#include "QtCustomTree/QtCustomTree.h"
#include "FakeDataSource.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    qDebug() << "Arrow right exists:" << QFile::exists(":/icons/arrow-right");
    qDebug() << "Arrow down exists:" << QFile::exists(":/icons/arrow-down");
    
    QMainWindow window;
    window.setWindowTitle("QtCustomTree Demo");
    window.resize(450, 650);
    
    QtCustomTree *tree = new QtCustomTree();
    FakeDataSource *dataSource = new FakeDataSource();
    tree->setDataSource(dataSource);
    tree->rebuild();
    
    window.setCentralWidget(tree);
    window.setStyleSheet("QMainWindow { background: #0f172a; }");
    window.show();
    
    return app.exec();
}
