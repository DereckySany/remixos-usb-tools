/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the tools applications of the Qt Toolkit.
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

/*
  text.cpp
*/

#include <qregexp.h>

#include "text.h"
#include <stdio.h>

QT_BEGIN_NAMESPACE

Text::Text()
    : first(0), last(0)
{
}

Text::Text(const QString &str)
    : first(0), last(0)
{
    operator<<(str);
}

Text::Text( const Text& text )
    : first(0), last(0)
{
    operator=( text );
}

Text::~Text()
{
    clear();
}

Text& Text::operator=( const Text& text )
{
    if ( this != &text ) {
	clear();
	operator<<( text );
    }
    return *this;
}

Text& Text::operator<<( Atom::Type atomType )
{
    return operator<<( Atom(atomType) );
}

Text& Text::operator<<( const QString& string )
{
    return operator<<( Atom(Atom::String, string) );
}

Text& Text::operator<<( const Atom& atom )
{
    if ( first == 0 ) {
	first = new Atom( atom.type(), atom.string() );
	last = first;
    } else {
	last = new Atom( last, atom.type(), atom.string() );
    }
    return *this;
}

Text& Text::operator<<( const Text& text )
{
    const Atom *atom = text.firstAtom();
    while ( atom != 0 ) {
	operator<<( *atom );
	atom = atom->next();
    }
    return *this;
}

void Text::stripFirstAtom()
{
    if ( first != 0 ) {
	if ( first == last )
	    last = 0;
	Atom *oldFirst = first;
	first = first->next();
	delete oldFirst;
    }
}

void Text::stripLastAtom()
{
    if ( last != 0 ) {
	Atom *oldLast = last;
	if ( first == last ) {
	    first = 0;
	    last = 0;
	} else {
	    last = first;
	    while ( last->next() != oldLast )
		last = last->next();
	    last->setNext( 0 );
	}
	delete oldLast;
    }
}

QString Text::toString() const
{
    QString str;
    const Atom *atom = firstAtom();
    while ( atom != 0 ) {
	if ( atom->type() == Atom::String || atom->type() == Atom::AutoLink )
	    str += atom->string();
	atom = atom->next();
    }
    return str;
}

Text Text::subText( Atom::Type left, Atom::Type right, const Atom *from ) const
{
    const Atom *begin = from ? from : firstAtom();
    const Atom *end;

    while ( begin != 0 && begin->type() != left )
	begin = begin->next();
    if ( begin != 0 )
	begin = begin->next();

    end = begin;
    while ( end != 0 && end->type() != right )
	end = end->next();
    if ( end == 0 )
	begin = 0;
    return subText( begin, end );
}

Text Text::sectionHeading(const Atom *sectionLeft)
{
    if ( sectionLeft != 0 ) {
	const Atom *begin = sectionLeft;
	while ( begin != 0 && begin->type() != Atom::SectionHeadingLeft )
	    begin = begin->next();
	if ( begin != 0 )
	    begin = begin->next();

	const Atom *end = begin;
	while ( end != 0 && end->type() != Atom::SectionHeadingRight )
	    end = end->next();

	if ( end != 0 )
	    return subText( begin, end );
    }
    return Text();
}

const Atom *Text::sectionHeadingAtom(const Atom *sectionLeft)
{
    if ( sectionLeft != 0 ) {
	const Atom *begin = sectionLeft;
	while ( begin != 0 && begin->type() != Atom::SectionHeadingLeft )
	    begin = begin->next();
	if ( begin != 0 )
	    begin = begin->next();

	return begin;
    }
    return 0;
}

void Text::dump() const
{
    const Atom *atom = firstAtom();
    while ( atom != 0 ) {
	QString str = atom->string();
	str.replace( "\\", "\\\\" );
	str.replace( "\"", "\\\"" );
	str.replace( "\n", "\\n" );
	str.replace( QRegExp("[^\x20-\x7e]"), "?" );
	if ( !str.isEmpty() )
	    str = " \"" + str + "\"";
	fprintf(stderr, "    %-15s%s\n", atom->typeString().toLatin1().data(), str.toLatin1().data() );
	atom = atom->next();
    }
}

Text Text::subText( const Atom *begin, const Atom *end )
{
    Text text;
    if ( begin != 0 ) {
	while ( begin != end ) {
	    text << *begin;
	    begin = begin->next();
	}
    }
    return text;
}

void Text::clear()
{
    while ( first != 0 ) {
	Atom *atom = first;
	first = first->next();
	delete atom;
    }
    first = 0;
    last = 0;
}

int Text::compare(const Text &text1, const Text &text2)
{
    if (text1.isEmpty())
	return text2.isEmpty() ? 0 : -1;
    if (text2.isEmpty())
	return 1;

    const Atom *atom1 = text1.firstAtom();
    const Atom *atom2 = text2.firstAtom();

    for (;;) {
	if (atom1->type() != atom2->type())
	    return (int)atom1->type() - (int)atom2->type();
	int cmp = QString::compare(atom1->string(), atom2->string());
        if (cmp != 0)
	    return cmp;

    	if (atom1 == text1.lastAtom())
	    return atom2 == text2.lastAtom() ? 0 : -1;
	if (atom2 == text2.lastAtom())
	    return 1;
	atom1 = atom1->next();
        atom2 = atom2->next();
    }
}

QT_END_NAMESPACE
