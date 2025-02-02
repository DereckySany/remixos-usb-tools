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

//#define QTCPSERVER_DEBUG

/*! \class QTcpServer

    \brief The QTcpServer class provides a TCP-based server.

    \reentrant
    \ingroup io
    \module network

    This class makes it possible to accept incoming TCP connections.
    You can specify the port or have QTcpServer pick one
    automatically. You can listen on a specific address or on all the
    machine's addresses.

    Call listen() to have the server listen for incoming connections.
    The newConnection() signal is then emitted each time a client
    connects to the server.

    Call nextPendingConnection() to accept the pending connection as
    a connected QTcpSocket. The function returns a pointer to a
    QTcpSocket in QAbstractSocket::ConnectedState that you can use for
    communicating with the client.

    If an error occurs, serverError() returns the type of error, and
    errorString() can be called to get a human readable description of
    what happened.

    When listening for connections, the address and port on which the
    server is listening are available as serverAddress() and
    serverPort().

    Calling close() makes QTcpServer stop listening for incoming
    connections.

    Although QTcpServer is mostly designed for use with an event
    loop, it's possible to use it without one. In that case, you must
    use waitForNewConnection(), which blocks until either a
    connection is available or a timeout expires.

    \sa QTcpSocket, {Fortune Server Example}, {Threaded Fortune Server Example},
        {Loopback Example}, {Torrent Example}
*/

/*! \fn void QTcpServer::newConnection()

    This signal is emitted every time a new connection is available.

    \sa hasPendingConnections(), nextPendingConnection()
*/

#include "private/qobject_p.h"
#include "qalgorithms.h"
#include "qhostaddress.h"
#include "qlist.h"
#include "qpointer.h"
#include "qnativesocketengine_p.h"
#include "qtcpserver.h"
#include "qtcpsocket.h"
#include "qnetworkproxy.h"

QT_BEGIN_NAMESPACE

#define Q_CHECK_SOCKETENGINE(returnValue) do { \
    if (!d->socketEngine) { \
        return returnValue; \
    } } while (0)

class QTcpServerPrivate : public QObjectPrivate, public QAbstractSocketEngineReceiver
{
    Q_DECLARE_PUBLIC(QTcpServer)
public:
    QTcpServerPrivate();
    ~QTcpServerPrivate();

    QList<QTcpSocket *> pendingConnections;

    quint16 port;
    QHostAddress address;

    QAbstractSocket::SocketState state;
    QAbstractSocketEngine *socketEngine;

    QAbstractSocket::SocketError serverSocketError;
    QString serverSocketErrorString;

    int maxConnections;

#ifndef QT_NO_NETWORKPROXY
    QNetworkProxy *proxy;
#endif

    // from QAbstractSocketEngineReceiver
    void readNotification();
    inline void writeNotification() {}
    inline void exceptionNotification() {}
#ifndef QT_NO_NETWORKPROXY
    inline void proxyAuthenticationRequired(const QNetworkProxy &, QAuthenticator *) {}
#endif

};

/*! \internal
*/
QTcpServerPrivate::QTcpServerPrivate()
 : port(0)
 , state(QAbstractSocket::UnconnectedState)
 , socketEngine(0)
 , serverSocketError(QAbstractSocket::UnknownSocketError)
 , maxConnections(30)
#ifndef QT_NO_NETWORKPROXY
 , proxy(0)
#endif
{
}

/*! \internal
*/
QTcpServerPrivate::~QTcpServerPrivate()
{
#ifndef QT_NO_NETWORKPROXY
     delete proxy;
#endif
}

/*! \internal
*/
void QTcpServerPrivate::readNotification()
{
    Q_Q(QTcpServer);
    for (;;) {
        if (pendingConnections.count() >= maxConnections) {
#if defined (QTCPSERVER_DEBUG)
            qDebug("QTcpServerPrivate::_q_processIncomingConnection() too many connections");
#endif
            if (socketEngine->isReadNotificationEnabled())
                socketEngine->setReadNotificationEnabled(false);
            return;
        }

        int descriptor = socketEngine->accept();
        if (descriptor == -1)
            break;
#if defined (QTCPSERVER_DEBUG)
        qDebug("QTcpServerPrivate::_q_processIncomingConnection() accepted socket %i", descriptor);
#endif
        q->incomingConnection(descriptor);

        QPointer<QTcpServer> that = q;
        emit q->newConnection();
        if (!that || !q->isListening())
            return;
    }
}

