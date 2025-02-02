/*
    This file is part of the WebKit open source project.
    This file has been generated by generate-bindings.pl. DO NOT MODIFY!

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

#include "config.h"

#include "JSCSSValueList.h"

#include <wtf/GetPtr.h>

#include <runtime/PropertyNameArray.h>
#include "CSSValue.h"
#include "CSSValueList.h"
#include "JSCSSValue.h"

#include <runtime/Error.h>
#include <runtime/JSNumberCell.h>

using namespace JSC;

namespace WebCore {

ASSERT_CLASS_FITS_IN_CELL(JSCSSValueList)

/* Hash table */

static const HashTableValue JSCSSValueListTableValues[3] =
{
    { "length", DontDelete|ReadOnly, (intptr_t)jsCSSValueListLength, (intptr_t)0 },
    { "constructor", DontEnum|ReadOnly, (intptr_t)jsCSSValueListConstructor, (intptr_t)0 },
    { 0, 0, 0, 0 }
};

static const HashTable JSCSSValueListTable =
#if ENABLE(PERFECT_HASH_SIZE)
    { 15, JSCSSValueListTableValues, 0 };
#else
    { 5, 3, JSCSSValueListTableValues, 0 };
#endif

/* Hash table for constructor */

static const HashTableValue JSCSSValueListConstructorTableValues[1] =
{
    { 0, 0, 0, 0 }
};

static const HashTable JSCSSValueListConstructorTable =
#if ENABLE(PERFECT_HASH_SIZE)
    { 0, JSCSSValueListConstructorTableValues, 0 };
#else
    { 1, 0, JSCSSValueListConstructorTableValues, 0 };
#endif

class JSCSSValueListConstructor : public DOMObject {
public:
    JSCSSValueListConstructor(ExecState* exec)
        : DOMObject(JSCSSValueListConstructor::createStructure(exec->lexicalGlobalObject()->objectPrototype()))
    {
        putDirect(exec->propertyNames().prototype, JSCSSValueListPrototype::self(exec), None);
    }
    virtual bool getOwnPropertySlot(ExecState*, const Identifier&, PropertySlot&);
    virtual const ClassInfo* classInfo() const { return &s_info; }
    static const ClassInfo s_info;

    static PassRefPtr<Structure> createStructure(JSValuePtr proto) 
    { 
        return Structure::create(proto, TypeInfo(ObjectType, ImplementsHasInstance)); 
    }
};

const ClassInfo JSCSSValueListConstructor::s_info = { "CSSValueListConstructor", 0, &JSCSSValueListConstructorTable, 0 };

bool JSCSSValueListConstructor::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSCSSValueListConstructor, DOMObject>(exec, &JSCSSValueListConstructorTable, this, propertyName, slot);
}

/* Hash table for prototype */

static const HashTableValue JSCSSValueListPrototypeTableValues[2] =
{
    { "item", DontDelete|Function, (intptr_t)jsCSSValueListPrototypeFunctionItem, (intptr_t)1 },
    { 0, 0, 0, 0 }
};

static const HashTable JSCSSValueListPrototypeTable =
#if ENABLE(PERFECT_HASH_SIZE)
    { 0, JSCSSValueListPrototypeTableValues, 0 };
#else
    { 2, 1, JSCSSValueListPrototypeTableValues, 0 };
#endif

const ClassInfo JSCSSValueListPrototype::s_info = { "CSSValueListPrototype", 0, &JSCSSValueListPrototypeTable, 0 };

JSObject* JSCSSValueListPrototype::self(ExecState* exec)
{
    return getDOMPrototype<JSCSSValueList>(exec);
}

bool JSCSSValueListPrototype::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    return getStaticFunctionSlot<JSObject>(exec, &JSCSSValueListPrototypeTable, this, propertyName, slot);
}

const ClassInfo JSCSSValueList::s_info = { "CSSValueList", &JSCSSValue::s_info, &JSCSSValueListTable, 0 };

JSCSSValueList::JSCSSValueList(PassRefPtr<Structure> structure, PassRefPtr<CSSValueList> impl)
    : JSCSSValue(structure, impl)
{
}

JSObject* JSCSSValueList::createPrototype(ExecState* exec)
{
    return new (exec) JSCSSValueListPrototype(JSCSSValueListPrototype::createStructure(JSCSSValuePrototype::self(exec)));
}

bool JSCSSValueList::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    const HashEntry* entry = JSCSSValueListTable.entry(exec, propertyName);
    if (entry) {
        slot.setCustom(this, entry->propertyGetter());
        return true;
    }
    bool ok;
    unsigned index = propertyName.toUInt32(&ok, false);
    if (ok && index < static_cast<CSSValueList*>(impl())->length()) {
        slot.setCustomIndex(this, index, indexGetter);
        return true;
    }
    return getStaticValueSlot<JSCSSValueList, Base>(exec, &JSCSSValueListTable, this, propertyName, slot);
}

bool JSCSSValueList::getOwnPropertySlot(ExecState* exec, unsigned propertyName, PropertySlot& slot)
{
    if (propertyName < static_cast<CSSValueList*>(impl())->length()) {
        slot.setCustomIndex(this, propertyName, indexGetter);
        return true;
    }
    return getOwnPropertySlot(exec, Identifier::from(exec, propertyName), slot);
}

JSValuePtr jsCSSValueListLength(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    CSSValueList* imp = static_cast<CSSValueList*>(static_cast<JSCSSValueList*>(asObject(slot.slotBase()))->impl());
    return jsNumber(exec, imp->length());
}

JSValuePtr jsCSSValueListConstructor(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    return static_cast<JSCSSValueList*>(asObject(slot.slotBase()))->getConstructor(exec);
}
void JSCSSValueList::getPropertyNames(ExecState* exec, PropertyNameArray& propertyNames)
{
    for (unsigned i = 0; i < static_cast<CSSValueList*>(impl())->length(); ++i)
        propertyNames.add(Identifier::from(exec, i));
     Base::getPropertyNames(exec, propertyNames);
}

JSValuePtr JSCSSValueList::getConstructor(ExecState* exec)
{
    return getDOMConstructor<JSCSSValueListConstructor>(exec);
}

JSValuePtr jsCSSValueListPrototypeFunctionItem(ExecState* exec, JSObject*, JSValuePtr thisValue, const ArgList& args)
{
    if (!thisValue->isObject(&JSCSSValueList::s_info))
        return throwError(exec, TypeError);
    JSCSSValueList* castedThisObj = static_cast<JSCSSValueList*>(asObject(thisValue));
    CSSValueList* imp = static_cast<CSSValueList*>(castedThisObj->impl());
    unsigned index = args.at(exec, 0)->toInt32(exec);


    JSC::JSValuePtr result = toJS(exec, WTF::getPtr(imp->item(index)));
    return result;
}


JSValuePtr JSCSSValueList::indexGetter(ExecState* exec, const Identifier& propertyName, const PropertySlot& slot)
{
    JSCSSValueList* thisObj = static_cast<JSCSSValueList*>(asObject(slot.slotBase()));
    return toJS(exec, static_cast<CSSValueList*>(thisObj->impl())->item(slot.index()));
}

}
