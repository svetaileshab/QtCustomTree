#include "QtCustomTree/QtTreeDragHandler.h"
#include "ITreeNodeData.h"

#include <QApplication>
#include <QDrag>
#include <QMimeData>
#include <QJsonArray>
#include <QPainter>
#include <QFontMetrics>
#include <QAbstractItemModel>

QtTreeDragHandler::QtTreeDragHandler(QTreeView *treeView, QAbstractItemModel *model, QObject *parent)
    : QObject(parent), m_treeView(treeView), m_model(model) {}

QtTreeDragHandler::~QtTreeDragHandler() {}

void QtTreeDragHandler::prepareDrag(QMouseEvent *event)
{
    if (!event || !m_model) return;
    
    QModelIndex idx = m_treeView->indexAt(event->pos());
    if (!idx.isValid()) return;
    
    // Получаем ID элемента под курсором
    QString id = m_model->data(idx, Qt::UserRole + 1).toString();
    if (id.isEmpty()) return;
    
    if (!m_dragIds.contains(id)) {
        m_dragIds.clear();
        m_dragIds.append(id);
    }
    
    m_dragStartPosition = event->pos();
    m_isDragPossible = true;
}

bool QtTreeDragHandler::handleMouseMove(QMouseEvent *event)
{
    if (!m_isDragPossible || !(event->buttons() & Qt::LeftButton) || !(event->modifiers() & Qt::ControlModifier))
        return false;
    
    if ((event->pos() - m_dragStartPosition).manhattanLength() >= QApplication::startDragDistance()) {
        if (!m_dragIds.isEmpty()) {
            startDragOperation(m_dragIds, event->pos());
            m_isDragPossible = false;
            return true;
        }
    }
    return false;
}

void QtTreeDragHandler::startDragOperation(const QList<QString> &ids, const QPoint &pos)
{
    Q_UNUSED(pos);
    
    QDrag *drag = new QDrag(m_treeView);
    QMimeData *mimeData = new QMimeData();
    
    QJsonArray arr;
    for (const QString &id : ids) {
        arr.append(serializeItem(id));
    }
    
    mimeData->setData("application/x-qtcustomtree-group", QJsonDocument(arr).toJson());
    
    if (!ids.isEmpty()) {
        mimeData->setData("application/x-qtcustomtree-item", 
                         QJsonDocument(serializeItem(ids.first())).toJson());
        mimeData->setText(ids.first());
    }
    
    drag->setMimeData(mimeData);
    drag->setPixmap(createDragPixmap(ids));
    drag->setHotSpot(QPoint(5, 5));
    
    QApplication::setOverrideCursor(Qt::ClosedHandCursor);
    drag->exec(Qt::CopyAction | Qt::MoveAction);
    QApplication::restoreOverrideCursor();
}

QJsonObject QtTreeDragHandler::serializeItem(const QString &id) const
{
    QJsonObject obj;
    obj["id"] = id;
    
    if (m_model) {
        QModelIndex idx = m_model->data(QModelIndex(), Qt::DisplayRole).isValid() 
                         ? m_model->index(0, 0) 
                         : QModelIndex();
        // Попытка получить данные через модель
        obj["name"] = m_model->data(idx, Qt::DisplayRole).toString();
    }
    
    return obj;
}

QPixmap QtTreeDragHandler::createDragPixmap(const QList<QString> &ids)
{
    QString text = ids.size() == 1 ? ids.first() : QString("[%1] items").arg(ids.size());
    if (text.length() > 25) text = text.left(25) + "...";
    
    QFont font;
    font.setPointSize(10);
    font.setBold(true);
    QFontMetrics fm(font);
    
    int w = fm.horizontalAdvance(text) + 10;
    int h = fm.height() + 10;
    
    QPixmap px(w, h);
    px.fill(Qt::transparent);
    
    QPainter p(&px);
    p.setRenderHint(QPainter::Antialiasing);
    p.setBrush(QColor(60, 60, 60, 150));
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(0, 0, w, h, 5, 5);
    p.setPen(QColor(255, 255, 255, 200));
    p.setFont(font);
    p.drawText(px.rect(), Qt::AlignCenter, text);
    
    return px;
}
