/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Qt Designer of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef NEWDYNAMICPROPERTYDIALOG_P_H
#define NEWDYNAMICPROPERTYDIALOG_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "propertyeditor_global.h"
#include <QtGui/QDialog>
#include <QtCore/QVariant>

QT_BEGIN_NAMESPACE

class QAbstractButton;
class QDesignerDialogGuiInterface;

namespace qdesigner_internal {

namespace Ui
{
    class NewDynamicPropertyDialog;
}

class QT_PROPERTYEDITOR_EXPORT NewDynamicPropertyDialog: public QDialog
{
    Q_OBJECT
public:
    explicit NewDynamicPropertyDialog(QDesignerDialogGuiInterface *dialogGui, QWidget *parent = 0);
    ~NewDynamicPropertyDialog();

    void setReservedNames(const QStringList &names);
    void setPropertyType(QVariant::Type t);

    QString propertyName() const;
    QVariant propertyValue() const;

private slots:

    void on_m_buttonBox_clicked(QAbstractButton *btn);
    void nameChanged(const QString &);

private:
    bool validatePropertyName(const QString& name);
    void setOkButtonEnabled(bool e);
    void information(const QString &message);

    QDesignerDialogGuiInterface *m_dialogGui;
    Ui::NewDynamicPropertyDialog *m_ui;
    QStringList m_reservedNames;
};

}  // namespace qdesigner_internal

QT_END_NAMESPACE

#endif // NEWDYNAMICPROPERTYDIALOG_P_H
