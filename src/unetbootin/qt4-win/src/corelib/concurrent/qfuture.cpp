/****************************************************************************
**
** Copyright (C) 2005-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtCore module of the Qt Toolkit.
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

/*! \class QFuture
    \threadsafe
    \brief The QFuture class represents the result of an asynchronous computation.
    \since 4.4

    To start a computation, use one of the APIs in the
    \l {threads.html#qtconcurrent-intro}{Qt Concurrent} framework.

    QFuture allows threads to be synchronized against one or more results
    which will be ready at a later point in time. The result can be of any type
    that has a default constructor and a copy constructor. If a result is not
    available at the time of calling the result(), resultAt(), or results()
    functions, QFuture will wait until the result becomes available. You can
    use the isResultReadyAt() function to determine if a result is ready or
    not. For QFuture objects that report more than one result, the
    resultCount() function returns the number of continuous results. This
    means that it is always safe to iterate through the results from 0 to 
    resultCount().

    QFuture provides a \l{Java-style iterators}{Java-style iterator}
    (QFutureIterator) and an \l{STL-style iterators}{STL-style iterator}
    (QFuture::const_iterator). Using these iterators is  another way to access
    results in the future.

    QFuture also offers ways to interact with a runnning computation. For
    instance, the computation can be canceled with the cancel() function. To
    pause the computation, use the setPaused() function or one of the pause(),
    resume(), or togglePaused() convenience functions. Be aware that not all
    asynchronous computations can be canceled or paused. For example, the
    future returned by QtConcurrent::run() cannot be canceled; but the
    future returned by QtConcurrent::mappedReduced() can.

    Progress information is provided by the progressValue(),
    progressMinimum(), progressMaximum(), and progressText() functions. The
    waitForFinished() function causes the calling thread to block and wait for
    the computation to finish, ensuring that all results are available.

    The state of the computation represented by a QFuture can be queried using
    the isCanceled(), isStarted(), isFinished(), isRunning(), or isPaused()
    functions.

    QFuture is a lightweight reference counted class that can be passed by
    value.

    QFuture<void> is specialized to not contain any of the result fetching
    functions. Any QFuture<T> can be assigned or copied into a QFuture<void>
    as well. This is useful if only status or progress information is needed
    - not the actual result data.

    To interact with running tasks using signals and slots, use QFutureWatcher.

    \sa QFutureWatcher, {threads.html#qtconcurrent-intro}{Qt Concurrent}
*/

/*! \fn QFuture::QFuture()

    Constructs an empty future.
*/

/*! \fn QFuture::QFuture(const QFuture &other)

    Constructs a copy of \a other.

    \sa operator=()
*/

/*! \fn QFuture::QFuture(QFutureInterface<T> *resultHolder)
    \internal
*/

/*! \fn QFuture::~QFuture()

    Destroys the future.

    Note that this neither waits nor cancels the asynchronous computation. Use
    waitForFinished() or QFutureSynchronizer when you need to ensure that the
    computation is completed before the future is destroyed.
*/

/*! \fn QFuture &QFuture::operator=(const QFuture &other)

     Assigns \a other to this future and returns a reference to this future.
*/

/*! \fn bool QFuture::operator==(const QFuture &other) const

    Returns true if \a other is a copy of this future; otherwise returns false.
*/

/*! \fn bool QFuture::operator!=(const QFuture &other) const

    Returns true if \a other is \e not a copy of this future; otherwise returns
    false.
*/

/*! \fn void QFuture::cancel()

    Cancels the asynchronous computation represented by this future. Note that
    the cancelation is asynchronous. Use waitForFinished() after calling
    cancel() when you need synchronous cancelation.

    Results currently available may still be accessed on a canceled future,
    but new results will \e not become available after calling this function.
    Any QFutureWatcher object that is watching this future will not deliver
    progress and result ready signals on a canceled future.

    Be aware that not all asynchronous computations can be canceled. For
    example, the future returned by QtConcurrent::run() cannot be canceled;
    but the future returned by QtConcurrent::mappedReduced() can.
*/

/*! \fn bool QFuture::isCanceled() const

    Returns true if the asynchronous computation has been canceled with the
    cancel() function; otherwise returns false.

    Be aware that the computation may still be running even though this
    function returns true. See cancel() for more details.
*/

/*! \fn void QFuture::setPaused(bool paused)

    If \a paused is true, this function pauses the asynchronous computation
    represented by the future. If the computation is  already paused, this
    function does nothing. Any QFutureWatcher object that is watching this
    future will stop delivering progress and result ready signals while the
    future is paused. Signal delivery will continue once the future is
    resumed.

    If \a paused is false, this function resumes the asynchronous computation.
    If the computation was not previously paused, this function does nothing.

    Be aware that not all computations can be paused. For example, the future
    returned by QtConcurrent::run() cannot be paused; but the future returned
    by QtConcurrent::mappedReduced() can.

    \sa pause(), resume(), togglePaused()
*/

