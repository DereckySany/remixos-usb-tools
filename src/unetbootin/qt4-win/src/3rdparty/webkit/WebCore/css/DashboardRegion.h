/*
 * This file is part of the DOM implementation for KDE.
 *
 * (C) 1999-2003 Lars Knoll (knoll@kde.org)
 * Copyright (C) 2004, 2005, 2006 Apple Computer, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef DashboardRegion_h
#define DashboardRegion_h

#include "Rect.h"

namespace WebCore {

class DashboardRegion : public Rect {
public:
    DashboardRegion() : m_isCircle(0), m_isRectangle(0) { }

    RefPtr<DashboardRegion> m_next;
    String m_label;
    String m_geometryType;
    bool m_isCircle : 1;
    bool m_isRectangle : 1;
};

} // namespace

#endif
