/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtNetwork module of the Qt Toolkit.
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

#include "qnetworkaccesscache_p.h"
#include "QtCore/qpointer.h"
#include "QtCore/qdatetime.h"
#include "QtCore/qqueue.h"
#include "qnetworkaccessmanager_p.h"
#include "qnetworkreply_p.h"
#include "qnetworkrequest.h"

QT_BEGIN_NAMESPACE

enum ExpiryTimeEnum {
    ExpiryTime = 120
};

namespace {
    struct Receiver
    {
        QPointer<QObject> object;
        const char *member;
    };
}

// idea copied from qcache.h
struct QNetworkAccessCache::Node
{
    QDateTime timestamp;
    QQueue<Receiver> receiverQueue;
    QByteArray key;

    Node *older, *newer;
    CacheableObject *object;

    int useCount;

    Node()
        : older(0), newer(0), object(0), useCount(0)
    { }
};

QNetworkAccessCache::CacheableObject::CacheableObject()
{
    // leave the members uninitialized
    // they must be initialized by the derived class's constructor
}

QNetworkAccessCache::CacheableObject::~CacheableObject()
{
#if 0 //def QT_DEBUG
    if (!key.isEmpty() && Ptr()->hasEntry(key))
        qWarning() << "QNetworkAccessCache: object" << (void*)this << "key" << key
                   << "destroyed without being removed from cache first!";
#endif
}

void QNetworkAccessCache::CacheableObject::setExpires(bool enable)
{
    expires = enable;
}

void QNetworkAccessCache::CacheableObject::setShareable(bool enable)
{
    shareable = enable;
}

QNetworkAccessCache::QNetworkAccessCache()
    : oldest(0), newest(0), timerId(-1)
{
}

QNetworkAccessCache::~QNetworkAccessCache()
{
    clear();
}

void QNetworkAccessCache::clear()
{
    NodeHash hashCopy = hash;
    hash.clear();

    // remove all entries
    NodeHash::Iterator it = hashCopy.begin();
    NodeHash::Iterator end = hashCopy.end();
    for ( ; it != end; ++it) {
        it->object->key.clear();
        it->object->dispose();
    }

    // now delete:
    hashCopy.clear();

    if (timerId != -1)
        killTimer(timerId);

    oldest = newest = 0;
}

/*!
    Appens the entry given by @p key to the end of the linked list.
    (i.e., makes it the newest entry)
 */
void QNetworkAccessCache::linkEntry(const QByteArray &key)
{
    NodeHash::Iterator it = hash.find(key);
    if (it == hash.end())
        return;

    Node *const node = &it.value();
    Q_ASSERT(node != oldest && node != newest);
    Q_ASSERT(node->older == 0 && node->newer == 0);
    Q_ASSERT(node->useCount == 0);

    if (newest) {
        Q_ASSERT(newest->newer == 0);
        newest->newer = node;
        node->older = newest;
    }
    if (!oldest) {
        // there are no entries, so this is the oldest one too
        oldest = node;
    }

    node->timestamp = QDateTime::currentDateTime().addSecs(ExpiryTime);
    newest = node;
}

/*!
    Removes the entry pointed by @p key from the linked list.
    Returns true if the entry removed was the oldest one.
 */
bool QNetworkAccessCache::unlinkEntry(const QByteArray &key)
{
    NodeHash::Iterator it = hash.find(key);
    if (it == hash.end())
        return false;

    Node *const node = &it.value();

    bool wasOldest = false;
    if (node == oldest) {
        oldest = node->newer;
        wasOldest = true;
    }
    if (node == newest)
        newest = node->older;
    if (node->older)
        node->older->newer = node->newer;
    if (node->newer)
        node->newer->older = node->older;

    node->newer = node->older = 0;
    return wasOldest;
}

void QNetworkAccessCache::updateTimer()
{
    if (timerId != -1)
        killTimer(timerId);

    if (!oldest)
        return;

    int interval = QDateTime::currentDateTime().secsTo(oldest->timestamp);
    if (interval <= 0) {
        interval = 0;
    } else {
        // round up the interval
        interval = (interval + 15) & ~16;
    }

    timerId = startTimer(interval * 1000);
}

