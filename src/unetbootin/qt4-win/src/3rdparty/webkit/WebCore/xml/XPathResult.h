/*
 * Copyright 2005 Frerich Raabe <raabe@kde.org>
 * Copyright (C) 2006 Apple Computer, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef XPathResult_h
#define XPathResult_h

#if ENABLE(XPATH)

#include "Shared.h"
#include "XPathValue.h"

namespace WebCore {

    typedef int ExceptionCode;

    class EventListener;
    class EventTargetNode;
    class Node;
    class String;

    class XPathResult : public Shared<XPathResult> {
    public:
        enum XPathResultType {
            ANY_TYPE = 0,
            NUMBER_TYPE = 1,
            STRING_TYPE = 2,
            BOOLEAN_TYPE = 3,
            UNORDERED_NODE_ITERATOR_TYPE = 4,
            ORDERED_NODE_ITERATOR_TYPE = 5,
            UNORDERED_NODE_SNAPSHOT_TYPE = 6,
            ORDERED_NODE_SNAPSHOT_TYPE = 7,
            ANY_UNORDERED_NODE_TYPE = 8,
            FIRST_ORDERED_NODE_TYPE = 9
        };
        
        XPathResult(EventTargetNode*, const XPath::Value&);
        ~XPathResult();
        
        void convertTo(unsigned short type, ExceptionCode&);

        unsigned short resultType() const;

        double numberValue(ExceptionCode&) const;
        String stringValue(ExceptionCode&) const;
        bool booleanValue(ExceptionCode&) const;
        Node* singleNodeValue(ExceptionCode&) const;

        bool invalidIteratorState() const;
        unsigned long snapshotLength(ExceptionCode&) const;
        Node* iterateNext(ExceptionCode&);
        Node* snapshotItem(unsigned long index, ExceptionCode&);

        void invalidateIteratorState();

    private:
        XPath::Value m_value;
        unsigned m_nodeSetPosition;
        XPath::NodeSet m_nodeSet; // FIXME: why duplicate the node set stored in m_value?
        unsigned short m_resultType;
        bool m_invalidIteratorState;
        RefPtr<EventTargetNode> m_eventTarget;
        RefPtr<EventListener> m_eventListener;
    };

}

#endif // ENABLE(XPATH)

#endif // XPathResult_h
