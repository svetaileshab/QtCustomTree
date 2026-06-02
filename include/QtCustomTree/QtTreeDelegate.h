#pragma once

#include <QStyledItemDelegate>
#include <QPen>

class QtTreeDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit QtTreeDelegate(QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;

    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const override;

protected:
    bool editorEvent(QEvent *event, QAbstractItemModel *model,
                     const QStyleOptionViewItem &option,
                     const QModelIndex &index) override;
};