/*!
    Constructs a QTcpServer object.

    \a parent is passed to the QObject constructor.

    \sa listen(), setSocketDescriptor()
*/
QTcpServer::QTcpServer(QObject *parent)
    : QObject(*new QTcpServerPrivate, parent)
{
}

/*!
    Destroys the QTcpServer object. If the server is listening for
    connections, the socket is automatically closed.

    Any client \l{QTcpSocket}s that are still connected must either
    disconnect or be reparented before the server is deleted.

    \sa close()
*/
QTcpServer::~QTcpServer()
{
    close();
}

/*!
    Tells the server to listen for incoming connections on address \a
    address and port \a port. If \a port is 0, a port is chosen
    automatically. If \a address is QHostAddress::Any, the server
    will listen on all network interfaces.

    Returns true on success; otherwise returns false.

    \sa isListening()
*/
bool QTcpServer::listen(const QHostAddress &address, quint16 port)
{
    Q_D(QTcpServer);
    if (d->state == QAbstractSocket::ListeningState) {
        qWarning("QTcpServer::listen() called when already listening");
        return false;
    }

    QAbstractSocket::NetworkLayerProtocol proto = address.protocol();

    if (d->socketEngine)
        delete d->socketEngine;
    d->socketEngine = QAbstractSocketEngine::createSocketEngine(address, QAbstractSocket::TcpSocket, this);
    if (!d->socketEngine->initialize(QAbstractSocket::TcpSocket, proto)) {
        d->serverSocketError = d->socketEngine->error();
        d->serverSocketErrorString = d->socketEngine->errorString();
        return false;
    }

#if defined(Q_OS_UNIX)
    // Under Unix, we want to be able to bind to the port, even if a socket on
    // the same address-port is in TIME_WAIT. Under Windows this is possible
    // anyway -- furthermore, the meaning of reusable on Windows is different:
    // it means that you can use the same address-port for multiple listening
    // sockets.
    // Don't abort though if we can't set that option. For example the socks
    // engine doesn't support that option, but that shouldn't prevent us from
    // trying to bind/listen.
    d->socketEngine->setOption(QAbstractSocketEngine::AddressReusable, 1);
#endif

    if (!d->socketEngine->bind(address, port)) {
        d->serverSocketError = d->socketEngine->error();
        d->serverSocketErrorString = d->socketEngine->errorString();
        return false;
    }

    if (!d->socketEngine->listen()) {
        d->serverSocketError = d->socketEngine->error();
        d->serverSocketErrorString = d->socketEngine->errorString();
        return false;
    }

    d->socketEngine->setReceiver(d);
    d->socketEngine->setReadNotificationEnabled(true);

    d->state = QAbstractSocket::ListeningState;
    d->address = d->socketEngine->localAddress();
    d->port = d->socketEngine->localPort();

#if defined (QTCPSERVER_DEBUG)
    qDebug("QTcpServer::listen(%i, \"%s\") == true (listening on port %i)", port,
           address.toString().toLatin1().constData(), d->socketEngine->localPort());
#endif
    return true;
}

/*!
    Returns true if the server is currently listening for incoming
    connections; otherwise returns false.

    \sa listen()
*/
bool QTcpServer::isListening() const
{
    Q_D(const QTcpServer);
    Q_CHECK_SOCKETENGINE(false);
    return d->socketEngine->state() == QAbstractSocket::ListeningState;
}

/*!
    Closes the server. The server will no longer listen for incoming
    connections.

    \sa listen()
*/
void QTcpServer::close()
{
    Q_D(QTcpServer);

    qDeleteAll(d->pendingConnections);
    d->pendingConnections.clear();

    if (d->socketEngine) {
        d->socketEngine->close();
        d->socketEngine->deleteLater();
        d->socketEngine = 0;
    }

    d->state = QAbstractSocket::UnconnectedState;
}

