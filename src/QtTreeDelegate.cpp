#include "QtCustomTree/QtTreeDelegate.h"
#include <QPainter>
#include <QStyleOptionViewItem>
#include <QAbstractItemModel>
#include <QMouseEvent>
#include <QTreeView>

QtTreeDelegate::QtTreeDelegate(QObject *parent)
    : QStyledItemDelegate(parent) {}

void QtTreeDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                           const QModelIndex &index) const
{
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    bool hasChildren = index.model()->hasChildren(index);

    // Рисуем фон
    if (opt.state & QStyle::State_Selected) {
        painter->fillRect(opt.rect, QColor("#3b82f6"));
    } else if (opt.features & QStyleOptionViewItem::Alternate) {
        painter->fillRect(opt.rect, QColor("#1e293b"));
    } else {
        painter->fillRect(opt.rect, QColor("#0f172a"));
    }

    // Рисуем стрелку для узлов с детьми
    if (hasChildren) {
        QRect arrowRect = opt.rect;
        arrowRect.setWidth(20);
        
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);
        
        QPen pen(QColor("#94a3b8"), 2);
        painter->setPen(pen);
        painter->setBrush(Qt::NoBrush);
        
        int cx = arrowRect.center().x();
        int cy = arrowRect.center().y();
        int sz = 4;
        
        if (opt.state & QStyle::State_Open) {
            // Стрелка вниз ▼
            QPolygon arrow;
            arrow << QPoint(cx - sz, cy - sz/2)
                  << QPoint(cx, cy + sz/2)
                  << QPoint(cx + sz, cy - sz/2);
            painter->drawPolyline(arrow);
        } else {
            // Стрелка вправо ▶
            QPolygon arrow;
            arrow << QPoint(cx - sz/2, cy - sz)
                  << QPoint(cx + sz/2, cy)
                  << QPoint(cx - sz/2, cy + sz);
            painter->drawPolyline(arrow);
        }
        
        painter->restore();
    }

    // Рисуем текст
    QRect textRect = opt.rect;
    if (hasChildren) {
        textRect.setLeft(textRect.left() + 20);
    }
    textRect.adjust(4, 0, -4, 0);

    painter->save();
    if (opt.state & QStyle::State_Selected) {
        painter->setPen(QColor("#ffffff"));
    } else {
        QVariant fgColor = index.data(Qt::ForegroundRole);
        if (fgColor.isValid()) {
            painter->setPen(fgColor.value<QColor>());
        } else {
            painter->setPen(QColor("#f1f5f9"));
        }
    }
    
    QFont font = opt.font;
    painter->setFont(font);
    painter->drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, opt.text);
    painter->restore();
}

QSize QtTreeDelegate::sizeHint(const QStyleOptionViewItem &option,
                               const QModelIndex &index) const
{
    QSize sz = QStyledItemDelegate::sizeHint(option, index);
    sz.setHeight(sz.height() + 8);
    return sz;
}


bool QtTreeDelegate::editorEvent(QEvent *event, QAbstractItemModel *model,
                                 const QStyleOptionViewItem &option,
                                 const QModelIndex &index)
{
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *me = static_cast<QMouseEvent*>(event);

        // Ширина стрелки = 20px, но с учётом отступа (indentation)
        int arrowWidth = 20;
        int level = 0;
        QModelIndex parent = index.parent();
        while (parent.isValid()) {
            level++;
            parent = parent.parent();
        }
        int arrowX = level * 20;  // 20 — стандартный indentation в Qt

        // Проверяем, кликнули ли по стрелке
        if (me->pos().x() >= arrowX && me->pos().x() <= arrowX + arrowWidth
            && model->hasChildren(index)) {
            QTreeView *tree = qobject_cast<QTreeView*>(this->parent());
            if (tree) {
                tree->setExpanded(index, !tree->isExpanded(index));
            }
            return true;
        }
    }

    return QStyledItemDelegate::editorEvent(event, model, option, index);
}
