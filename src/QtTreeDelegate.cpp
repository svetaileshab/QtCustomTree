#include "QtCustomTree/QtTreeDelegate.h"
#include <QPainter>
#include <QStyleOptionViewItem>

QtTreeDelegate::QtTreeDelegate(QObject *parent)
    : QStyledItemDelegate(parent) {}

void QtTreeDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                           const QModelIndex &index) const
{
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    // Фон
    if (opt.features & QStyleOptionViewItem::Alternate) {
        painter->fillRect(opt.rect, QColor("#1e293b"));
    } else {
        painter->fillRect(opt.rect, QColor("#0f172a"));
    }

    // Выделение
    QVariant bgColor = index.data(Qt::BackgroundRole);
    if (bgColor.isValid()) {
        painter->fillRect(opt.rect, bgColor.value<QColor>());
    }

    // Текст
    QRect textRect = opt.rect.adjusted(8, 0, -8, 0);
    painter->save();
    
    QVariant fgColor = index.data(Qt::ForegroundRole);
    if (fgColor.isValid()) {
        painter->setPen(fgColor.value<QColor>());
    } else {
        painter->setPen(QColor("#f1f5f9"));
    }
    
    QFont font = opt.font;
    QVariant fontData = index.data(Qt::FontRole);
    if (fontData.isValid()) {
        font = fontData.value<QFont>();
    }
    painter->setFont(font);
    
    painter->drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, opt.text);
    painter->restore();
}
