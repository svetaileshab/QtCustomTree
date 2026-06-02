#pragma once

#include <QObject>
#include <QTreeView>
#include <QModelIndex>
#include <functional>
#include <memory>

class QAbstractItemModel;
class ITreeNodeData;

class QtTreeStatusManager : public QObject
{
    Q_OBJECT

public:
    explicit QtTreeStatusManager(QTreeView *treeView, QAbstractItemModel *model, QObject *parent = nullptr);
    ~QtTreeStatusManager();

    void updateItemStatus(const QString &itemId, const QString &status);
    void updateAllStatuses(std::function<QString(const QString&)> statusProvider);
    void updateItemTooltip(const QModelIndex &index, std::shared_ptr<ITreeNodeData> data);

    static QColor statusToColor(const QString &status);

private:
    QTreeView *m_treeView;
    QAbstractItemModel *m_model;
};
