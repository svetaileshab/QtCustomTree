#pragma once

#include <QWidget>
#include <QTreeView>
#include <QPersistentModelIndex>
#include <QList>
#include <memory>

class QAbstractItemModel;
class ITreeDataSource;
class QtTreeModel;
class QtTreeSelection;
class QtTreeDragHandler;
class QtTreeStatusManager;
class QtTreeContextMenu;

class QtCustomTree : public QWidget
{
    Q_OBJECT

public:
    explicit QtCustomTree(QWidget *parent = nullptr);
    ~QtCustomTree();

    void setDataSource(ITreeDataSource *source);
    void rebuild();
    void clear();
    void setMaxSelection(int max);

    QtTreeSelection* selection() const { return m_selection; }
    QTreeView* treeView() const { return m_treeView; }

    QList<QString> getSelectedIds() const;
    void selectItemById(const QString &id);

signals:
    void itemDoubleClicked(const QString &id);
    void selectionChanged();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    void setupUi();
    void setupConnections();
    bool isItemIndex(const QModelIndex &idx) const;

    QTreeView *m_treeView = nullptr;
    QtTreeModel *m_model = nullptr;
    QtTreeSelection *m_selection = nullptr;
    QtTreeDragHandler *m_dragHandler = nullptr;
    QtTreeStatusManager *m_statusManager = nullptr;
    QtTreeContextMenu *m_contextMenu = nullptr;
};