/*!
    Returns the native socket descriptor the server uses to listen
    for incoming instructions, or -1 if the server is not listening.

    If the server is using QNetworkProxy, the returned descriptor may
    not be usable with native socket functions.

    \sa setSocketDescriptor(), isListening()
*/
int QTcpServer::socketDescriptor() const
{
    Q_D(const QTcpServer);
    Q_CHECK_SOCKETENGINE(-1);
    return d->socketEngine->socketDescriptor();
}

/*!
    Sets the socket descriptor this server should use when listening
    for incoming connections to \a socketDescriptor. Returns true if
    the socket is set successfully; otherwise returns false.

    The socket is assumed to be in listening state.

    \sa socketDescriptor(), isListening()
*/
bool QTcpServer::setSocketDescriptor(int socketDescriptor)
{
    Q_D(QTcpServer);
    if (isListening()) {
        qWarning("QTcpServer::setSocketDescriptor() called when already listening");
        return false;
    }

    if (d->socketEngine)
        delete d->socketEngine;
    d->socketEngine = QAbstractSocketEngine::createSocketEngine(socketDescriptor, this);
    if (!d->socketEngine->initialize(socketDescriptor, QAbstractSocket::ListeningState)) {
        d->serverSocketError = d->socketEngine->error();
        d->serverSocketErrorString = d->socketEngine->errorString();
#if defined (QTCPSERVER_DEBUG)
        qDebug("QTcpServer::setSocketDescriptor(%i) failed (%s)", socketDescriptor,
               d->serverSocketErrorString.toLatin1().constData());
#endif
        return false;
    }

    d->socketEngine->setReceiver(d);
    d->socketEngine->setReadNotificationEnabled(true);

    d->state = d->socketEngine->state();
    d->address = d->socketEngine->localAddress();
    d->port = d->socketEngine->localPort();

#if defined (QTCPSERVER_DEBUG)
    qDebug("QTcpServer::setSocketDescriptor(%i) succeeded.", socketDescriptor);
#endif
    return true;
}

/*!
    Returns the server's port if the server is listening for
    connections; otherwise returns 0.

    \sa serverAddress(), listen()
*/
quint16 QTcpServer::serverPort() const
{
    Q_D(const QTcpServer);
    Q_CHECK_SOCKETENGINE(0);
    return d->socketEngine->localPort();
}

/*!
    Returns the server's address if the server is listening for
    connections; otherwise returns QHostAddress::Null.

    \sa serverPort(), listen()
*/
QHostAddress QTcpServer::serverAddress() const
{
    Q_D(const QTcpServer);
    Q_CHECK_SOCKETENGINE(QHostAddress(QHostAddress::Null));
    return d->socketEngine->localAddress();
}

/*!
    Waits for at most \a msec milliseconds or until an incoming
    connection is available. Returns true if a connection is
    available; otherwise returns false. If the operation timed out
    and \a timedOut is not 0, *\a timedOut will be set to true.

    This is a blocking function call. Its use is disadvised in a
    single-threaded GUI application, since the whole application will
    stop responding until the function returns.
    waitForNewConnection() is mostly useful when there is no event
    loop available.

    The non-blocking alternative is to connect to the newConnection()
    signal.

    If msec is -1, this function will not time out.

    \sa hasPendingConnections(), nextPendingConnection()
*/
bool QTcpServer::waitForNewConnection(int msec, bool *timedOut)
{
    Q_D(QTcpServer);
    if (d->state != QAbstractSocket::ListeningState)
        return false;

    if (!d->socketEngine->waitForRead(msec, timedOut)) {
        d->serverSocketError = d->socketEngine->error();
        d->serverSocketErrorString = d->socketEngine->errorString();
        return false;
    }

    if (timedOut && *timedOut)
        return false;

    d->readNotification();

    return true;
}

/*!
    Returns true if the server has a pending connection; otherwise
    returns false.

    \sa nextPendingConnection(), setMaxPendingConnections()
*/
bool QTcpServer::hasPendingConnections() const
{
    return !d_func()->pendingConnections.isEmpty();
}

