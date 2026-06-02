#include "QtCustomTree/QtTreeModel.h"
#include "ITreeDataSource.h"
#include "ITreeNodeData.h"

#include <QColor>
#include <QFont>
#include <QIcon>
#include <QApplication>

QtTreeModel::QtTreeModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    m_statusPriority["alarm"] = 4;
    m_statusPriority["warning"] = 3;
    m_statusPriority["fault"] = 2;
    m_statusPriority["disabled"] = 1;
    m_statusPriority["normal"] = 0;
}

void QtTreeModel::setDataSource(ITreeDataSource *source)
{
    m_dataSource = source;
}

void QtTreeModel::clear()
{
    beginResetModel();
    m_nodes.clear();
    m_fetchedNodes.clear();
    endResetModel();
}

void QtTreeModel::rebuild()
{
    if (!m_dataSource) return;
    
    beginResetModel();
    m_nodes.clear();
    m_fetchedNodes.clear();
    
    rebuildFromDataSource();
    
    // Корневые узлы загружены сразу
    for (int i = 0; i < m_nodes.size(); ++i) {
        if (m_nodes[i].parent == -1) {
            m_fetchedNodes.insert(i);
        }
    }
    
    endResetModel();
}

void QtTreeModel::rebuildFromDataSource()
{
    if (!m_dataSource) return;
    
    QHash<QString, int> nodeIdToIndex;
    QList<QString> rootIds = m_dataSource->getRootNodes();
    
    // Строим дерево рекурсивно
    std::function<void(const QString&, int)> buildNode = [&](const QString &nodeId, int parentIdx) {
        auto itemData = m_dataSource->getItemData(nodeId);
        if (!itemData) return;
        
        Node node;
        node.id = nodeId;
        node.name = itemData->name();
        node.type = ItemNode;
        node.data = itemData;
        node.parent = parentIdx;
        
        auto children = m_dataSource->getChildren(nodeId);
        if (!children.isEmpty()) {
            node.type = GroupNode;
        }
        
        int idx = addNode(node);
        nodeIdToIndex[nodeId] = idx;
        
        if (parentIdx >= 0) {
            m_nodes[parentIdx].children.append(idx);
        }
        
        for (const QString &childId : children) {
            buildNode(childId, idx);
        }
    };
    
    for (const QString &rootId : rootIds) {
        buildNode(rootId, -1);
    }
}

int QtTreeModel::addNode(const Node &n)
{
    int i = m_nodes.size();
    m_nodes.append(n);
    return i;
}

std::shared_ptr<ITreeNodeData> QtTreeModel::getItemData(const QModelIndex &idx) const
{
    int i = idx.internalId();
    if (i >= 0 && i < m_nodes.size()) {
        return m_nodes[i].data;
    }
    return nullptr;
}

bool QtTreeModel::hasChildren(const QModelIndex &parent) const
{
    if (!parent.isValid()) return !m_nodes.isEmpty();
    int i = parent.internalId();
    if (i < 0 || i >= m_nodes.size()) return false;
    return !m_nodes[i].children.isEmpty();
}

QModelIndex QtTreeModel::index(int row, int col, const QModelIndex &parent) const
{
    if (!hasIndex(row, col, parent)) return {};
    int parentId = parent.isValid() ? parent.internalId() : -1;
    int count = 0;
    for (int i = 0; i < m_nodes.size(); ++i) {
        if (m_nodes[i].parent == parentId) {
            if (count == row) return createIndex(row, col, i);
            count++;
        }
    }
    return {};
}

QModelIndex QtTreeModel::parent(const QModelIndex &child) const
{
    if (!child.isValid()) return {};
    int i = child.internalId();
    if (i < 0 || i >= m_nodes.size()) return {};
    int p = m_nodes[i].parent;
    if (p < 0) return {};
    int row = 0;
    int gp = m_nodes[p].parent;
    for (int j = 0; j < m_nodes.size(); ++j) {
        if (m_nodes[j].parent == gp) {
            if (j == p) break;
            row++;
        }
    }
    return createIndex(row, 0, p);
}

int QtTreeModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        int n = 0;
        for (const auto &nd : m_nodes) if (nd.parent == -1) n++;
        return n;
    }
    int i = parent.internalId();
    if (i < 0 || i >= m_nodes.size()) return 0;
    
    if (!m_fetchedNodes.contains(i) && m_nodes[i].type != ItemNode) {
        return 0;
    }
    
    return m_nodes[i].children.size();
}

QString QtTreeModel::getMaxStatus(int nodeIndex) const
{
    if (nodeIndex < 0 || nodeIndex >= m_nodes.size()) return "normal";
    const Node &node = m_nodes[nodeIndex];
    
    if (node.type == ItemNode) {
        if (node.data) {
            return node.data->status().isEmpty() ? "normal" : node.data->status();
        }
        return "normal";
    }
    
    QString maxStatus = "normal";
    int maxPriority = 0;
    bool hasNormalOrHigher = false;
    
    for (int childIdx : node.children) {
        QString childStatus = getMaxStatus(childIdx);
        int priority = m_statusPriority.value(childStatus, 0);
        
        if (childStatus != "disabled" && childStatus != "normal") {
            hasNormalOrHigher = true;
        }
        if (childStatus == "normal") {
            hasNormalOrHigher = true;
        }
        
        if (priority > maxPriority) {
            maxPriority = priority;
            maxStatus = childStatus;
            if (maxPriority == 4) break;
        }
    }
    
    if (maxStatus == "disabled" && hasNormalOrHigher) {
        maxStatus = "normal";
        maxPriority = 0;
        for (int childIdx : node.children) {
            QString childStatus = getMaxStatus(childIdx);
            if (childStatus == "disabled") continue;
            int priority = m_statusPriority.value(childStatus, 0);
            if (priority > maxPriority) {
                maxPriority = priority;
                maxStatus = childStatus;
            }
        }
    }
    
    return maxStatus;
}

QVariant QtTreeModel::data(const QModelIndex &idx, int role) const
{
    if (!idx.isValid()) return {};
    int i = idx.internalId();
    if (i < 0 || i >= m_nodes.size()) return {};
    const auto &n = m_nodes[i];
    
    if (role == Qt::BackgroundRole) {
        if (n.type == ItemNode && m_highlightedIds.contains(n.id)) {
            return QColor("#3b82f6");
        }
        return QVariant();
    }
    
    if (role == Qt::ForegroundRole) {
        if (n.type == ItemNode) {
            if (m_highlightedIds.contains(n.id))
                return QColor("#ffffff");
            if (n.data) {
                const QString &s = n.data->status();
                if (s == "alarm")    return QColor("#ef4444");
                if (s == "warning")  return QColor("#f59e0b");
                if (s == "fault")    return QColor("#f97316");
                if (s == "disabled") return QColor("#6b7280");
                return QColor("#10b981");
            }
        }
        if (n.type == GroupNode) {
            QString maxStatus = getMaxStatus(i);
            if (maxStatus == "alarm")    return QColor("#ef4444");
            if (maxStatus == "warning")  return QColor("#f59e0b");
            if (maxStatus == "fault")    return QColor("#f97316");
            if (maxStatus == "disabled") return QColor("#6b7280");
            return QColor("#ffffff");
        }
    }
    
    if (role == Qt::DisplayRole) {
        return n.name;
    }
    
    if (role == Qt::DecorationRole) {
        if (n.type == ItemNode && n.data && n.data->isOnPlan()) {
            return QIcon(":/images/location.png");
        }
        return QVariant();
    }
    
    if (role == Qt::FontRole) {
        if (n.type == GroupNode) {
            QFont font;
            font.setBold(true);
            return font;
        }
        if (n.type == ItemNode && m_highlightedIds.contains(n.id)) {
            QFont font;
            font.setBold(true);
            return font;
        }
    }
    
    if (role == Qt::ToolTipRole) {
        if (n.data) {
            return n.data->tooltip().isEmpty() ? n.name : n.data->tooltip();
        }
        return n.name;
    }
    
    if (role == Qt::UserRole + 1) {
        if (n.type == ItemNode) {
            return n.id;
        }
        return QString();
    }

    return {};
}

