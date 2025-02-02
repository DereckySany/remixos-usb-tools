// -*- mode: c++; c-basic-offset: 4 -*-
/*
 *  This file is part of the KDE libraries
 *  Copyright (C) 2005, 2006 Apple Computer, Inc.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 *
 */

#ifndef WTF_Vector_h
#define WTF_Vector_h

#include "Assertions.h"
#include "FastMalloc.h"
#include "VectorTraits.h"
#include <limits>
#include <stdlib.h>
#include <string.h>
#include <utility>

namespace WTF {

    using std::min;
    using std::max;
    
    template <bool needsDestruction, typename T>
    class VectorDestructor;

    template<typename T>
    struct VectorDestructor<false, T>
    {
        static void destruct(T*, T*) {}
    };

    template<typename T>
    struct VectorDestructor<true, T>
    {
        static void destruct(T* begin, T* end) 
        {
            for (T* cur = begin; cur != end; ++cur)
                cur->~T();
        }
    };

    template <bool needsInitialization, bool canInitializeWithMemset, typename T>
    class VectorInitializer;

    template<bool ignore, typename T>
    struct VectorInitializer<false, ignore, T>
    {
        static void initialize(T*, T*) {}
    };

    template<typename T>
    struct VectorInitializer<true, false, T>
    {
        static void initialize(T* begin, T* end) 
        {
            for (T* cur = begin; cur != end; ++cur)
                new (cur) T;
        }
    };

    template<typename T>
    struct VectorInitializer<true, true, T>
    {
        static void initialize(T* begin, T* end) 
        {
            memset(begin, 0, reinterpret_cast<char*>(end) - reinterpret_cast<char*>(begin));
        }
    };

    template <bool canMoveWithMemcpy, typename T>
    class VectorMover;

    template<typename T>
    struct VectorMover<false, T>
    {
        static void move(const T* src, const T* srcEnd, T* dst)
        {
            while (src != srcEnd) {
                new (dst) T(*src);
                src->~T();
                ++dst;
                ++src;
            }
        }
        static void moveOverlapping(const T* src, const T* srcEnd, T* dst)
        {
            if (src > dst)
                move(src, srcEnd, dst);
            else {
                T* dstEnd = dst + (srcEnd - src);
                while (src != srcEnd) {
                    --srcEnd;
                    --dstEnd;
                    new (dstEnd) T(*srcEnd);
                    srcEnd->~T();
                }
            }
        }
    };

    template<typename T>
    struct VectorMover<true, T>
    {
        static void move(const T* src, const T* srcEnd, T* dst) 
        {
            memcpy(dst, src, reinterpret_cast<const char*>(srcEnd) - reinterpret_cast<const char*>(src));
        }
        static void moveOverlapping(const T* src, const T* srcEnd, T* dst) 
        {
            memmove(dst, src, reinterpret_cast<const char*>(srcEnd) - reinterpret_cast<const char*>(src));
        }
    };

    template <bool canCopyWithMemcpy, typename T>
    class VectorCopier;

    template<typename T>
    struct VectorCopier<false, T>
    {
        static void uninitializedCopy(const T* src, const T* srcEnd, T* dst) 
        {
            while (src != srcEnd) {
                new (dst) T(*src);
                ++dst;
                ++src;
            }
        }
    };

    template<typename T>
    struct VectorCopier<true, T>
    {
        static void uninitializedCopy(const T* src, const T* srcEnd, T* dst) 
        {
            memcpy(dst, src, reinterpret_cast<const char*>(srcEnd) - reinterpret_cast<const char*>(src));
        }
    };

    template <bool canFillWithMemset, typename T>
    class VectorFiller;

    template<typename T>
    struct VectorFiller<false, T>
    {
        static void uninitializedFill(T* dst, T* dstEnd, const T& val) 
        {
            while (dst != dstEnd) {
                new (dst) T(val);
                ++dst;
            }
        }
    };

    template<typename T>
    struct VectorFiller<true, T>
    {
        static void uninitializedFill(T* dst, T* dstEnd, const T& val) 
        {
            ASSERT(sizeof(T) == sizeof(char));
            memset(dst, val, dstEnd - dst);
        }
    };
    
    template<bool canCompareWithMemcmp, typename T>
    class VectorComparer;
    
