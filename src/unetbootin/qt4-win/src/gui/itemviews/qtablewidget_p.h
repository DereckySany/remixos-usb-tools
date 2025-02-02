/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License versions 2.0 or 3.0 as published by the Free Software
** Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file.  Alternatively you may (at
** your option) use any later version of the GNU General Public
** License if such license has been publicly approved by Trolltech ASA
** (or its successors, if any) and the KDE Free Qt Foundation. In
** addition, as a special exception, Trolltech gives you certain
** additional rights. These rights are described in the Trolltech GPL
** Exception version 1.2, which can be found at
** http://www.trolltech.com/products/qt/gplexception/ and in the file
** GPL_EXCEPTION.txt in this package.
**
** Please review the following information to ensure GNU General
** Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/. If
** you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** In addition, as a special exception, Trolltech, as the sole
** copyright holder for Qt Designer, grants users of the Qt/Eclipse
** Integration plug-in the right for the Qt/Eclipse Integration to
** link to functionality provided by Qt Designer and its related
** libraries.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not expressly
** granted herein.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef QTABLEWIDGET_P_H
#define QTABLEWIDGET_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API. This header file may change
// from version to version without notice, or even be removed.
//
// We mean it.
//

#include <qheaderview.h>
#include <qtablewidget.h>
#include <qabstractitemmodel.h>
#include <private/qabstractitemmodel_p.h>
#include <private/qtableview_p.h>
#include <private/qwidgetitemdata_p.h>

#ifndef QT_NO_TABLEWIDGET

QT_BEGIN_NAMESPACE

// workaround for VC++ 6.0 linker bug
typedef bool(*LessThan)(const QPair<QTableWidgetItem*,int>&,const QPair<QTableWidgetItem*,int>&);

class QTableWidgetMimeData : public QMimeData
{
    Q_OBJECT
public:
    QList<QTableWidgetItem*> items;
};

class QTableModelLessThan
{
public:
    inline bool operator()(QTableWidgetItem *i1, QTableWidgetItem *i2) const
        { return (*i1 < *i2); }
};

class QTableModelGreaterThan
{
public:
    inline bool operator()(QTableWidgetItem *i1, QTableWidgetItem *i2) const
        { return (*i2 < *i1); }
};

class QTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum ItemFlagsExtension {
        ItemIsHeaderItem = 128
    }; // we need this to separate header items from other items

    QTableModel(int rows, int columns, QTableWidget *parent);
    ~QTableModel();

    bool insertRows(int row, int count = 1, const QModelIndex &parent = QModelIndex());
    bool insertColumns(int column, int count = 1, const QModelIndex &parent = QModelIndex());

    bool removeRows(int row, int count = 1, const QModelIndex &parent = QModelIndex());
    bool removeColumns(int column, int count = 1, const QModelIndex &parent = QModelIndex());

    void setItem(int row, int column, QTableWidgetItem *item);
    QTableWidgetItem *takeItem(int row, int column);
    QTableWidgetItem *item(int row, int column) const;
    QTableWidgetItem *item(const QModelIndex &index) const;
    void removeItem(QTableWidgetItem *item);

    void setHorizontalHeaderItem(int section, QTableWidgetItem *item);
    void setVerticalHeaderItem(int section, QTableWidgetItem *item);
    QTableWidgetItem *takeHorizontalHeaderItem(int section);
    QTableWidgetItem *takeVerticalHeaderItem(int section);
    QTableWidgetItem *horizontalHeaderItem(int section);
    QTableWidgetItem *verticalHeaderItem(int section);

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const
        { return QAbstractTableModel::index(row, column, parent); }
    
    QModelIndex index(const QTableWidgetItem *item) const;

    void setRowCount(int rows);
    void setColumnCount(int columns);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    bool setItemData(const QModelIndex &index, const QMap<int, QVariant> &roles);

    QMap<int, QVariant> itemData(const QModelIndex &index) const;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role);

    Qt::ItemFlags flags(const QModelIndex &index) const;

    void sort(int column, Qt::SortOrder order);
    static bool itemLessThan(const QPair<QTableWidgetItem*,int> &left,
                             const QPair<QTableWidgetItem*,int> &right);
    static bool itemGreaterThan(const QPair<QTableWidgetItem*,int> &left,
                                const QPair<QTableWidgetItem*,int> &right);
    static bool canConvertToDouble(const QVariant &value);

    void ensureSorted(int column, Qt::SortOrder order, int start, int end);
    QVector<QTableWidgetItem*> columnItems(int column) const;
    void updateRowIndexes(QModelIndexList &indexes, int movedFromRow, int movedToRow);
    static QVector<QTableWidgetItem*>::iterator sortedInsertionIterator(
        const QVector<QTableWidgetItem*>::iterator &begin,
        const QVector<QTableWidgetItem*>::iterator &end,
        Qt::SortOrder order, QTableWidgetItem *item);

    bool isValid(const QModelIndex &index) const;
    inline long tableIndex(int row, int column) const
        { return (row * horizontalHeaderItems.count()) + column; }

    void clear();
    void clearContents();
    void itemChanged(QTableWidgetItem *item);

    QTableWidgetItem *createItem() const;
    const QTableWidgetItem *itemPrototype() const;
    void setItemPrototype(const QTableWidgetItem *item);

    // dnd
    QStringList mimeTypes() const;
    QMimeData *mimeData(const QModelIndexList &indexes) const;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action,
            int row, int column, const QModelIndex &parent);
    Qt::DropActions supportedDropActions() const;

    QMimeData *internalMimeData()  const;

private:
    const QTableWidgetItem *prototype;
    QVector<QTableWidgetItem*> tableItems;
    QVector<QTableWidgetItem*> verticalHeaderItems;
    QVector<QTableWidgetItem*> horizontalHeaderItems;

    // A cache must be mutable if get-functions should have const modifiers
    mutable QModelIndexList cachedIndexes;
};

class QTableWidgetPrivate : public QTableViewPrivate
{
    Q_DECLARE_PUBLIC(QTableWidget)
public:
    QTableWidgetPrivate() : QTableViewPrivate() {}
    inline QTableModel *model() const { return qobject_cast<QTableModel*>(q_func()->model()); }
    void setup();

    // view signals
    void _q_emitItemPressed(const QModelIndex &index);
    void _q_emitItemClicked(const QModelIndex &index);
    void _q_emitItemDoubleClicked(const QModelIndex &index);
    void _q_emitItemActivated(const QModelIndex &index);
    void _q_emitItemEntered(const QModelIndex &index);
    // model signals
    void _q_emitItemChanged(const QModelIndex &index);
    // selection signals
    void _q_emitCurrentItemChanged(const QModelIndex &previous, const QModelIndex &current);
    // sorting
    void _q_sort();
    void _q_dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
};

class QTableWidgetItemPrivate
{
public:
    QTableWidgetItemPrivate(QTableWidgetItem *item) : q(item), id(-1) {}
    QTableWidgetItem *q;
    int id;
};

QT_END_NAMESPACE

#endif // QT_NO_TABLEWIDGET

#endif // QTABLEWIDGET_P_H
