#pragma once

#include <QObject>
#include <QMenu>
#include <QTreeView>
#include <QModelIndex>

class QAbstractItemModel;

class QtTreeContextMenu : public QObject
{
    Q_OBJECT

public:
    explicit QtTreeContextMenu(QTreeView *treeView, QAbstractItemModel *model, QObject *parent = nullptr);
    ~QtTreeContextMenu();

    void show(const QPoint &pos);

signals:
    void copyIdRequested(const QString &id);
    void copyNameRequested(const QString &name);
    void refreshRequested();

private slots:
    void onCopyIdTriggered();
    void onCopyNameTriggered();
    void onExpandAllTriggered();
    void onCollapseAllTriggered();
    void onRefreshTriggered();

private:
    void setupMenu();
    QModelIndex getCurrentIndex() const;

    QMenu *m_menu = nullptr;
    QTreeView *m_treeView = nullptr;
    QAbstractItemModel *m_model = nullptr;
};
