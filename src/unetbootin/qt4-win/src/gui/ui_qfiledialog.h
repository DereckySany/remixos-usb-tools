/*
*********************************************************************
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
*********************************************************************
*/

/********************************************************************************
** Form generated from reading ui file 'qfiledialog.ui'
**
** Created: Fri May 30 22:20:15 2008
**      by: Qt User Interface Compiler version 4.4.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_QFILEDIALOG_H
#define UI_QFILEDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QSplitter>
#include <QtGui/QStackedWidget>
#include <QtGui/QToolButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include "qfiledialog_p.h"
#include "qsidebar_p.h"

QT_BEGIN_NAMESPACE

class Ui_QFileDialog
{
public:
    QGridLayout *gridLayout;
    QLabel *lookInLabel;
    QHBoxLayout *hboxLayout;
    QFileDialogComboBox *lookInCombo;
    QToolButton *backButton;
    QToolButton *forwardButton;
    QToolButton *toParentButton;
    QToolButton *newFolderButton;
    QToolButton *listModeButton;
    QToolButton *detailModeButton;
    QSplitter *splitter;
    QSidebar *sidebar;
    QFrame *frame;
    QVBoxLayout *vboxLayout;
    QStackedWidget *stackedWidget;
    QWidget *page;
    QVBoxLayout *vboxLayout1;
    QFileDialogListView *listView;
    QWidget *page_2;
    QVBoxLayout *vboxLayout2;
    QFileDialogTreeView *treeView;
    QLabel *fileNameLabel;
    QFileDialogLineEdit *fileNameEdit;
    QDialogButtonBox *buttonBox;
    QLabel *fileTypeLabel;
    QComboBox *fileTypeCombo;

    void setupUi(QDialog *QFileDialog)
    {
    if (QFileDialog->objectName().isEmpty())
        QFileDialog->setObjectName(QString::fromUtf8("QFileDialog"));
    QFileDialog->resize(521, 316);
    QFileDialog->setSizeGripEnabled(true);
    gridLayout = new QGridLayout(QFileDialog);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    lookInLabel = new QLabel(QFileDialog);
    lookInLabel->setObjectName(QString::fromUtf8("lookInLabel"));

    gridLayout->addWidget(lookInLabel, 0, 0, 1, 1);

    hboxLayout = new QHBoxLayout();
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    lookInCombo = new QFileDialogComboBox(QFileDialog);
    lookInCombo->setObjectName(QString::fromUtf8("lookInCombo"));
    QSizePolicy sizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(1);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(lookInCombo->sizePolicy().hasHeightForWidth());
    lookInCombo->setSizePolicy(sizePolicy);
    lookInCombo->setMinimumSize(QSize(50, 0));

    hboxLayout->addWidget(lookInCombo);

    backButton = new QToolButton(QFileDialog);
    backButton->setObjectName(QString::fromUtf8("backButton"));

    hboxLayout->addWidget(backButton);

    forwardButton = new QToolButton(QFileDialog);
    forwardButton->setObjectName(QString::fromUtf8("forwardButton"));

    hboxLayout->addWidget(forwardButton);

    toParentButton = new QToolButton(QFileDialog);
    toParentButton->setObjectName(QString::fromUtf8("toParentButton"));

    hboxLayout->addWidget(toParentButton);

    newFolderButton = new QToolButton(QFileDialog);
    newFolderButton->setObjectName(QString::fromUtf8("newFolderButton"));

    hboxLayout->addWidget(newFolderButton);

    listModeButton = new QToolButton(QFileDialog);
    listModeButton->setObjectName(QString::fromUtf8("listModeButton"));

    hboxLayout->addWidget(listModeButton);

    detailModeButton = new QToolButton(QFileDialog);
    detailModeButton->setObjectName(QString::fromUtf8("detailModeButton"));

    hboxLayout->addWidget(detailModeButton);


    gridLayout->addLayout(hboxLayout, 0, 1, 1, 2);

    splitter = new QSplitter(QFileDialog);
    splitter->setObjectName(QString::fromUtf8("splitter"));
    QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(splitter->sizePolicy().hasHeightForWidth());
    splitter->setSizePolicy(sizePolicy1);
    splitter->setOrientation(Qt::Horizontal);
    sidebar = new QSidebar(splitter);
    sidebar->setObjectName(QString::fromUtf8("sidebar"));
    splitter->addWidget(sidebar);
    frame = new QFrame(splitter);
    frame->setObjectName(QString::fromUtf8("frame"));
    frame->setFrameShape(QFrame::NoFrame);
    frame->setFrameShadow(QFrame::Raised);
    vboxLayout = new QVBoxLayout(frame);
    vboxLayout->setSpacing(0);
    vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
    vboxLayout->setContentsMargins(0, 0, 0, 0);
    stackedWidget = new QStackedWidget(frame);
    stackedWidget->setObjectName(QString::fromUtf8("stackedWidget"));
    page = new QWidget();
    page->setObjectName(QString::fromUtf8("page"));
    vboxLayout1 = new QVBoxLayout(page);
    vboxLayout1->setSpacing(0);
    vboxLayout1->setObjectName(QString::fromUtf8("vboxLayout1"));
    vboxLayout1->setContentsMargins(0, 0, 0, 0);
    listView = new QFileDialogListView(page);
    listView->setObjectName(QString::fromUtf8("listView"));

    vboxLayout1->addWidget(listView);

    stackedWidget->addWidget(page);
    page_2 = new QWidget();
    page_2->setObjectName(QString::fromUtf8("page_2"));
    vboxLayout2 = new QVBoxLayout(page_2);
    vboxLayout2->setSpacing(0);
    vboxLayout2->setObjectName(QString::fromUtf8("vboxLayout2"));
    vboxLayout2->setContentsMargins(0, 0, 0, 0);
    treeView = new QFileDialogTreeView(page_2);
    treeView->setObjectName(QString::fromUtf8("treeView"));

    vboxLayout2->addWidget(treeView);

    stackedWidget->addWidget(page_2);

    vboxLayout->addWidget(stackedWidget);

    splitter->addWidget(frame);

    gridLayout->addWidget(splitter, 1, 0, 1, 3);

    fileNameLabel = new QLabel(QFileDialog);
    fileNameLabel->setObjectName(QString::fromUtf8("fileNameLabel"));
    QSizePolicy sizePolicy2(QSizePolicy::Minimum, QSizePolicy::Preferred);
    sizePolicy2.setHorizontalStretch(0);
    sizePolicy2.setVerticalStretch(0);
    sizePolicy2.setHeightForWidth(fileNameLabel->sizePolicy().hasHeightForWidth());
    fileNameLabel->setSizePolicy(sizePolicy2);
    fileNameLabel->setMinimumSize(QSize(0, 0));

    gridLayout->addWidget(fileNameLabel, 2, 0, 1, 1);

    fileNameEdit = new QFileDialogLineEdit(QFileDialog);
    fileNameEdit->setObjectName(QString::fromUtf8("fileNameEdit"));
    QSizePolicy sizePolicy3(QSizePolicy::Expanding, QSizePolicy::Fixed);
    sizePolicy3.setHorizontalStretch(1);
    sizePolicy3.setVerticalStretch(0);
    sizePolicy3.setHeightForWidth(fileNameEdit->sizePolicy().hasHeightForWidth());
    fileNameEdit->setSizePolicy(sizePolicy3);

    gridLayout->addWidget(fileNameEdit, 2, 1, 1, 1);

    buttonBox = new QDialogButtonBox(QFileDialog);
    buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
    buttonBox->setOrientation(Qt::Vertical);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::NoButton|QDialogButtonBox::Ok);

    gridLayout->addWidget(buttonBox, 2, 2, 2, 1);

    fileTypeLabel = new QLabel(QFileDialog);
    fileTypeLabel->setObjectName(QString::fromUtf8("fileTypeLabel"));
    QSizePolicy sizePolicy4(QSizePolicy::Preferred, QSizePolicy::Fixed);
    sizePolicy4.setHorizontalStretch(0);
    sizePolicy4.setVerticalStretch(0);
    sizePolicy4.setHeightForWidth(fileTypeLabel->sizePolicy().hasHeightForWidth());
    fileTypeLabel->setSizePolicy(sizePolicy4);

    gridLayout->addWidget(fileTypeLabel, 3, 0, 1, 1);

    fileTypeCombo = new QComboBox(QFileDialog);
    fileTypeCombo->setObjectName(QString::fromUtf8("fileTypeCombo"));
    QSizePolicy sizePolicy5(QSizePolicy::Expanding, QSizePolicy::Fixed);
    sizePolicy5.setHorizontalStretch(0);
    sizePolicy5.setVerticalStretch(0);
    sizePolicy5.setHeightForWidth(fileTypeCombo->sizePolicy().hasHeightForWidth());
    fileTypeCombo->setSizePolicy(sizePolicy5);

    gridLayout->addWidget(fileTypeCombo, 3, 1, 1, 1);

    QWidget::setTabOrder(lookInCombo, backButton);
    QWidget::setTabOrder(backButton, forwardButton);
    QWidget::setTabOrder(forwardButton, toParentButton);
    QWidget::setTabOrder(toParentButton, newFolderButton);
    QWidget::setTabOrder(newFolderButton, listModeButton);
    QWidget::setTabOrder(listModeButton, detailModeButton);
    QWidget::setTabOrder(detailModeButton, sidebar);
    QWidget::setTabOrder(sidebar, listView);
    QWidget::setTabOrder(listView, fileNameEdit);
    QWidget::setTabOrder(fileNameEdit, fileTypeCombo);
    QWidget::setTabOrder(fileTypeCombo, buttonBox);
    QWidget::setTabOrder(buttonBox, treeView);

    retranslateUi(QFileDialog);

    stackedWidget->setCurrentIndex(0);


    QMetaObject::connectSlotsByName(QFileDialog);
    } // setupUi

    void retranslateUi(QDialog *QFileDialog)
    {
    lookInLabel->setText(QApplication::translate("QFileDialog", "Look in:", 0, QApplication::UnicodeUTF8));

#ifndef QT_NO_TOOLTIP
    backButton->setToolTip(QApplication::translate("QFileDialog", "Back", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP


#ifndef QT_NO_TOOLTIP
    forwardButton->setToolTip(QApplication::translate("QFileDialog", "Forward", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP


#ifndef QT_NO_TOOLTIP
    toParentButton->setToolTip(QApplication::translate("QFileDialog", "Parent Directory", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP


#ifndef QT_NO_TOOLTIP
    newFolderButton->setToolTip(QApplication::translate("QFileDialog", "Create New Folder", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP


#ifndef QT_NO_TOOLTIP
    listModeButton->setToolTip(QApplication::translate("QFileDialog", "List View", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP


#ifndef QT_NO_TOOLTIP
    detailModeButton->setToolTip(QApplication::translate("QFileDialog", "Detail View", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP

    fileTypeLabel->setText(QApplication::translate("QFileDialog", "Files of type:", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(QFileDialog);
    } // retranslateUi

};

namespace Ui {
    class QFileDialog: public Ui_QFileDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QFILEDIALOG_H
