#include "QtCustomTree/QtTreeDelegate.h"
#include <QPainter>
#include <QStyleOptionViewItem>
#include <QAbstractItemModel>

QtTreeDelegate::QtTreeDelegate(QObject *parent)
    : QStyledItemDelegate(parent) {}

void QtTreeDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                           const QModelIndex &index) const
{
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    // Рисуем фон (включая alternate colors)
    if (opt.features & QStyleOptionViewItem::Alternate) {
        painter->fillRect(opt.rect, QColor("#1e293b"));
    } else {
        painter->fillRect(opt.rect, QColor("#0f172a"));
    }

    // Фон выделения (из модели)
    QVariant bgColor = index.data(Qt::BackgroundRole);
    if (bgColor.isValid()) {
        painter->fillRect(opt.rect, bgColor.value<QColor>());
    }

    // Рисуем только текст (без стандартного display)
    QRect textRect = opt.rect.adjusted(8, 0, -8, 0);
    
    painter->save();
    
    // Цвет текста
    QVariant fgColor = index.data(Qt::ForegroundRole);
    if (fgColor.isValid()) {
        painter->setPen(fgColor.value<QColor>());
    } else {
        painter->setPen(QColor("#f1f5f9"));
    }
    
    // Шрифт
    QFont font = opt.font;
    QVariant fontData = index.data(Qt::FontRole);
    if (fontData.isValid()) {
        font = fontData.value<QFont>();
    }
    painter->setFont(font);
    
    // Иконка (DecorationRole)
    QVariant iconData = index.data(Qt::DecorationRole);
    if (iconData.isValid() && iconData.type() == QVariant::Icon) {
        QIcon icon = iconData.value<QIcon>();
        QRect iconRect(textRect.x(), textRect.y() + (textRect.height() - 16) / 2, 16, 16);
        icon.paint(painter, iconRect);
        textRect.setX(textRect.x() + 20);
    }
    
    painter->drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, opt.text);
    painter->restore();
}