    template<typename T>
    struct VectorComparer<false, T>
    {
        static bool compare(const T* a, const T* b, size_t size)
        {
            for (size_t i = 0; i < size; ++i)
                if (a[i] != b[i])
                    return false;
            return true;
        }
    };

    template<typename T>
    struct VectorComparer<true, T>
    {
        static bool compare(const T* a, const T* b, size_t size)
        {
            return memcmp(a, b, sizeof(T) * size) == 0;
        }
    };
    
    template<typename T>
    struct VectorTypeOperations
    {
        static void destruct(T* begin, T* end)
        {
            VectorDestructor<VectorTraits<T>::needsDestruction, T>::destruct(begin, end);
        }

        static void initialize(T* begin, T* end)
        {
            VectorInitializer<VectorTraits<T>::needsInitialization, VectorTraits<T>::canInitializeWithMemset, T>::initialize(begin, end);
        }

        static void move(const T* src, const T* srcEnd, T* dst)
        {
            VectorMover<VectorTraits<T>::canMoveWithMemcpy, T>::move(src, srcEnd, dst);
        }

        static void moveOverlapping(const T* src, const T* srcEnd, T* dst)
        {
            VectorMover<VectorTraits<T>::canMoveWithMemcpy, T>::moveOverlapping(src, srcEnd, dst);
        }

        static void uninitializedCopy(const T* src, const T* srcEnd, T* dst)
        {
            VectorCopier<VectorTraits<T>::canCopyWithMemcpy, T>::uninitializedCopy(src, srcEnd, dst);
        }

        static void uninitializedFill(T* dst, T* dstEnd, const T& val)
        {
            VectorFiller<VectorTraits<T>::canFillWithMemset, T>::uninitializedFill(dst, dstEnd, val);
        }
        
        static bool compare(const T* a, const T* b, size_t size)
        {
            return VectorComparer<VectorTraits<T>::canCompareWithMemcmp, T>::compare(a, b, size);
        }
    };

    template<typename T, size_t inlineCapacity>
    class VectorBuffer;

    template<typename T>
    class VectorBuffer<T, 0> {
    public:
        VectorBuffer()
            : m_buffer(0), m_capacity(0)
        {
        }
        
        VectorBuffer(size_t capacity)
#if !ASSERT_DISABLED
            : m_capacity(0)
#endif
        {
            allocateBuffer(capacity);
        }

        ~VectorBuffer()
        {
            deallocateBuffer(m_buffer);
        }
        
        static void deallocateBuffer(T* buffer)
        {
            fastFree(buffer);
        }
        
        void allocateBuffer(size_t newCapacity)
        {
            ASSERT(newCapacity >= m_capacity);
            m_capacity = newCapacity;
            if (newCapacity > std::numeric_limits<size_t>::max() / sizeof(T))
                abort();
            m_buffer = static_cast<T*>(fastMalloc(newCapacity * sizeof(T)));
        }

        T* buffer() { return m_buffer; }
        const T* buffer() const { return m_buffer; }
        size_t capacity() const { return m_capacity; }

        void swap(VectorBuffer<T, 0>& other)
        {
            std::swap(m_capacity, other.m_capacity);
            std::swap(m_buffer, other.m_buffer);
        }

        T* releaseBuffer()
        {
            T* buffer = m_buffer;
            m_buffer = 0;
            m_capacity = 0;
            return buffer;
        }

    protected:
        VectorBuffer(T* buffer, size_t capacity)
            : m_buffer(buffer), m_capacity(capacity)
        {
        }

        T* m_buffer;

    private:
        size_t m_capacity;
    };

    template<typename T, size_t inlineCapacity>
    class VectorBuffer : private VectorBuffer<T, 0> {
    private:
        typedef VectorBuffer<T, 0> BaseBuffer;
    public:
        VectorBuffer()
            : BaseBuffer(inlineBuffer(), inlineCapacity)
        {
        }

        VectorBuffer(size_t capacity)
            : BaseBuffer(inlineBuffer(), inlineCapacity)
        {
            if (capacity > inlineCapacity)
                allocateBuffer(capacity);
        }

        ~VectorBuffer()
        {
            if (buffer() == inlineBuffer())
                BaseBuffer::m_buffer = 0;
        }

        void deallocateBuffer(T* buffer)
        {
            if (buffer != inlineBuffer())
                BaseBuffer::deallocateBuffer(buffer);
        }

        using BaseBuffer::allocateBuffer;

