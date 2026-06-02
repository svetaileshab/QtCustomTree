#include "QtCustomTree/QtTreeContextMenu.h"

#include <QApplication>
#include <QClipboard>
#include <QAction>
#include <QAbstractItemModel>

QtTreeContextMenu::QtTreeContextMenu(QTreeView *treeView, QAbstractItemModel *model, QObject *parent)
    : QObject(parent), m_treeView(treeView), m_model(model)
{
    setupMenu();
}

QtTreeContextMenu::~QtTreeContextMenu()
{
    delete m_menu;
}

void QtTreeContextMenu::setupMenu()
{
    m_menu = new QMenu();
    m_menu->setStyleSheet(
        "QMenu { background: #0f172a; color: #f1f5f9; border: 1px solid #334155; border-radius: 8px; padding: 8px; }"
        "QMenu::item { padding: 8px 16px; border-radius: 6px; margin: 2px; }"
        "QMenu::item:selected { background: #3b82f6; }"
    );

    QAction *copyIdAction = m_menu->addAction("📋 Copy ID");
    connect(copyIdAction, &QAction::triggered, this, &QtTreeContextMenu::onCopyIdTriggered);

    QAction *copyNameAction = m_menu->addAction("📋 Copy Name");
    connect(copyNameAction, &QAction::triggered, this, &QtTreeContextMenu::onCopyNameTriggered);
    
    m_menu->addSeparator();

    QAction *expandAllAction = m_menu->addAction("📂 Expand All");
    connect(expandAllAction, &QAction::triggered, this, &QtTreeContextMenu::onExpandAllTriggered);

    QAction *collapseAllAction = m_menu->addAction("📁 Collapse All");
    connect(collapseAllAction, &QAction::triggered, this, &QtTreeContextMenu::onCollapseAllTriggered);
    
    m_menu->addSeparator();

    QAction *refreshAction = m_menu->addAction("🔄 Refresh");
    connect(refreshAction, &QAction::triggered, this, &QtTreeContextMenu::onRefreshTriggered);
}

void QtTreeContextMenu::show(const QPoint &pos)
{
    if (m_menu && m_treeView) {
        m_menu->exec(m_treeView->mapToGlobal(pos));
    }
}

QModelIndex QtTreeContextMenu::getCurrentIndex() const
{
    return m_treeView ? m_treeView->currentIndex() : QModelIndex();
}

void QtTreeContextMenu::onCopyIdTriggered()
{
    QModelIndex idx = getCurrentIndex();
    if (idx.isValid() && m_model) {
        QString id = m_model->data(idx, Qt::UserRole + 1).toString();
        if (!id.isEmpty()) {
            QApplication::clipboard()->setText(id);
            emit copyIdRequested(id);
        }
    }
}

void QtTreeContextMenu::onCopyNameTriggered()
{
    QModelIndex idx = getCurrentIndex();
    if (idx.isValid()) {
        QString name = m_model->data(idx, Qt::DisplayRole).toString();
        QApplication::clipboard()->setText(name);
        emit copyNameRequested(name);
    }
}

void QtTreeContextMenu::onExpandAllTriggered()
{
    if (m_treeView) m_treeView->expandAll();
}

void QtTreeContextMenu::onCollapseAllTriggered()
{
    if (m_treeView) m_treeView->collapseAll();
}

void QtTreeContextMenu::onRefreshTriggered()
{
    emit refreshRequested();
}