/*! \fn bool QFuture::isPaused() const

    Returns true if the asynchronous computation has been paused with the
    pause() function; otherwise returns false.

    Be aware that the computation may still be running even though this
    function returns true. See setPaused() for more details.

    \sa setPaused(), togglePaused()
*/

/*! \fn void QFuture::pause()

    Pauses the asynchronous computation represented by this future. This is a
    convenience method that simply calls setPaused(true).

    \sa resume()
*/

/*! \fn void QFuture::resume()

    Resumes the asynchronous computation represented by this future. This is a
    convenience method that simply calls setPaused(false).

    \sa pause()
*/

/*! \fn void QFuture::togglePaused()

    Toggles the paused state of the asynchronous computation. In other words,
    if the computation is currently paused, calling this function resumes it;
    if the computation is running, it is paused. This is a convenience method
    for calling setPaused(!isPaused()).

    \sa setPaused(), pause(), resume()
*/

/*! \fn bool QFuture::isStarted() const

    Returns true if the asynchronous computation represented by this future
    has been started; otherwise returns false.
*/

/*! \fn bool QFuture::isFinished() const

    Returns true if the asynchronous computation represented by this future
    has finished; otherwise returns false.
*/

/*! \fn bool QFuture::isRunning() const

    Returns true if the asynchronous computation represented by this future is
    currently running; otherwise returns false.
*/

/*! \fn int QFuture::resultCount() const

    Returns the number of continuous results available in this future. The real
    number of results stored might be different from this value, due to gaps
    in the result set. It is always safe to iterate through the results from 0
    to resultCount().
    \sa result(), resultAt(), results()
*/

/*! \fn int QFuture::progressValue() const

    Returns the current progress value, which is between the progressMinimum()
    and progressMaximum().

    \sa progressMinimum(), progressMaximum()
*/

/*! \fn int QFuture::progressMinimum() const

    Returns the minimum progressValue().

    \sa progressValue(), progressMaximum()
*/

/*! \fn int QFuture::progressMaximum() const

    Returns the maximum progressValue().

    \sa progressValue(), progressMinimum()
*/

/*! \fn QString QFuture::progressText() const

    Returns the (optional) textual representation of the progress as reported
    by the asynchronous computation.

    Be aware that not all computations provide a textual representation of the
    progress, and as such, this function may return an empty string.
*/

/*! \fn void QFuture::waitForFinished()

    Waits for the asynchronous computation to finish (including cancel()ed
    computations).
*/

/*! \fn T QFuture::result() const

    Returns the first result in the future. If the result is not immediately
    available, this function will block and wait for the result to become
    available. This is a convenience method for calling resultAt(0).

    \sa resultAt(), results()
*/

/*! \fn T QFuture::resultAt(int index) const

    Returns the result at \a index in the future. If the result is not
    immediately available, this function will block and wait for the result to
    become available.

    \sa result(), results(), resultCount()
*/

/*! \fn bool QFuture::isResultReadyAt(int index) const

    Returns true if the result at \a index is immediately available; otherwise
    returns false.

    \sa resultAt(), resultCount()
*/

/*! \fn QFuture::operator T() const

    Returns the first result in the future. If the result is not immediately
    available, this function will block and wait for the result to become
    available. This is a convenience method for calling result() or
    resultAt(0).

    \sa result(), resultAt(), results()
*/

/*! \fn QList<T> QFuture::results() const

    Returns all results from the future. If the results are not immediately
    available, this function will block and wait for them to become available.

    \sa result(), resultAt(), resultCount()
*/

/*! \fn QFuture::const_iterator QFuture::begin() const

    Returns a const \l{STL-style iterator} pointing to the first result in the
    future.

    \sa constBegin(), end()
*/

/*! \fn QFuture::const_iterator QFuture::end() const

    Returns a const \l{STL-style iterator} pointing to the imaginary result
    after the last result in the future.

    \sa begin(), constEnd()
*/

/*! \fn QFuture::const_iterator QFuture::constBegin() const

    Returns a const \l{STL-style iterator} pointing to the first result in the
    future.

    \sa begin(), constEnd()
*/

/*! \fn QFuture::const_iterator QFuture::constEnd() const

    Returns a const \l{STL-style iterator} pointing to the imaginary result
    after the last result in the future.

    \sa constBegin(), end()
*/