        using BaseBuffer::buffer;
        using BaseBuffer::capacity;

        T* releaseBuffer()
        {
            if (buffer() == inlineBuffer())
                return 0;
            return BaseBuffer::releaseBuffer();
        }

        void swap(VectorBuffer<T, inlineCapacity>&);

    private:
        static const size_t m_inlineBufferSize = inlineCapacity * sizeof(T);
        T* inlineBuffer() { return reinterpret_cast<T*>(&m_inlineBuffer); }

        // FIXME: Nothing guarantees this buffer is appropriately aligned to hold objects of type T.
        char m_inlineBuffer[m_inlineBufferSize];
    };

    template<typename T, size_t inlineCapacity = 0>
    class Vector {
    private:
        typedef VectorBuffer<T, inlineCapacity> Impl;
        typedef VectorTypeOperations<T> TypeOperations;

    public:
        typedef T* iterator;
        typedef const T* const_iterator;

        Vector() 
            : m_size(0)
        {
        }
        
        explicit Vector(size_t size) 
            : m_size(0)
        {
            resize(size);
        }

        ~Vector()
        {
            clear();
        }

        Vector(const Vector&);
        template<size_t otherCapacity> 
        Vector(const Vector<T, otherCapacity>&);

        Vector& operator=(const Vector&);
        template<size_t otherCapacity> 
        Vector& operator=(const Vector<T, otherCapacity>&);

        size_t size() const { return m_size; }
        size_t capacity() const { return m_impl.capacity(); }
        bool isEmpty() const { return !size(); }

        T& at(size_t i) 
        { 
            ASSERT(i < size());
            return m_impl.buffer()[i]; 
        }
        const T& at(size_t i) const 
        {
            ASSERT(i < size());
            return m_impl.buffer()[i]; 
        }

        T& operator[](size_t i) { return at(i); }
        const T& operator[](size_t i) const { return at(i); }

        T* data() { return m_impl.buffer(); }
        const T* data() const { return m_impl.buffer(); }

        iterator begin() { return data(); }
        iterator end() { return begin() + m_size; }
        const_iterator begin() const { return data(); }
        const_iterator end() const { return begin() + m_size; }
        
        T& first() { return at(0); }
        const T& first() const { return at(0); }
        T& last() { return at(size() - 1); }
        const T& last() const { return at(size() - 1); }

        void resize(size_t size);
        void reserveCapacity(size_t newCapacity);

        void clear() { resize(0); }

        template<typename U> void append(const U*, size_t);
        template<typename U> void append(const U&);
        template<typename U, size_t c> void append(const Vector<U, c>&);

        template<typename U> void insert(size_t position, const U*, size_t);
        template<typename U> void insert(size_t position, const U&);
        template<typename U, size_t c> void insert(size_t position, const Vector<U, c>&);

        template<typename U> void prepend(const U*, size_t);
        template<typename U> void prepend(const U&);
        template<typename U, size_t c> void prepend(const Vector<U, c>&);

        void remove(size_t position);

        void removeLast() 
        {
            ASSERT(!isEmpty());
            resize(size() - 1); 
        }

        Vector(size_t size, const T& val)
            : m_size(size)
            , m_impl(size)
        {
            TypeOperations::uninitializedFill(begin(), end(), val);
        }

        void fill(const T&, size_t);
        void fill(const T& val) { fill(val, size()); }

        template<typename Iterator> void appendRange(Iterator start, Iterator end);

        T* releaseBuffer();

        void swap(Vector<T, inlineCapacity>& other)
        {
            std::swap(m_size, other.m_size);
            m_impl.swap(other.m_impl);
        }

    private:
        void expandCapacity(size_t newMinCapacity);
        const T* expandCapacity(size_t newMinCapacity, const T*);
        template<typename U> U* expandCapacity(size_t newMinCapacity, U*); 

        size_t m_size;
        Impl m_impl;
    };

    template<typename T, size_t inlineCapacity>
    Vector<T, inlineCapacity>::Vector(const Vector& other)
        : m_size(other.size())
        , m_impl(other.capacity())
    {
        TypeOperations::uninitializedCopy(other.begin(), other.end(), begin());
    }

    template<typename T, size_t inlineCapacity>
    template<size_t otherCapacity> 
    Vector<T, inlineCapacity>::Vector(const Vector<T, otherCapacity>& other)
        : m_size(other.size())
        , m_impl(other.capacity())
    {
        TypeOperations::uninitializedCopy(other.begin(), other.end(), begin());
    }

