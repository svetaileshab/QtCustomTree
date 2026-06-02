#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QFrame>
#include <QDebug>
#include "QtCustomTree/QtCustomTree.h"
#include "FakeDataSource.h"
#include "FakeFileDataSource.h"
#include "FakeOrgDataSource.h"
#include "FakeGeoDataSource.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    QMainWindow window;
    window.setWindowTitle("QtCustomTree Demo");
    window.resize(500, 700);
    
    QWidget *central = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(central);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    
    QLabel *label = new QLabel("Select test data source:");
    label->setStyleSheet(
        "QLabel {"
        "  color: #10b981;"
        "  padding: 12px 16px 4px 16px;"
        "  font-size: 16px;"
        "  font-weight: bold;"
        "  background: #0f172a;"
        "}"
    );
    layout->addWidget(label);
    
    QComboBox *combo = new QComboBox();
    combo->addItem("🏭 Devices & Sensors");
    combo->addItem("📁 File System");
    combo->addItem("👥 Organization Structure");
    combo->addItem("🌍 Geo Locations");
    combo->setStyleSheet(
        "QComboBox {"
        "  background: #1e293b;"
        "  color: #f1f5f9;"
        "  border: none;"
        "  padding: 10px 16px;"
        "  font-size: 14px;"
        "  margin: 0 8px 8px 8px;"
        "  border-radius: 6px;"
        "}"
        "QComboBox:hover {"
        "  background: #334155;"
        "}"
        "QComboBox::drop-down {"
        "  border: none;"
        "  width: 30px;"
        "  padding-right: 8px;"
        "}"
        "QComboBox::down-arrow {"
        "  image: url(:/icons/arrow-down);"
        "  width: 20px;"
        "  height: 20px;"
        "}"
        "QComboBox QAbstractItemView {"
        "  background: #1e293b;"
        "  color: #f1f5f9;"
        "  selection-background-color: #3b82f6;"
        "  border: 1px solid #334155;"
        "  border-radius: 4px;"
        "  padding: 4px;"
        "  outline: none;"
        "}"
        );

    layout->addWidget(combo);
    
    QFrame *separator = new QFrame();
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Plain);
    separator->setLineWidth(20);
    separator->setStyleSheet(
        "QFrame {"
        "  color: #10b981;"
        "  margin: 4px 8px;"
        "}"
        );
    layout->addWidget(separator);
    
    QtCustomTree *tree = new QtCustomTree();
    layout->addWidget(tree);
    
    FakeDataSource *devicesSource = new FakeDataSource();
    FakeFileDataSource *fileSource = new FakeFileDataSource();
    FakeOrgDataSource *orgSource = new FakeOrgDataSource();
    FakeGeoDataSource *geoSource = new FakeGeoDataSource();
    
    tree->setDataSource(devicesSource);
    tree->rebuild();
    
    QObject::connect(combo, QOverload<int>::of(&QComboBox::currentIndexChanged), 
        [&](int index) {
            switch (index) {
                case 0: tree->setDataSource(devicesSource); break;
                case 1: tree->setDataSource(fileSource); break;
                case 2: tree->setDataSource(orgSource); break;
                case 3: tree->setDataSource(geoSource); break;
            }
            tree->rebuild();
        });

    window.setCentralWidget(central);
    window.setStyleSheet("QMainWindow { background: #0f172a; }");
    window.show();
    
    return app.exec();
}
