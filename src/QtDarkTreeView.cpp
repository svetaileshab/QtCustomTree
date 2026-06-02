#include "QtCustomTree/QtDarkTreeView.h"
#include <QPainter>
#include <QStyleOptionViewItem>

QtDarkTreeView::QtDarkTreeView(QWidget *parent)
    : QTreeView(parent) {}

void QtDarkTreeView::drawBranches(QPainter *painter, const QRect &rect,
                                   const QModelIndex &index) const
{
    if (!index.isValid() || !model() || !model()->hasChildren(index))
        return;

    QStyleOptionViewItem opt;
    opt.initFrom(this);
    opt.rect = rect;
    
    QRect arrowRect = opt.rect;
    arrowRect.setWidth(20);
    
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(QPen(QColor("#94a3b8"), 2));
    
    int cx = arrowRect.center().x();
    int cy = arrowRect.center().y();
    int sz = 5;
    
    if (isExpanded(index)) {
        // ▼
        QPolygonF arrow;
        arrow << QPointF(cx - sz, cy - sz/2)
              << QPointF(cx, cy + sz)
              << QPointF(cx + sz, cy - sz/2);
        painter->drawPolyline(arrow);
    } else {
        // ▶
        QPolygonF arrow;
        arrow << QPointF(cx - sz/2, cy - sz)
              << QPointF(cx + sz, cy)
              << QPointF(cx - sz/2, cy + sz);
        painter->drawPolyline(arrow);
    }
    
    painter->restore();
    
    // Вызываем родительский метод для остальных веток
    QTreeView::drawBranches(painter, rect, index);
}
