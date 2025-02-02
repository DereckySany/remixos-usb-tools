/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt Assistant of the Qt Toolkit.
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

#include "qhelpdbreader_p.h"

#include <QtCore/QVariant>
#include <QtCore/QFile>
#include <QtSql/QSqlQuery>

QT_BEGIN_NAMESPACE

QHelpDBReader::QHelpDBReader(const QString &dbName, const QString &uniqueId,
                           QObject *parent)
    : QObject(parent)
{
    m_dbName = dbName;
    m_uniqueId = uniqueId;
    m_initDone = false;
    m_query = 0;
    m_useAttributesCache = false;
}

QHelpDBReader::~QHelpDBReader()
{
    if (m_initDone) {
        delete m_query;
        QSqlDatabase::removeDatabase(m_uniqueId);
    }
}

bool QHelpDBReader::init()
{
    if (m_initDone)
		return true;

    if (!QFile::exists(m_dbName))
        return false;

	QSqlDatabase db = QSqlDatabase::addDatabase(QLatin1String("QSQLITE"), m_uniqueId);
	db.setDatabaseName(m_dbName);
    if (!db.open()) {
        m_error = tr("Cannot open DB!");
        QSqlDatabase::removeDatabase(m_uniqueId);
		return false;
	}

    m_initDone = true;
	m_query = new QSqlQuery(db);

    return true;
}

QString QHelpDBReader::databaseName() const
{
    return m_dbName;
}

QString QHelpDBReader::errorMessage() const
{
    return m_error;
}

QString QHelpDBReader::namespaceName() const
{
    if (!m_namespace.isEmpty())
        return m_namespace;
    if (m_query) {
        m_query->exec(QLatin1String("SELECT Name FROM NamespaceTable"));
        if (m_query->next())
            m_namespace = m_query->value(0).toString();
    }
    return m_namespace;
}

QString QHelpDBReader::virtualFolder() const
{
    if (m_query) {
        m_query->exec(QLatin1String("SELECT Name FROM FolderTable WHERE Id=1"));
        if (m_query->next())
            return m_query->value(0).toString();
    }
    return QString();
}

QList<QStringList> QHelpDBReader::filterAttributeSets() const
{
    QList<QStringList> result;
    if (m_query) {
        m_query->exec(QLatin1String("SELECT a.Id, b.Name FROM FileAttributeSetTable a, "
            "FilterAttributeTable b WHERE a.FilterAttributeId=b.Id ORDER BY a.Id"));
        int oldId = -1;
        while (m_query->next()) {
            int id = m_query->value(0).toInt();
            if (id != oldId) {
                result.append(QStringList());
                oldId = id;
            }
            result.last().append(m_query->value(1).toString());
        }
    }
    return result;
}

bool QHelpDBReader::fileExists(const QString &virtualFolder,
                               const QString &filePath,
                               const QStringList &filterAttributes) const
{
    if (virtualFolder.isEmpty() || filePath.isEmpty() || !m_query)
        return false;
    
//SELECT COUNT(a.Name) FROM FileNameTable a, FolderTable b, FileFilterTable c, FilterAttributeTable d WHERE a.FolderId=b.Id AND b.Name='qtdoc' AND a.Name='qstring.html' AND a.FileId=c.FileId AND c.FilterAttributeId=d.Id AND d.Name='qtrefdoc'

    QString query;
    namespaceName();
    if (filterAttributes.isEmpty()) {
        query = QString(QLatin1String("SELECT COUNT(a.Name) FROM FileNameTable a, FolderTable b "
            "WHERE a.FolderId=b.Id AND b.Name=\'%1\' AND a.Name=\'%2\'")).arg(quote(virtualFolder)).arg(quote(filePath));        
    } else {
        query = QString(QLatin1String("SELECT COUNT(a.Name) FROM FileNameTable a, FolderTable b, "
            "FileFilterTable c, FilterAttributeTable d WHERE a.FolderId=b.Id "
            "AND b.Name=\'%1\' AND a.Name=\'%2\' AND a.FileId=c.FileId AND "
            "c.FilterAttributeId=d.Id AND d.Name=\'%3\'"))
            .arg(quote(virtualFolder)).arg(quote(filePath))
            .arg(quote(filterAttributes.first()));
        for (int i=1; i<filterAttributes.count(); ++i) {
            query.append(QString(QLatin1String(" INTERSECT SELECT COUNT(a.Name) FROM FileNameTable a, "
                "FolderTable b, FileFilterTable c, FilterAttributeTable d WHERE a.FolderId=b.Id "
                "AND b.Name=\'%1\' AND a.Name=\'%2\' AND a.FileId=c.FileId AND "
                "c.FilterAttributeId=d.Id AND d.Name=\'%3\'"))
                .arg(quote(virtualFolder)).arg(quote(filePath))
                .arg(quote(filterAttributes.at(i))));
        }
    }
    m_query->exec(query);
    if (m_query->next() && m_query->isValid() && m_query->value(0).toInt())
        return true;
    return false;
}

