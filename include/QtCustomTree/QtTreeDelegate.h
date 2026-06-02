#pragma once

#include <QStyledItemDelegate>

class QtTreeDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit QtTreeDelegate(QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;
};