/*! \class QFuture::const_iterator
    \reentrant
    \since 4.4

    \brief The QFuture::const_iterator class provides an STL-style const
    iterator for QFuture.

    QFuture provides both \l{STL-style iterators} and \l{Java-style iterators}.
    The STL-style iterators are more low-level and more cumbersome to use; on
    the other hand, they are slightly faster and, for developers who already
    know STL, have the advantage of familiarity.

    The default QFuture::const_iterator constructor creates an uninitialized
    iterator. You must initialize it using a QFuture function like
    QFuture::constBegin() or QFuture::constEnd() before you start iterating.
    Here's a typical loop that prints all the results available in a future:

    \snippet doc/src/snippets/code/src.corelib.concurrent.qfuture.cpp 0

    \sa QFutureIterator, QFuture
*/

/*! \typedef QFuture::const_iterator::iterator_category

    Typedef for std::bidirectional_iterator_tag. Provided for STL compatibility.
*/

/*! \typedef QFuture::const_iterator::difference_type

    Typedef for ptrdiff_t. Provided for STL compatibility.
*/

/*! \typedef QFuture::const_iterator::value_type

    Typedef for T. Provided for STL compatibility.
*/

/*! \typedef QFuture::const_iterator::pointer

    Typedef for const T *. Provided for STL compatibility.
*/

/*! \typedef QFuture::const_iterator::reference

    Typedef for const T &. Provided for STL compatibility.
*/

/*! \fn QFuture::const_iterator::const_iterator()

    Constructs an uninitialized iterator.

    Functions like operator*() and operator++() should not be called on an
    uninitialized iterartor. Use operator=() to assign a value to it before
    using it.

    \sa QFuture::constBegin() QFuture::constEnd()
*/

/*! \fn QFuture::const_iterator::const_iterator(QFuture const * const future, int index)
    \internal
*/

/*! \fn QFuture::const_iterator::const_iterator(const const_iterator &other)

    Constructs a copy of \a other.
*/

/*! \fn QFuture::const_iterator &QFuture::const_iterator::operator=(const const_iterator &other)

    Assigns \a other to this iterator.
*/

/*! \fn const T &QFuture::const_iterator::operator*() const

    Returns the current result.
*/

/*! \fn const T *QFuture::const_iterator::operator->() const

    Returns a pointer to the current result.
*/

/*! \fn bool QFuture::const_iterator::operator!=(const const_iterator &other) const

    Returns true if \a other points to a different result than this iterator;
    otherwise returns false.

    \sa operator==()
*/

/*! \fn bool QFuture::const_iterator::operator==(const const_iterator &other) const

    Returns true if \a other points to the same result as this iterator;
    otherwise returns false.

    \sa operator!=()
*/

/*! \fn QFuture::const_iterator &QFuture::const_iterator::operator++()

    The prefix ++ operator (\c{++it}) advances the iterator to the next result
    in the future and returns an iterator to the new current result.

    Calling this function on QFuture::constEnd() leads to undefined results.

    \sa operator--()
*/

/*! \fn QFuture::const_iterator QFuture::const_iterator::operator++(int)

    \overload

    The postfix ++ operator (\c{it++}) advances the iterator to the next
    result in the future and returns an iterator to the previously current
    result.
*/

/*! \fn QFuture::const_iterator &QFuture::const_iterator::operator--()

    The prefix -- operator (\c{--it}) makes the preceding result current and
    returns an iterator to the new current result.

    Calling this function on QFuture::constBegin() leads to undefined results.

    \sa operator++()
*/

/*! \fn QFuture::const_iterator QFuture::const_iterator::operator--(int)

    \overload

    The postfix -- operator (\c{it--}) makes the preceding result current and
    returns an iterator to the previously current result.
*/

/*! \fn QFuture::const_iterator &QFuture::const_iterator::operator+=(int j)

    Advances the iterator by \a j results. (If \a j is negative, the iterator
    goes backward.)

    \sa operator-=(), operator+()
*/

/*! \fn QFuture::const_iterator &QFuture::const_iterator::operator-=(int j)

    Makes the iterator go back by \a j results. (If \a j is negative, the
    iterator goes forward.)

    \sa operator+=(), operator-()
*/

/*! \fn QFuture::const_iterator QFuture::const_iterator::operator+(int j) const

    Returns an iterator to the results at \a j positions forward from this
    iterator. (If \a j is negative, the iterator goes backward.)

    \sa operator-(), operator+=()
*/

/*! \fn QFuture::const_iterator QFuture::const_iterator::operator-(int j) const

    Returns an iterator to the result at \a j positions backward from this
    iterator. (If \a j is negative, the iterator goes forward.)

    \sa operator+(), operator-=()
*/

/*! \typedef QFuture::ConstIterator

    Qt-style synonym for QFuture::const_iterator.
*/

