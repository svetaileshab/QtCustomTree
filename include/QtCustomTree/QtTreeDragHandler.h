#pragma once

#include <QObject>
#include <QPoint>
#include <QTreeView>
#include <QMouseEvent>
#include <QDrag>
#include <QMimeData>
#include <QJsonObject>
#include <QJsonDocument>
#include <QList>
#include <memory>

class QAbstractItemModel;
class ITreeNodeData;

class QtTreeDragHandler : public QObject
{
    Q_OBJECT

public:
    QtTreeDragHandler(QTreeView *treeView, QAbstractItemModel *model, QObject *parent = nullptr);
    ~QtTreeDragHandler();

    void prepareDrag(QMouseEvent *event);
    bool handleMouseMove(QMouseEvent *event);

    void setSelectedIds(const QList<QString> &ids) { m_dragIds = ids; }

protected:
    virtual QJsonObject serializeItem(const QString &id) const;
    virtual QPixmap createDragPixmap(const QList<QString> &ids);

private:
    void startDragOperation(const QList<QString> &ids, const QPoint &pos);

    QTreeView *m_treeView;
    QAbstractItemModel *m_model;
    QList<QString> m_dragIds;
    bool m_isDragPossible = false;
    QPoint m_dragStartPosition;
};
