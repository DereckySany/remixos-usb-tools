/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtXMLPatterns module of the Qt Toolkit.
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

#include "qcommonvalues_p.h"
#include "qpatternistlocale_p.h"
#include "qschemanumeric_p.h"
#include "qatomicstring_p.h"
#include "qtocodepointsiterator_p.h"

#include "qassemblestringfns_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

/*
 * Determines whether @p cp is a valid XML 1.0 character.
 *
 * @see <a href="http://www.w3.org/TR/REC-xml/#charsets">Extensible Markup
 * Language (XML) 1.0 (Third Edition)2.2 Characters</a>
 */
static inline bool isValidXML10Char(const qint32 cp)
{
    /* [2]     Char     ::=     #x9 | #xA | #xD | [#x20-#xD7FF] |
     *                          [#xE000-#xFFFD] | [#x10000-#x10FFFF]
     */
    return (cp == 0x9                       ||
            cp == 0xA                       ||
            cp == 0xD                       ||
            (0x20 <= cp && cp <= 0xD7FF)    ||
            (0xE000 <= cp && cp <= 0xFFFD)  ||
            (0x10000 <= cp && cp <= 0x10FFFF));
}

Item CodepointsToStringFN::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    const Item::Iterator::Ptr it(m_operands.first()->evaluateSequence(context));

    if(!it)
        return CommonValues::EmptyString;

    QString retval;
    Item item(it->next());
    while(item)
    {
        const qint32 cp = static_cast<qint32>(item.as<Numeric>()->toInteger());

        if(!isValidXML10Char(cp))
        {
            context->error(QtXmlPatterns::tr("%1 is not a valid XML 1.0 character.")
                                            .arg(formatData(QLatin1String("0x") +
                                                          QString::number(cp, 16))),
                                       ReportContext::FOCH0001, this);

            return CommonValues::EmptyString;
        }
        retval.append(QChar(cp));
        item = it->next();
    }

    return AtomicString::fromValue(retval);
}

Item::Iterator::Ptr StringToCodepointsFN::evaluateSequence(const DynamicContext::Ptr &context) const
{
    const Item item(m_operands.first()->evaluateSingleton(context));
    if(!item)
        return CommonValues::emptyIterator;

    const QString str(item.stringValue());
    if(str.isEmpty())
        return CommonValues::emptyIterator;
    else
        return Item::Iterator::Ptr(new ToCodepointsIterator(str));
}

QT_END_NAMESPACE
