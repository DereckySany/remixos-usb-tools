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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of Qt Designer.  This header
// file may change from version to version without notice, or even be removed.
//
// We mean it.
//

#ifndef STYLESHEETEDITOR_H
#define STYLESHEETEDITOR_H

#include <QtGui/QTextEdit>
#include <QtGui/QDialog>
#include <QtGui/QLabel>
#include "shared_global_p.h"

QT_BEGIN_NAMESPACE

class QDesignerFormWindowInterface;
class QDesignerFormEditorInterface;

class QDialogButtonBox;

namespace qdesigner_internal {

class QDESIGNER_SHARED_EXPORT StyleSheetEditor : public QTextEdit
{
    Q_OBJECT
public:
    StyleSheetEditor(QWidget *parent = 0);
};

// Edit a style sheet.
class QDESIGNER_SHARED_EXPORT StyleSheetEditorDialog : public QDialog
{
    Q_OBJECT
public:
    enum Mode {
        ModeGlobal, // resources are disabled (we don't have current resource set loaded), used e.g. in configuration dialog context
        ModePerForm // resources are available
    };

    StyleSheetEditorDialog(QDesignerFormEditorInterface *core, QWidget *parent, Mode mode = ModePerForm);
    ~StyleSheetEditorDialog();
    QString text() const;
    void setText(const QString &t);

    static bool isStyleSheetValid(const QString &styleSheet);


private slots:
    void validateStyleSheet();
    void slotContextMenuRequested(const QPoint &pos);
    void slotAddResource(const QString &property);
    void slotAddGradient(const QString &property);
    void slotAddColor(const QString &property);
    void slotAddFont();
    void slotRequestHelp();

protected:
    QDialogButtonBox *buttonBox() const;
    void setOkButtonEnabled(bool v);

private:
    void insertCssProperty(const QString &name, const QString &value);

    QDialogButtonBox *m_buttonBox;
    StyleSheetEditor *m_editor;
    QLabel *m_validityLabel;
    QDesignerFormEditorInterface *m_core;
    QAction *m_addResourceAction;
    QAction *m_addGradientAction;
    QAction *m_addColorAction;
    QAction *m_addFontAction;
};

// Edit the style sheet property of the designer selection.
// Provides an "Apply" button.

class QDESIGNER_SHARED_EXPORT StyleSheetPropertyEditorDialog : public StyleSheetEditorDialog
{
    Q_OBJECT
public:
    StyleSheetPropertyEditorDialog(QWidget *parent, QDesignerFormWindowInterface *fw, QWidget *widget);

    static bool isStyleSheetValid(const QString &styleSheet);

private slots:
    void applyStyleSheet();

private:
    QDesignerFormWindowInterface *m_fw;
    QWidget *m_widget;
};

} // namespace qdesigner_internal

QT_END_NAMESPACE

#endif // STYLESHEETEDITOR_H