QByteArray QHelpDBReader::fileData(const QString &virtualFolder,
                                   const QString &filePath) const
{
    QByteArray ba;
    if (virtualFolder.isEmpty() || filePath.isEmpty() || !m_query)
        return ba;
    
    namespaceName();
    m_query->prepare(QLatin1String("SELECT a.Data FROM FileDataTable a, FileNameTable b, FolderTable c, "
        "NamespaceTable d WHERE a.Id=b.FileId AND (b.Name=? OR b.Name=?) AND b.FolderId=c.Id "
        "AND c.Name=? AND c.NamespaceId=d.Id AND d.Name=?"));
    m_query->bindValue(0, filePath);
    m_query->bindValue(1, QLatin1String("./") + filePath);
    m_query->bindValue(2, virtualFolder);
    m_query->bindValue(3, m_namespace);
    m_query->exec();
    if (m_query->next() && m_query->isValid())
        ba = qUncompress(m_query->value(0).toByteArray());    
    return ba;
}

QStringList QHelpDBReader::customFilters() const
{
    QStringList lst;
    if (m_query) {        
        m_query->exec(QLatin1String("SELECT Name FROM FilterNameTable"));
        while (m_query->next())
            lst.append(m_query->value(0).toString());
    }
    return lst;
}

QStringList QHelpDBReader::filterAttributes(const QString &filterName) const
{
    QStringList lst;
    if (m_query) {
        if (filterName.isEmpty()) {
            m_query->prepare(QLatin1String("SELECT Name FROM FilterAttributeTable"));
        } else {
            m_query->prepare(QLatin1String("SELECT a.Name FROM FilterAttributeTable a, "
                "FilterTable b, FilterNameTable c WHERE c.Name=? "
                "AND c.Id=b.NameId AND b.FilterAttributeId=a.Id"));
            m_query->bindValue(0, filterName);
        }
        m_query->exec();
        while (m_query->next())
            lst.append(m_query->value(0).toString());        
    }
    return lst;
}

QStringList QHelpDBReader::indicesForFilter(const QStringList &filterAttributes) const
{
    QStringList indices;
    if (!m_query)
        return indices;
    
    //SELECT DISTINCT a.Name FROM IndexTable a, IndexFilterTable b, FilterAttributeTable c WHERE a.Id=b.IndexId AND b.FilterAttributeId=c.Id AND c.Name in ('4.2.3', 'qt')
    
    QString query;
    if (filterAttributes.isEmpty()) {
        query = QLatin1String("SELECT DISTINCT Name FROM IndexTable");
    } else {
        query = QString(QLatin1String("SELECT DISTINCT a.Name FROM IndexTable a, "
        "IndexFilterTable b, FilterAttributeTable c WHERE a.Id=b.IndexId "
        "AND b.FilterAttributeId=c.Id AND c.Name='%1'")).arg(quote(filterAttributes.first()));
        for (int i=1; i<filterAttributes.count(); ++i) {
            query.append(QString(QLatin1String(" INTERSECT SELECT DISTINCT a.Name FROM IndexTable a, "
                "IndexFilterTable b, FilterAttributeTable c WHERE a.Id=b.IndexId "
                "AND b.FilterAttributeId=c.Id AND c.Name='%1'"))
                .arg(quote(filterAttributes.at(i))));
        }
    }
    
    m_query->exec(query);
    while (m_query->next()) {
        if (!m_query->value(0).toString().isEmpty())
            indices.append(m_query->value(0).toString());
    }
    return indices;
}