    template<typename T, size_t inlineCapacity>
    Vector<T, inlineCapacity>& Vector<T, inlineCapacity>::operator=(const Vector<T, inlineCapacity>& other)
    {
        if (&other == this)
            return *this;
        
        if (size() > other.size())
            resize(other.size());
        else if (other.size() > capacity()) {
            clear();
            reserveCapacity(other.size());
        }
        
        std::copy(other.begin(), other.begin() + size(), begin());
        TypeOperations::uninitializedCopy(other.begin() + size(), other.end(), end());
        m_size = other.size();

        return *this;
    }

    template<typename T, size_t inlineCapacity>
    template<size_t otherCapacity> 
    Vector<T, inlineCapacity>& Vector<T, inlineCapacity>::operator=(const Vector<T, otherCapacity>& other)
    {
        if (&other == this)
            return *this;
        
        if (size() > other.size())
            resize(other.size());
        else if (other.size() > capacity()) {
            clear();
            reserveCapacity(other.size());
        }
        
        std::copy(other.begin(), other.begin() + size(), begin());
        TypeOperations::uninitializedCopy(other.begin() + size(), other.end(), end());
        m_size = other.size();

        return *this;
    }

    template<typename T, size_t inlineCapacity>
    void Vector<T, inlineCapacity>::fill(const T& val, size_t newSize)
    {
        if (size() > newSize)
            resize(newSize);
        else if (newSize > capacity()) {
            clear();
            reserveCapacity(newSize);
        }
        
        std::fill(begin(), end(), val);
        TypeOperations::uninitializedFill(end(), begin() + newSize, val);
        m_size = newSize;
    }

    template<typename T, size_t inlineCapacity>
    template<typename Iterator>
    void Vector<T, inlineCapacity>::appendRange(Iterator start, Iterator end)
    {
        for (Iterator it = start; it != end; ++it)
            append(*it);
    }

    template<typename T, size_t inlineCapacity>
    void Vector<T, inlineCapacity>::expandCapacity(size_t newMinCapacity)
    {
        reserveCapacity(max(newMinCapacity, max(static_cast<size_t>(16), capacity() + capacity() / 4 + 1)));
    }
    
    template<typename T, size_t inlineCapacity>
    const T* Vector<T, inlineCapacity>::expandCapacity(size_t newMinCapacity, const T* ptr)
    {
        if (ptr < begin() || ptr >= end()) {
            expandCapacity(newMinCapacity);
            return ptr;
        }
        size_t index = ptr - begin();
        expandCapacity(newMinCapacity);
        return begin() + index;
    }

    template<typename T, size_t inlineCapacity> template<typename U>
    inline U* Vector<T, inlineCapacity>::expandCapacity(size_t newMinCapacity, U* ptr)
    {
        expandCapacity(newMinCapacity);
        return ptr;
    }

    template<typename T, size_t inlineCapacity>
    void Vector<T, inlineCapacity>::resize(size_t size)
    {
        if (size <= m_size)
            TypeOperations::destruct(begin() + size, end());
        else {
            if (size > capacity())
                expandCapacity(size);
            TypeOperations::initialize(end(), begin() + size);
        }
        
        m_size = size;
    }

    template<typename T, size_t inlineCapacity>
    void Vector<T, inlineCapacity>::reserveCapacity(size_t newCapacity)
    {
        if (newCapacity < capacity())
            return;
        T* oldBuffer = begin();
        T* oldEnd = end();
        m_impl.allocateBuffer(newCapacity);
        TypeOperations::move(oldBuffer, oldEnd, begin());
        m_impl.deallocateBuffer(oldBuffer);
    }

    // Templatizing these is better than just letting the conversion happen implicitly,
    // because for instance it allows a PassRefPtr to be appended to a RefPtr vector
    // without refcount thrash.

    template<typename T, size_t inlineCapacity> template<typename U>
    void Vector<T, inlineCapacity>::append(const U* data, size_t dataSize)
    {
        size_t newSize = m_size + dataSize;
        if (newSize > capacity())
            data = expandCapacity(newSize, data);
        T* dest = end();
        for (size_t i = 0; i < dataSize; ++i)
            new (&dest[i]) T(data[i]);
        m_size = newSize;
    }