bool QNetworkAccessCache::emitEntryReady(Node *node, QObject *target, const char *member)
{
    if (!connect(this, SIGNAL(entryReady(QNetworkAccessCache::CacheableObject*)),
                 target, member, Qt::QueuedConnection))
        return false;

    emit entryReady(node->object);
    disconnect(SIGNAL(entryReady(QNetworkAccessCache::CacheableObject*)));

    return true;
}

void QNetworkAccessCache::timerEvent(QTimerEvent *)
{
    // expire old items
    QDateTime now = QDateTime::currentDateTime();

    while (oldest && oldest->timestamp < now) {
        Node *next = oldest->newer;
        oldest->object->dispose();

        hash.remove(oldest->key); // oldest gets deleted
        oldest = next;
    }

    // fixup the list
    if (oldest)
        oldest->older = 0;
    else
        newest = 0;

    updateTimer();
}

void QNetworkAccessCache::addEntry(const QByteArray &key, CacheableObject *entry)
{
    Q_ASSERT(!key.isEmpty());

    if (unlinkEntry(key))
        updateTimer();

    Node &node = hash[key];     // create the entry in the hash if it didn't exist
    if (node.useCount)
        qWarning("QNetworkAccessCache::addEntry: overriding active cache entry '%s'",
                 key.constData());
    if (node.object)
        node.object->dispose();
    node.object = entry;
    node.object->key = key;
    node.key = key;
    node.useCount = 1;
}

bool QNetworkAccessCache::hasEntry(const QByteArray &key) const
{
    return hash.contains(key);
}

bool QNetworkAccessCache::requestEntry(const QByteArray &key, QObject *target, const char *member)
{
    NodeHash::Iterator it = hash.find(key);
    if (it == hash.end())
        return false;           // no such entry

    Node *node = &it.value();

    if (node->useCount > 0 && !node->object->shareable) {
        // object is not shareable and is in use
        // queue for later use
        Q_ASSERT(node->older == 0 && node->newer == 0);
        Receiver receiver;
        receiver.object = target;
        receiver.member = member;
        node->receiverQueue.enqueue(receiver);

        // request queued
        return true;
    } else {
        // node not in use or is shareable
        if (unlinkEntry(key))
            updateTimer();

        ++node->useCount;
        return emitEntryReady(node, target, member);
    }
}

QNetworkAccessCache::CacheableObject *QNetworkAccessCache::requestEntryNow(const QByteArray &key)
{
    NodeHash::Iterator it = hash.find(key);
    if (it == hash.end())
        return 0;
    if (it->useCount > 0) {
        if (it->object->shareable) {
            ++it->useCount;
            return it->object;
        }

        // object in use and not shareable
        return 0;
    }

    // entry not in use, let the caller have it
    bool wasOldest = unlinkEntry(key);
    ++it->useCount;

    if (wasOldest)
        updateTimer();
    return it->object;
}

void QNetworkAccessCache::releaseEntry(const QByteArray &key)
{
    NodeHash::Iterator it = hash.find(key);
    if (it == hash.end()) {
        qWarning("QNetworkAccessCache::releaseEntry: trying to release key '%s' that is not in cache",
                 key.constData());
        return;
    }

    Node *node = &it.value();
    Q_ASSERT(node->useCount > 0);

    // are there other objects waiting?
    if (!node->receiverQueue.isEmpty()) {
        // queue another activation
        Receiver receiver;
        do {
            receiver = node->receiverQueue.dequeue();
        } while (receiver.object.isNull() && !node->receiverQueue.isEmpty());

        if (!receiver.object.isNull()) {
            emitEntryReady(node, receiver.object, receiver.member);
            return;
        }
    }

    if (!--node->useCount) {
        // no objects waiting; add it back to the expiry list
        if (node->object->expires)
            linkEntry(key);

        if (oldest == node)
            updateTimer();
    }
}

void QNetworkAccessCache::removeEntry(const QByteArray &key)
{
    NodeHash::Iterator it = hash.find(key);
    if (it == hash.end()) {
        qWarning("QNetworkAccessCache::removeEntry: trying to remove key '%s' that is not in cache",
                 key.constData());
        return;
    }

    Node *node = &it.value();
    if (unlinkEntry(key))
        updateTimer();
    if (node->useCount > 1)
        qWarning("QNetworkAccessCache::removeEntry: removing active cache entry '%s'",
                 key.constData());

    node->object->key.clear();
    hash.remove(node->key);
}

QT_END_NAMESPACE