void QHelpDBReader::linksForKeyword(const QString &keyword, const QStringList &filterAttributes,
                                    QMap<QString, QUrl> &linkMap) const
{
    if (!m_query)
        return;

    QString query;
    if (filterAttributes.isEmpty()) {
        query = QString(QLatin1String("SELECT d.Title, f.Name, e.Name, d.Name, a.Anchor "
            "FROM IndexTable a, FileNameTable d, "
            "FolderTable e, NamespaceTable f WHERE "
            "a.FileId=d.FileId AND d.FolderId=e.Id AND a.NamespaceId=f.Id "
            "AND a.Name='%1'")).arg(quote(keyword));
    } else if (m_useAttributesCache) {
        query = QString(QLatin1String("SELECT d.Title, f.Name, e.Name, d.Name, a.Anchor "
            "FROM IndexTable a, AttributeIndexIdTable b, "
            "FileNameTable d, FolderTable e, NamespaceTable f WHERE "
            "a.FileId=d.FileId AND d.FolderId=e.Id "
            "AND a.NamespaceId=f.Id AND a.Id=b.Id AND a.Name='%1'"))
            .arg(quote(keyword));
    } else {
        query = QString(QLatin1String("SELECT d.Title, f.Name, e.Name, d.Name, a.Anchor "
            "FROM IndexTable a, IndexFilterTable b, FilterAttributeTable c, "
            "FileNameTable d, FolderTable e, NamespaceTable f "
            "WHERE a.FileId=d.FileId AND d.FolderId=e.Id "
            "AND a.NamespaceId=f.Id AND b.IndexId=a.Id AND b.FilterAttributeId=c.Id "
            "AND a.Name='%1' AND c.Name='%2'")).arg(quote(keyword))
            .arg(quote(filterAttributes.first()));
        for (int i=1; i<filterAttributes.count(); ++i) {
            query.append(QString(QLatin1String(" INTERSECT SELECT d.Title, f.Name, e.Name, d.Name, a.Anchor "
                "FROM IndexTable a, IndexFilterTable b, FilterAttributeTable c, "
                "FileNameTable d, FolderTable e, NamespaceTable f "
                "WHERE a.FileId=d.FileId AND d.FolderId=e.Id "
                "AND a.NamespaceId=f.Id AND b.IndexId=a.Id AND b.FilterAttributeId=c.Id "
                "AND a.Name='%1' AND c.Name='%2'")).arg(quote(keyword))
                .arg(quote(filterAttributes.at(i))));
        }
    }

    m_query->exec(query);
    while (m_query->next()) {
        linkMap.insertMulti(m_query->value(0).toString(), buildQUrl(m_query->value(1).toString(),
            m_query->value(2).toString(), m_query->value(3).toString(),
            m_query->value(4).toString()));
    }
}

