/*
    Copyright (C) 2007 Trolltech ASA

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef QWEBHISTORY_P_H
#define QWEBHISTORY_P_H

#include "BackForwardList.h"
#include "HistoryItem.h"

class QWebHistoryItemPrivate : public QSharedData
{
public:
    QWebHistoryItemPrivate(WebCore::HistoryItem *i)
    {
        i->ref();
        item = i;
    }
    ~QWebHistoryItemPrivate()
    {
        item->deref();
    }
    
    WebCore::HistoryItem *item;
};

class QWebHistoryPrivate : public QSharedData
{
public:
    QWebHistoryPrivate(WebCore::BackForwardList *l)
    {
        l->ref();
        lst = l;
    }
    ~QWebHistoryPrivate()
    {
        lst->deref();
    }
    WebCore::BackForwardList *lst;
};


#endif
