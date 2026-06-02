#pragma once

#include <QAbstractItemModel>
#include <QList>
#include <QSet>
#include <QHash>
#include <memory>

class ITreeDataSource;
class ITreeNodeData;

class QtTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    enum NodeType { GroupNode, ItemNode };

    struct Node {
        NodeType type;
        QString id;
        QString name;
        std::shared_ptr<ITreeNodeData> data;
        int parent = -1;
        QList<int> children;
    };

    explicit QtTreeModel(QObject *parent = nullptr);

    void setDataSource(ITreeDataSource *source);
    void rebuild();

    std::shared_ptr<ITreeNodeData> getItemData(const QModelIndex &idx) const;
    bool hasChildren(const QModelIndex &parent) const override;

    QModelIndex index(int row, int col, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &) const override { return 1; }
    QVariant data(const QModelIndex &idx, int role) const override;

    void setHighlightedIds(const QSet<QString> &ids);
    void clear();
    void updateItemStatus(const QString &itemId, const QString &status);

    bool setData(const QModelIndex &idx, const QVariant &value, int role) override;
    QModelIndex findItemIndex(const QString &itemId) const;

    bool canFetchMore(const QModelIndex &parent) const override;
    void fetchMore(const QModelIndex &parent) override;

private:
    void rebuildFromDataSource();
    int addNode(const Node &n);
    QModelIndex indexForNode(int i) const;
    QString getMaxStatus(int nodeIndex) const;

    ITreeDataSource *m_dataSource = nullptr;
    QList<Node> m_nodes;
    QSet<QString> m_highlightedIds;
    QSet<int> m_fetchedNodes;
    QMap<QString, int> m_statusPriority;
};