void QHelpDBReader::linksForIdentifier(const QString &id,
                                       const QStringList &filterAttributes,
                                       QMap<QString, QUrl> &linkMap) const
{
    if (!m_query)
        return;

    QString query;
    if (filterAttributes.isEmpty()) {
        query = QString(QLatin1String("SELECT d.Title, f.Name, e.Name, d.Name, a.Anchor "
        "FROM IndexTable a, FileNameTable d, FolderTable e, "
        "NamespaceTable f WHERE a.FileId=d.FileId AND "
        "d.FolderId=e.Id AND a.NamespaceId=f.Id AND a.Identifier='%1'"))
        .arg(quote(id));
    } else if (m_useAttributesCache) {
        query = QString(QLatin1String("SELECT d.Title, f.Name, e.Name, d.Name, a.Anchor "
            "FROM IndexTable a, AttributeIndexIdTable b, "
            "FileNameTable d, FolderTable e, NamespaceTable f WHERE "
            "a.FileId=d.FileId AND d.FolderId=e.Id "
            "AND a.NamespaceId=f.Id AND a.Id=b.Id AND a.Identifier='%1'"))
            .arg(quote(id));
    } else {
        query = QString(QLatin1String("SELECT d.Title, f.Name, e.Name, d.Name, a.Anchor "
            "FROM IndexTable a, IndexFilterTable b, FilterAttributeTable c, "
            "FileNameTable d, FolderTable e, NamespaceTable f "
            "WHERE a.FileId=d.FileId AND d.FolderId=e.Id "
            "AND a.NamespaceId=f.Id AND b.IndexId=a.Id AND b.FilterAttributeId=c.Id "
            "AND a.Identifier='%1' AND c.Name='%2'")).arg(quote(id))
            .arg(quote(filterAttributes.first()));
        for (int i=0; i<filterAttributes.count(); ++i) {
            query.append(QString(QLatin1String(" INTERSECT SELECT d.Title, f.Name, e.Name, "
                "d.Name, a.Anchor FROM IndexTable a, IndexFilterTable b, "
                "FilterAttributeTable c, FileNameTable d, "
                "FolderTable e, NamespaceTable f WHERE "
                "a.FileId=d.FileId AND d.FolderId=e.Id AND a.NamespaceId=f.Id "
                "AND b.IndexId=a.Id AND b.FilterAttributeId=c.Id AND "
                "a.Identifier='%1' AND c.Name='%2'")).arg(quote(id))
                .arg(quote(filterAttributes.at(i))));
        }
    }

    m_query->exec(query);
    while (m_query->next()) {
        linkMap.insertMulti(m_query->value(0).toString(), buildQUrl(m_query->value(1).toString(),
            m_query->value(2).toString(), m_query->value(3).toString(),
            m_query->value(4).toString()));
    }
}

QUrl QHelpDBReader::buildQUrl(const QString &ns, const QString &folder,
                              const QString &relFileName, const QString &anchor) const
{
    QUrl url;
    url.setScheme(QLatin1String("qthelp"));
    url.setAuthority(ns);
    url.setPath(folder + QLatin1Char('/') + relFileName);
    url.setFragment(anchor);
    return url;
}

QList<QByteArray> QHelpDBReader::contentsForFilter(const QStringList &filterAttributes) const
{
	QList<QByteArray> contents;
    if (!m_query)
        return contents;

    //SELECT DISTINCT a.Data FROM ContentsTable a, ContentsFilterTable b, FilterAttributeTable c WHERE a.Id=b.ContentsId AND b.FilterAttributeId=c.Id AND c.Name='qt' INTERSECT SELECT DISTINCT a.Data FROM ContentsTable a, ContentsFilterTable b, FilterAttributeTable c WHERE a.Id=b.ContentsId AND b.FilterAttributeId=c.Id AND c.Name='3.3.8';

    QString query;
    if (filterAttributes.isEmpty()) {
        query = QLatin1String("SELECT Data from ContentsTable");
    } else {
        query = QString(QLatin1String("SELECT a.Data FROM ContentsTable a, "
            "ContentsFilterTable b, FilterAttributeTable c "
            "WHERE a.Id=b.ContentsId AND b.FilterAttributeId=c.Id "
            "AND c.Name='%1'")).arg(quote(filterAttributes.first()));
        for (int i=1; i<filterAttributes.count(); ++i) {
            query.append(QString(QLatin1String(" INTERSECT SELECT a.Data FROM ContentsTable a, "
            "ContentsFilterTable b, FilterAttributeTable c "
            "WHERE a.Id=b.ContentsId AND b.FilterAttributeId=c.Id "
            "AND c.Name='%1'")).arg(quote(filterAttributes.at(i))));
        }
    }

    m_query->exec(query);
    while (m_query->next()) {
		contents.append(m_query->value(0).toByteArray());
    }
    return contents;
}

QUrl QHelpDBReader::urlOfPath(const QString &relativePath) const
{
	QUrl url;
	if (!m_query)
		return url;

	m_query->exec(QLatin1String("SELECT a.Name, b.Name FROM NamespaceTable a, "
		"FolderTable b WHERE a.id=b.NamespaceId and a.Id=1"));
	if (m_query->next()) {
		QString rp = relativePath;
		QString anchor;
		int i = rp.indexOf(QLatin1Char('#'));
		if (i > -1) {
			rp = relativePath.left(i);
			anchor = relativePath.mid(i+1);
		}
		url = buildQUrl(m_query->value(0).toString(),
			m_query->value(1).toString(), rp, anchor);
	}
	return url;
}