/*! \class QFutureIterator
    \reentrant
    \since 4.4

    \brief The QFutureIterator class provides a Java-style const iterator for
    QFuture.

    QFuture has both \l{Java-style iterators} and \l{STL-style iterators}. The
    Java-style iterators are more high-level and easier to use than the
    STL-style iterators; on the other hand, they are slightly less efficient.

    An alternative to using iterators is to use index positions. Some QFuture
    member functions take an index as their first parameter, making it
    possible to access results without using iterators.

    QFutureIterator\<T\> allows you to iterate over a QFuture\<T\>. Note that
    there is no mutable iterator for QFuture (unlike the other Java-style
    iterators).

    The QFutureIterator constructor takes a QFuture as its argument. After
    construction, the iterator is located at the very beginning of the result
    list (i.e. before the first result). Here's how to iterate over all the
    results sequentially:

    \snippet doc/src/snippets/code/src.corelib.concurrent.qfuture.cpp 1

    The next() function returns the next result (waiting for it to become
    available, if necessary) from the future and advances the iterator. Unlike
    STL-style iterators, Java-style iterators point \e between results rather
    than directly \e at results. The first call to next() advances the iterator
    to the position between the first and second result, and returns the first
    result; the second call to next() advances the iterator to the position
    between the second and third result, and returns the second result; and
    so on.

    \img javaiterators1.png

    Here's how to iterate over the elements in reverse order:

    \snippet doc/src/snippets/code/src.corelib.concurrent.qfuture.cpp 2

    If you want to find all occurrences of a particular value, use findNext()
    or findPrevious() in a loop.

    Multiple iterators can be used on the same future. If the future is
    modified while a QFutureIterator is active, the QFutureIterator will
    continue iterating over the original future, ignoring the modified copy.

    \sa QFuture::const_iterator, QFuture
*/

/*!
    \fn QFutureIterator::QFutureIterator(const QFuture<T> &future)

    Constructs an iterator for traversing \a future. The iterator is set to be
    at the front of the result list (before the first result).

    \sa operator=()
*/

/*! \fn QFutureIterator &QFutureIterator::operator=(const QFuture<T> &future)

    Makes the iterator operate on \a future. The iterator is set to be at the
    front of the result list (before the first result).

    \sa toFront(), toBack()
*/

/*! \fn void QFutureIterator::toFront()

    Moves the iterator to the front of the result list (before the first
    result).

    \sa toBack(), next()
*/

/*! \fn void QFutureIterator::toBack()

    Moves the iterator to the back of the result list (after the last result).

    \sa toFront(), previous()
*/

/*! \fn bool QFutureIterator::hasNext() const

    Returns true if there is at least one result ahead of the iterator, e.g.,
    the iterator is \e not at the back of the result list; otherwise returns
    false.

    \sa hasPrevious(), next()
*/

/*! \fn const T &QFutureIterator::next()

    Returns the next result and advances the iterator by one position.

    Calling this function on an iterator located at the back of the result
    list leads to undefined results.

    \sa hasNext(), peekNext(), previous()
*/

/*! \fn const T &QFutureIterator::peekNext() const

    Returns the next result without moving the iterator.

    Calling this function on an iterator located at the back of the result
    list leads to undefined results.

    \sa hasNext(), next(), peekPrevious()
*/

/*! \fn bool QFutureIterator::hasPrevious() const

    Returns true if there is at least one result ahead of the iterator, e.g.,
    the iterator is \e not at the front of the result list; otherwise returns
    false.

    \sa hasNext(), previous()
*/

/*! \fn const T &QFutureIterator::previous()

    Returns the previous result and moves the iterator back by one position.

    Calling this function on an iterator located at the front of the result
    list leads to undefined results.

    \sa hasPrevious(), peekPrevious(), next()
*/

/*! \fn const T &QFutureIterator::peekPrevious() const

    Returns the previous result without moving the iterator.

    Calling this function on an iterator located at the front of the result
    list leads to undefined results.

    \sa hasPrevious(), previous(), peekNext()
*/

/*! \fn bool QFutureIterator::findNext(const T &value)

    Searches for \a value starting from the current iterator position forward.
    Returns true if \a value is found; otherwise returns false.

    After the call, if \a value was found, the iterator is positioned just
    after the matching result; otherwise, the iterator is positioned at the
    back of the result list.

    \sa findPrevious()
*/

/*! \fn bool QFutureIterator::findPrevious(const T &value)

    Searches for \a value starting from the current iterator position
    backward. Returns true if \a value is found; otherwise returns false.

    After the call, if \a value was found, the iterator is positioned just
    before the matching result; otherwise, the iterator is positioned at the
    front of the result list.

    \sa findNext()
*/