    template<typename T, size_t inlineCapacity> template<typename U>
    inline void Vector<T, inlineCapacity>::append(const U& val)
    {
        const U* ptr = &val;
        if (size() == capacity())
            ptr = expandCapacity(size() + 1, ptr);
        new (end()) T(*ptr);
        ++m_size;
    }

    template<typename T, size_t inlineCapacity> template<typename U, size_t c>
    inline void Vector<T, inlineCapacity>::append(const Vector<U, c>& val)
    {
        append(val.begin(), val.size());
    }

    template<typename T, size_t inlineCapacity> template<typename U>
    void Vector<T, inlineCapacity>::insert(size_t position, const U* data, size_t dataSize)
    {
        ASSERT(position <= size());
        size_t newSize = m_size + dataSize;
        if (newSize > capacity())
            data = expandCapacity(newSize, data);
        T* spot = begin() + position;
        TypeOperations::moveOverlapping(spot, end(), spot + dataSize);
        for (size_t i = 0; i < dataSize; ++i)
            new (&spot[i]) T(data[i]);
        m_size = newSize;
    }
     
    template<typename T, size_t inlineCapacity> template<typename U>
    inline void Vector<T, inlineCapacity>::insert(size_t position, const U& val)
    {
        ASSERT(position <= size());
        const U* data = &val;
        if (size() == capacity())
            data = expandCapacity(size() + 1, data);
        T* spot = begin() + position;
        TypeOperations::moveOverlapping(spot, end(), spot + 1);
        new (spot) T(*data);
        ++m_size;
    }
   
    template<typename T, size_t inlineCapacity> template<typename U, size_t c>
    inline void Vector<T, inlineCapacity>::insert(size_t position, const Vector<U, c>& val)
    {
        insert(position, val.begin(), val.size());
    }

    template<typename T, size_t inlineCapacity> template<typename U>
    void Vector<T, inlineCapacity>::prepend(const U* data, size_t dataSize)
    {
        insert(0, data, dataSize);
    }

    template<typename T, size_t inlineCapacity> template<typename U>
    inline void Vector<T, inlineCapacity>::prepend(const U& val)
    {
        insert(0, val);
    }
   
    template<typename T, size_t inlineCapacity> template<typename U, size_t c>
    inline void Vector<T, inlineCapacity>::prepend(const Vector<U, c>& val)
    {
        insert(0, val.begin(), val.size());
    }
    
    template<typename T, size_t inlineCapacity>
    inline void Vector<T, inlineCapacity>::remove(size_t position)
    {
        ASSERT(position < size());
        T* spot = begin() + position;
        spot->~T();
        TypeOperations::moveOverlapping(spot + 1, end(), spot);
        --m_size;
    }

    template<typename T, size_t inlineCapacity>
    T* Vector<T, inlineCapacity>::releaseBuffer()
    {
        T* buffer = m_impl.releaseBuffer();
        if (!buffer && m_size) {
            // If the vector had some data, but no buffer to release,
            // that means it was using the inline buffer. In that case,
            // we create a brand new buffer so the caller always gets one.
            size_t bytes = m_size * sizeof(T);
            buffer = static_cast<T*>(fastMalloc(bytes));
            memcpy(buffer, data(), bytes);
        }
        m_size = 0;
        return buffer;
    }

    template<typename T, size_t inlineCapacity>
    void deleteAllValues(const Vector<T, inlineCapacity>& collection)
    {
        typedef typename Vector<T, inlineCapacity>::const_iterator iterator;
        iterator end = collection.end();
        for (iterator it = collection.begin(); it != end; ++it)
            delete *it;
    }

    template<typename T, size_t inlineCapacity>
    inline void swap(Vector<T, inlineCapacity>& a, Vector<T, inlineCapacity>& b)
    {
        a.swap(b);
    }

    template<typename T, size_t inlineCapacity>
    bool operator==(const Vector<T, inlineCapacity>& a, const Vector<T, inlineCapacity>& b)
    {
        if (a.size() != b.size())
            return false;

        return VectorTypeOperations<T>::compare(a.data(), b.data(), a.size());
    }

    template<typename T, size_t inlineCapacity>
    inline bool operator!=(const Vector<T, inlineCapacity>& a, const Vector<T, inlineCapacity>& b)
    {
        return !(a == b);
    }


} // namespace WTF

using WTF::Vector;

#endif // WTF_Vector_h