QStringList QHelpDBReader::files(const QStringList &filterAttributes,
                                 const QString &extensionFilter) const
{
    QStringList lst;
    if (!m_query)
        return lst;

    QString query;
    QString extension;
    if (!extensionFilter.isEmpty())
        extension = QString(QLatin1String("AND b.Name like \'%.%1\'")).arg(extensionFilter);

    if (filterAttributes.isEmpty()) {
        query = QString(QLatin1String("SELECT a.Name, b.Name FROM FolderTable a, "
            "FileNameTable b WHERE b.FolderId=a.Id %1"))
            .arg(extension);
    } else {
        query = QString(QLatin1String("SELECT a.Name, b.Name FROM FolderTable a, "
            "FileNameTable b, FileFilterTable c, FilterAttributeTable d "
            "WHERE b.FolderId=a.Id AND b.FileId=c.FileId "
            "AND c.FilterAttributeId=d.Id AND d.Name=\'%1\' %2"))
            .arg(quote(filterAttributes.first())).arg(extension);
        for (int i=1; i<filterAttributes.count(); ++i) {
            query.append(QString(QLatin1String(" INTERSECT SELECT a.Name, b.Name FROM "
                "FolderTable a, FileNameTable b, FileFilterTable c, "
                "FilterAttributeTable d WHERE b.FolderId=a.Id AND "
                "b.FileId=c.FileId AND c.FilterAttributeId=d.Id AND "
                "d.Name=\'%1\' %2")).arg(quote(filterAttributes.at(i)))
                .arg(extension));
        }
    }
    m_query->exec(query);
    while (m_query->next()) {
        lst.append(m_query->value(0).toString() + QLatin1Char('/')
            + m_query->value(1).toString());
    }

    return lst;
}

QVariant QHelpDBReader::metaData(const QString &name) const
{
    QVariant v;
    if (!m_query)
        return v;

    m_query->prepare(QLatin1String("SELECT COUNT(Value), Value FROM MetaDataTable "
        "WHERE Name=?"));
    m_query->bindValue(0, name);
    if (m_query->exec() && m_query->next()
        && m_query->value(0).toInt() == 1)
        v = m_query->value(1);
    return v;
}

QString QHelpDBReader::mergeList(const QStringList &list) const
{
    QString str;
    foreach (QString s, list)
        str.append(QLatin1Char('\'') + quote(s) + QLatin1String("\', "));
    if (str.endsWith(QLatin1String(", ")))
        str = str.left(str.length()-2);
    return str;
}

QString QHelpDBReader::quote(const QString &string) const
{
    QString s = string;
    s.replace(QLatin1Char('\''), QLatin1String("\'\'"));
    return s;
}

bool QHelpDBReader::createAttributesCache(const QStringList &attributes)
{
    m_useAttributesCache = false;
    
    if (attributes.count() < 2) {
        m_viewAttributes.clear();
        return true;
    }
    
    bool needUpdate = !m_viewAttributes.count();
    
    foreach (QString s, attributes)
        m_viewAttributes.remove(s);

    if (m_viewAttributes.count() || needUpdate) {
        m_viewAttributes.clear();

        QString query = QLatin1String("DROP TABLE AttributeIndexIdTable");
        m_query->exec(query);

        query = QLatin1String("CREATE TEMP TABLE "
            "AttributeIndexIdTable (Id INTEGER)");

        if (!m_query->exec(query))
            return false;

        query = QString(QLatin1String("INSERT INTO AttributeIndexIdTable "
            "SELECT a.IndexId FROM IndexFilterTable a, "
            "FilterAttributeTable b WHERE a.FilterAttributeId=b.Id "
            "AND b.Name='%1'")).arg(attributes.first());
        for (int i=0; i<attributes.count(); ++i) {
            query.append(QString(QLatin1String(" INTERSECT SELECT a.IndexId FROM "
                "IndexFilterTable a, FilterAttributeTable b WHERE "
                "a.FilterAttributeId=b.Id AND b.Name='%1'"))
                .arg(attributes.at(i)));
        }
        if (!m_query->exec(query))
            return false;
    }
    foreach (QString s, attributes)
        m_viewAttributes.insert(s);
    m_useAttributesCache = true;
    return true;
}

QT_END_NAMESPACE
