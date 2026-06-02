#include "QtCustomTree/QtTreeSelection.h"
#include "ITreeNodeData.h"

#include <QAbstractItemModel>
#include <functional>

QtTreeSelection::QtTreeSelection(QTreeView *treeView, QAbstractItemModel *model, QObject *parent)
    : QObject(parent), m_treeView(treeView), m_model(model) {}

QtTreeSelection::~QtTreeSelection()
{
    clearAllSelections();
}

QList<QString> QtTreeSelection::getSelectedIds() const
{
    return m_selectedIds.values();
}

void QtTreeSelection::addToSelection(const QString &id)
{
    if (m_selectedIds.size() >= m_maxSelection) { 
        emit limitReached(); 
        return; 
    }
    m_selectedIds.insert(id);
    emit selectionChanged();
}

void QtTreeSelection::removeFromSelection(const QString &id)
{
    m_selectedIds.remove(id);
    emit selectionChanged();
}

void QtTreeSelection::clearAllSelections()
{
    m_selectedIds.clear();
    m_anchorIndex = QPersistentModelIndex();
    m_anchorId.clear();
    m_anchorVisualIndex = -1;
    m_lastProcessedIndex = -1;
    emit selectionChanged();
}

void QtTreeSelection::setAnchor(const QModelIndex &idx) 
{ 
    m_anchorIndex = idx; 
}

void QtTreeSelection::clearAnchor() 
{ 
    m_anchorIndex = QPersistentModelIndex(); 
}

bool QtTreeSelection::isItemNode(const QModelIndex &idx) const
{
    if (!idx.isValid()) return false;
    QString id = m_model->data(idx, Qt::UserRole).toString();
    if (id.isEmpty()) {
        // Fallback: check DisplayRole
        return !m_model->data(idx, Qt::DisplayRole).toString().isEmpty();
    }
    return !id.isEmpty();
}

QString QtTreeSelection::getIdFromIndex(const QModelIndex &idx) const
{
    if (!idx.isValid()) return {};
    return m_model->data(idx, Qt::UserRole + 1).toString();
}

QModelIndex QtTreeSelection::findIndexById(const QString &id) const
{
    QList<QModelIndex> all = getAllVisibleItems();
    for (const QModelIndex &idx : all) {
        if (getIdFromIndex(idx) == id) return idx;
    }
    return QModelIndex();
}

void QtTreeSelection::handleSingleClick(const QModelIndex &idx)
{
    m_selectedIds.clear();
    m_anchorId.clear();
    m_anchorVisualIndex = -1;
    m_lastProcessedIndex = -1;
    
    if (idx.isValid()) {
        QString id = getIdFromIndex(idx);
        if (!id.isEmpty()) {
            m_selectedIds.insert(id);
            m_anchorId = id;
            m_anchorIndex = idx;
            m_anchorVisualIndex = findVisualIndex(idx);
        }
    }
    updateVisualSelection();
    emit selectionChanged();
}

void QtTreeSelection::handleShiftClick(const QModelIndex &idx)
{
    if (!idx.isValid()) return;
    
    QString id = getIdFromIndex(idx);
    if (id.isEmpty()) return;
    
    if (m_selectedIds.contains(id)) {
        m_selectedIds.remove(id);
    } else {
        if (m_selectedIds.size() >= m_maxSelection) { 
            emit limitReached(); 
            return; 
        }
        m_selectedIds.insert(id);
    }
    m_anchorIndex = idx;
    m_anchorId = id;
    m_anchorVisualIndex = findVisualIndex(idx);
    m_lastProcessedIndex = -1;
    updateVisualSelection();
    emit selectionChanged();
}

void QtTreeSelection::navigateDownWithShift()
{
    if (m_anchorId.isEmpty()) return;
    QList<QModelIndex> items = getAllVisibleItems();
    if (items.isEmpty()) return;
    
    int curIdx = (m_lastProcessedIndex >= 0) ? m_lastProcessedIndex : m_anchorVisualIndex;
    if (curIdx >= items.size() - 1) return;
    int nextIdx = curIdx + 1;
    
    if (nextIdx > m_anchorVisualIndex) {
        QString id = getIdFromIndex(items[nextIdx]);
        if (!m_selectedIds.contains(id)) {
            if (m_selectedIds.size() >= m_maxSelection) { emit limitReached(); return; }
            m_selectedIds.insert(id);
        }
    } else if (nextIdx < m_anchorVisualIndex) {
        QString id = getIdFromIndex(items[curIdx]);
        m_selectedIds.remove(id);
    } else {
        QSet<QString> toRemove;
        for (const QString &selId : m_selectedIds) {
            if (selId != m_anchorId) toRemove.insert(selId);
        }
        for (const QString &remId : toRemove) m_selectedIds.remove(remId);
        QString id = getIdFromIndex(items[nextIdx]);
        if (!m_selectedIds.contains(id)) {
            if (m_selectedIds.size() >= m_maxSelection) { emit limitReached(); return; }
            m_selectedIds.insert(id);
        }
    }
    m_lastProcessedIndex = nextIdx;
    updateVisualSelection();
    emit selectionChanged();
}

void QtTreeSelection::navigateUpWithShift()
{
    if (m_anchorId.isEmpty()) return;
    QList<QModelIndex> items = getAllVisibleItems();
    if (items.isEmpty()) return;
    
    int curIdx = (m_lastProcessedIndex >= 0) ? m_lastProcessedIndex : m_anchorVisualIndex;
    if (curIdx <= 0) return;
    int prevIdx = curIdx - 1;
    
    if (prevIdx < m_anchorVisualIndex) {
        QString id = getIdFromIndex(items[prevIdx]);
        if (!m_selectedIds.contains(id)) {
            if (m_selectedIds.size() >= m_maxSelection) { emit limitReached(); return; }
            m_selectedIds.insert(id);
        }
    } else if (prevIdx > m_anchorVisualIndex) {
        QString id = getIdFromIndex(items[curIdx]);
        m_selectedIds.remove(id);
    } else {
        QSet<QString> toRemove;
        for (const QString &selId : m_selectedIds) {
            if (selId != m_anchorId) toRemove.insert(selId);
        }
        for (const QString &remId : toRemove) m_selectedIds.remove(remId);
        QString id = getIdFromIndex(items[prevIdx]);
        if (!m_selectedIds.contains(id)) {
            if (m_selectedIds.size() >= m_maxSelection) { emit limitReached(); return; }
            m_selectedIds.insert(id);
        }
    }
    m_lastProcessedIndex = prevIdx;
    updateVisualSelection();
    emit selectionChanged();
}

int QtTreeSelection::findVisualIndex(const QModelIndex &idx) const
{
    QList<QModelIndex> all = getAllVisibleItems();
    for (int i = 0; i < all.size(); ++i) {
        if (all[i] == idx) return i;
    }
    return -1;
}

QList<QModelIndex> QtTreeSelection::getAllVisibleItems() const
{
    QList<QModelIndex> items;
    std::function<void(const QModelIndex&)> collect = [&](const QModelIndex &parent) {
        for (int r = 0; r < m_model->rowCount(parent); ++r) {
            QModelIndex idx = m_model->index(r, 0, parent);
            if (idx.isValid()) {
                QString id = getIdFromIndex(idx);
                if (!id.isEmpty()) items.append(idx);
            }
            if (m_treeView->isExpanded(idx)) collect(idx);
        }
    };
    collect(QModelIndex());
    return items;
}

void QtTreeSelection::updateVisualSelection()
{
    // Модель сама обрабатывает выделение через setHighlightedIds
    // Этот метод можно расширить для кастомной визуализации
}
