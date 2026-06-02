#include "QtCustomTree/QtCustomTree.h"
#include "QtCustomTree/QtTreeModel.h"
#include "QtCustomTree/QtTreeSelection.h"
#include "QtCustomTree/QtTreeDragHandler.h"
#include "QtCustomTree/QtTreeStatusManager.h"
#include "QtCustomTree/QtTreeContextMenu.h"
#include "ITreeDataSource.h"
#include "ITreeNodeData.h"

#include <QVBoxLayout>
#include <QHeaderView>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QTimer>

QtCustomTree::QtCustomTree(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
    
    m_model = new QtTreeModel(this);
    m_treeView->setModel(m_model);
    
    m_selection = new QtTreeSelection(m_treeView, m_model, this);
    m_selection->setMaxSelection(100);
    
    m_dragHandler = new QtTreeDragHandler(m_treeView, m_model, this);
    m_statusManager = new QtTreeStatusManager(m_treeView, m_model, this);
    m_contextMenu = new QtTreeContextMenu(m_treeView, m_model, this);
    
    setupConnections();
}

QtCustomTree::~QtCustomTree() {}

void QtCustomTree::setupUi()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    m_treeView = new QTreeView();
    m_treeView->setHeaderHidden(true);
    m_treeView->setRootIsDecorated(true);
    m_treeView->setAlternatingRowColors(true);
    m_treeView->setSelectionMode(QAbstractItemView::NoSelection);
    m_treeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_treeView->setDragEnabled(true);
    m_treeView->setDragDropMode(QAbstractItemView::DragOnly);
    m_treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    m_treeView->setAnimated(true);
    m_treeView->setStyleSheet(
        "QTreeView {"
        "  background: #0f172a;"
        "  alternate-background-color: #1e293b;"
        "  selection-background-color: #3b82f6;"
        "  color: #f1f5f9;"
        "}"
        "QTreeView::item:selected { background: #3b82f6; color: #ffffff; }"
        "QTreeView::branch { background: transparent; }"
    );

    layout->addWidget(m_treeView);
}

void QtCustomTree::setupConnections()
{
    connect(m_treeView, &QTreeView::doubleClicked, this, [this](const QModelIndex &index) {
        if (m_model->hasChildren(index)) {
            m_treeView->setExpanded(index, !m_treeView->isExpanded(index));
        } else {
            QString id = m_model->data(index, Qt::UserRole + 1).toString();
            if (!id.isEmpty()) {
                emit itemDoubleClicked(id);
            }
        }
    });
    
    connect(m_treeView, &QTreeView::customContextMenuRequested, this, [this](const QPoint &pos) {
        m_contextMenu->show(pos);
    });
    
    connect(m_contextMenu, &QtTreeContextMenu::refreshRequested, this, [this]() {
        rebuild();
    });
    
    connect(m_selection, &QtTreeSelection::selectionChanged, this, [this]() {
        QList<QString> ids = m_selection->getSelectedIds();
        m_model->setHighlightedIds(QSet<QString>(ids.begin(), ids.end()));
        m_dragHandler->setSelectedIds(ids);
        emit selectionChanged();
    });
    
    m_treeView->viewport()->installEventFilter(this);
    m_treeView->installEventFilter(this);
}

void QtCustomTree::setDataSource(ITreeDataSource *source)
{
    m_model->setDataSource(source);
}

void QtCustomTree::rebuild()
{
    m_model->rebuild();
    if (m_model->rowCount() > 0) {
        m_treeView->expandToDepth(0);
    }
}

void QtCustomTree::clear()
{
    m_model->clear();
}

void QtCustomTree::setMaxSelection(int max)
{
    m_selection->setMaxSelection(max);
}

QList<QString> QtCustomTree::getSelectedIds() const
{
    return m_selection->getSelectedIds();
}

void QtCustomTree::selectItemById(const QString &id)
{
    QModelIndex idx = m_model->findItemIndex(id);
    if (idx.isValid()) {
        m_treeView->scrollTo(idx);
        m_selection->handleSingleClick(idx);
    }
}

bool QtCustomTree::isItemIndex(const QModelIndex &idx) const
{
    if (!idx.isValid()) return false;
    return !m_model->data(idx, Qt::UserRole + 1).toString().isEmpty();
}

bool QtCustomTree::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == m_treeView->viewport() && event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *me = static_cast<QMouseEvent*>(event);
        QModelIndex idx = m_treeView->indexAt(me->pos());
        
        if (me->button() == Qt::LeftButton) {
            if (me->modifiers() & Qt::ShiftModifier) {
                m_selection->handleShiftClick(idx);
                return true;
            } else if (me->modifiers() & Qt::ControlModifier) {
                m_dragHandler->prepareDrag(me);
                return false;
            } else {
                m_selection->handleSingleClick(idx);
                if (m_model->hasChildren(idx)) {
                    return false; // Позволяем раскрыть узел
                }
                return true;
            }
        }
    }

    if (obj == m_treeView && event->type() == QEvent::KeyPress) {
        QKeyEvent *ke = static_cast<QKeyEvent*>(event);
        if (ke->key() == Qt::Key_Down && (ke->modifiers() & Qt::ShiftModifier)) {
            m_selection->navigateDownWithShift();
            return true;
        }
        if (ke->key() == Qt::Key_Up && (ke->modifiers() & Qt::ShiftModifier)) {
            m_selection->navigateUpWithShift();
            return true;
        }
    }

    if (event->type() == QEvent::MouseMove) {
        if (m_dragHandler->handleMouseMove(static_cast<QMouseEvent*>(event))) {
            return true;
        }
    }
    
    return QWidget::eventFilter(obj, event);
}
