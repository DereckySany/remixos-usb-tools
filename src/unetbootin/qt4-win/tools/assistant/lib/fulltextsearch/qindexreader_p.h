/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QCLucene library and is distributable under
** the terms of the LGPL license as specified in the license.txt file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef QINDEXREADER_P_H
#define QINDEXREADER_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API. It exists for the convenience
// of the help generator tools. This header file may change from version
// to version without notice, or even be removed.
//
// We mean it.
//

#include "qterm_p.h"
#include "qdocument_p.h"
#include "qclucene_global_p.h"

#include <QtCore/QList>
#include <QtCore/QString>
#include <QtCore/QSharedDataPointer>
#include <QtCore/QSharedData>

CL_NS_DEF(index)
    class IndexReader;
CL_NS_END
CL_NS_USE(index)

QT_BEGIN_NAMESPACE

class QCLuceneIndexWriter;
class QCLuceneIndexSearcher;

class QHELP_EXPORT QCLuceneIndexReaderPrivate : public QSharedData
{
public:
    QCLuceneIndexReaderPrivate();
    QCLuceneIndexReaderPrivate(const QCLuceneIndexReaderPrivate &other);

    ~QCLuceneIndexReaderPrivate();

    IndexReader *reader;
    bool deleteCLuceneIndexReader;

private:
    QCLuceneIndexReaderPrivate &operator=(const QCLuceneIndexReaderPrivate &other);
};

class QHELP_EXPORT QCLuceneIndexReader
{
public:
    enum FieldOption {
		ALL = 1,
		INDEXED = 2,
		UNINDEXED = 4,
		INDEXED_WITH_TERMVECTOR = 8,
		INDEXED_NO_TERMVECTOR = 16,
		TERMVECTOR = 32,
		TERMVECTOR_WITH_POSITION = 64,
		TERMVECTOR_WITH_OFFSET = 128,
		TERMVECTOR_WITH_POSITION_OFFSET = 256
	};

    virtual ~QCLuceneIndexReader();

    static bool isLuceneFile(const QString &filename);
    static bool indexExists(const QString &directory);
    static QCLuceneIndexReader open(const QString &path);
    
    static void unlock(const QString &path);
    static bool isLocked(const QString &directory);

	static quint64 lastModified(const QString &directory);
	static qint64 getCurrentVersion(const QString &directory);

    void close();
    bool isCurrent();
	void undeleteAll();
	qint64 getVersion();
	void deleteDocument(qint32 docNum);
    bool hasNorms(const QString &field);
    qint32 deleteDocuments(const QCLuceneTerm &term);
    bool document(qint32 index, QCLuceneDocument &document);
    void setNorm(qint32 doc, const QString &field, qreal value);
    void setNorm(qint32 doc, const QString &field, quint8 value);

protected:
    friend class QCLuceneIndexWriter;
    friend class QCLuceneIndexSearcher;
    QSharedDataPointer<QCLuceneIndexReaderPrivate> d;

private:
    QCLuceneIndexReader();
};

QT_END_NAMESPACE

#endif  // QINDEXREADER_P_H