/*!
    Returns the next pending connection as a connected QTcpSocket
    object.

    The socket is created as a child of the server, which means that
    it is automatically deleted when the QTcpServer object is
    destroyed. It is still a good idea to delete the object
    explicitly when you are done with it, to avoid wasting memory.

    0 is returned if this function is called when there are no pending
    connections.

    \sa hasPendingConnections()
*/
QTcpSocket *QTcpServer::nextPendingConnection()
{
    Q_D(QTcpServer);
    if (d->pendingConnections.isEmpty())
        return 0;

    if (!d->socketEngine->isReadNotificationEnabled())
        d->socketEngine->setReadNotificationEnabled(true);

    return d->pendingConnections.takeFirst();
}

/*!
    This virtual function is called by QTcpServer when a new
    connection is available. The \a socketDescriptor argument is the
    native socket descriptor for the accepted connection.

    The base implementation creates a QTcpSocket, sets the socket
    descriptor and then stores the QTcpSocket in an internal list of
    pending connections. Finally newConnection() is emitted.

    Reimplement this function to alter the server's behavior when a
    connection is available.

    If this server is using QNetworkProxy then the \a socketDescriptor
    may not be usable with native socket functions, and should only be
    used with QTcpSocket::setSocketDescriptor().

    \sa newConnection(), nextPendingConnection()
*/
void QTcpServer::incomingConnection(int socketDescriptor)
{
#if defined (QTCPSERVER_DEBUG)
    qDebug("QTcpServer::incomingConnection(%i)", socketDescriptor);
#endif

    QTcpSocket *socket = new QTcpSocket(this);
    socket->setSocketDescriptor(socketDescriptor);
    d_func()->pendingConnections.append(socket);
}

/*!
    Sets the maximum number of pending accepted connections to \a
    numConnections. QTcpServer will accept no more than \a
    numConnections incoming connections before
    nextPendingConnection() is called. By default, the limit is 30
    pending connections.

    Clients may still able to connect after the server has reached
    its maximum number of pending connections (i.e., QTcpSocket can
    still emit the connected() signal). QTcpServer will stop
    accepting the new connections, but the operating system may
    still keep them in queue.

    \sa maxPendingConnections(), hasPendingConnections()
*/
void QTcpServer::setMaxPendingConnections(int numConnections)
{
    d_func()->maxConnections = numConnections;
}

/*!
    Returns the maximum number of pending accepted connections. The
    default is 30.

    \sa setMaxPendingConnections(), hasPendingConnections()
*/
int QTcpServer::maxPendingConnections() const
{
    return d_func()->maxConnections;
}

/*!
    Returns an error code for the last error that occurred.

    \sa errorString()
*/
QAbstractSocket::SocketError QTcpServer::serverError() const
{
    return d_func()->serverSocketError;
}

/*!
    Returns a human readable description of the last error that
    occurred.

    \sa serverError()
*/
QString QTcpServer::errorString() const
{
    return d_func()->serverSocketErrorString;
}

#ifndef QT_NO_NETWORKPROXY
/*!
    \since 4.1

    Sets the explicit network proxy for this socket to \a networkProxy.

    To disable the use of a proxy for this socket, use the
    QNetworkProxy::NoProxy proxy type:

    \snippet doc/src/snippets/code/src.network.socket.qtcpserver.cpp 0

    \sa proxy(), QNetworkProxy
*/
void QTcpServer::setProxy(const QNetworkProxy &networkProxy)
{
    Q_D(QTcpServer);
    if (!d->proxy)
        d->proxy = new QNetworkProxy();
    *d->proxy = networkProxy;
}

/*!
    \since 4.1

    Returns the network proxy for this socket.
    By default QNetworkProxy::DefaultProxy is used.

    \sa setProxy(), QNetworkProxy
*/
QNetworkProxy QTcpServer::proxy() const
{
    Q_D(const QTcpServer);
    if (d->proxy)
        return *d->proxy;
    return QNetworkProxy();
}
#endif // QT_NO_NETWORKPROXY

QT_END_NAMESPACE

#include "moc_qtcpserver.cpp"