void QtTreeModel::setHighlightedIds(const QSet<QString> &ids)
{
    QSet<QString> changed = m_highlightedIds;
    changed.unite(ids);
    
    for (int i = 0; i < m_nodes.size(); ++i) {
        if (m_nodes[i].type == ItemNode && changed.contains(m_nodes[i].id)) {
            QModelIndex idx = indexForNode(i);
            if (idx.isValid()) {
                emit dataChanged(idx, idx, {Qt::BackgroundRole, Qt::FontRole});
            }
        }
    }
    m_highlightedIds = ids;
}

QModelIndex QtTreeModel::indexForNode(int i) const
{
    if (i < 0 || i >= m_nodes.size()) return {};
    const auto &n = m_nodes[i];
    
    if (n.parent >= 0 && !m_fetchedNodes.contains(n.parent)) {
        return {};
    }
    
    int row = 0;
    if (n.parent >= 0) {
        const auto &sib = m_nodes[n.parent].children;
        for (int j = 0; j < sib.size(); ++j) if (sib[j] == i) { row = j; break; }
    } else {
        for (int j = 0; j < m_nodes.size(); ++j) if (m_nodes[j].parent == -1) { if (j == i) break; row++; }
    }
    return createIndex(row, 0, i);
}

void QtTreeModel::updateItemStatus(const QString &itemId, const QString &status)
{
    for (int i = 0; i < m_nodes.size(); ++i) {
        if (m_nodes[i].type == ItemNode && m_nodes[i].id == itemId) {
            if (m_nodes[i].data) {
                // Статус обновляется через ITreeNodeData
            }
            QModelIndex idx = indexForNode(i);
            if (idx.isValid()) {
                emit dataChanged(idx, idx, {Qt::ForegroundRole, Qt::ToolTipRole});
            }
            
            int parentIdx = m_nodes[i].parent;
            while (parentIdx >= 0) {
                if (m_fetchedNodes.contains(parentIdx) || m_nodes[parentIdx].parent == -1) {
                    QModelIndex pIdx = indexForNode(parentIdx);
                    if (pIdx.isValid()) {
                        emit dataChanged(pIdx, pIdx, {Qt::ForegroundRole});
                    }
                }
                parentIdx = m_nodes[parentIdx].parent;
            }
            return;
        }
    }
}

bool QtTreeModel::setData(const QModelIndex &idx, const QVariant &value, int role)
{
    int i = idx.internalId();
    if (i < 0 || i >= m_nodes.size()) return false;
    if (role == Qt::ToolTipRole) {
        m_nodes[i].name = value.toString();
        emit dataChanged(idx, idx, {role});
        return true;
    }
    return false;
}

QModelIndex QtTreeModel::findItemIndex(const QString &itemId) const
{
    for (int i = 0; i < m_nodes.size(); ++i) {
        if (m_nodes[i].type == ItemNode && m_nodes[i].id == itemId) {
            return indexForNode(i);
        }
    }
    return QModelIndex();
}

bool QtTreeModel::canFetchMore(const QModelIndex &parent) const
{
    if (!parent.isValid()) return false;
    
    int i = parent.internalId();
    if (i < 0 || i >= m_nodes.size()) return false;
    
    const Node &node = m_nodes[i];
    if (node.type == ItemNode) return false;
    if (m_fetchedNodes.contains(i)) return false;
    
    return true;
}

void QtTreeModel::fetchMore(const QModelIndex &parent)
{
    if (!parent.isValid()) return;
    
    int i = parent.internalId();
    if (i < 0 || i >= m_nodes.size()) return;
    
    const Node &node = m_nodes[i];
    if (node.type == ItemNode) return;
    if (m_fetchedNodes.contains(i)) return;
    
    m_fetchedNodes.insert(i);
    
    int childCount = node.children.size();
    int chunkSize = 100;
    
    for (int start = 0; start < childCount; start += chunkSize) {
        int end = std::min(start + chunkSize - 1, childCount - 1);
        beginInsertRows(parent, start, end);
        endInsertRows();
        QApplication::processEvents();
    }
}
