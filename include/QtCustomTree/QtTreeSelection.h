#pragma once

#include <QObject>
#include <QTreeView>
#include <QModelIndex>
#include <QSet>
#include <QList>
#include <QPersistentModelIndex>
#include <memory>

class QAbstractItemModel;
class ITreeNodeData;

class QtTreeSelection : public QObject
{
    Q_OBJECT

public:
    explicit QtTreeSelection(QTreeView *treeView, QAbstractItemModel *model, QObject *parent = nullptr);
    ~QtTreeSelection();

    QList<QString> getSelectedIds() const;
    void addToSelection(const QString &id);
    void removeFromSelection(const QString &id);
    void clearAllSelections();

    void setAnchor(const QModelIndex &index);
    void clearAnchor();

    void navigateDownWithShift();
    void navigateUpWithShift();

    void handleSingleClick(const QModelIndex &index);
    void handleShiftClick(const QModelIndex &index);

    void setMaxSelection(int max) { m_maxSelection = max; }

signals:
    void selectionChanged();
    void limitReached();

private:
    void updateVisualSelection();
    int findVisualIndex(const QModelIndex &idx) const;
    QList<QModelIndex> getAllVisibleItems() const;
    bool isItemNode(const QModelIndex &index) const;
    QString getIdFromIndex(const QModelIndex &index) const;
    QModelIndex findIndexById(const QString &id) const;

    QTreeView *m_treeView;
    QAbstractItemModel *m_model;
    QSet<QString> m_selectedIds;
    QPersistentModelIndex m_anchorIndex;
    QString m_anchorId;
    int m_anchorVisualIndex = -1;
    int m_lastProcessedIndex = -1;
    int m_maxSelection = 100;
};
