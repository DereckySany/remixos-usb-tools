/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#include "qcssparser_p.h"

#include <qdebug.h>
#include <qcolor.h>
#include <qfont.h>
#include <qfileinfo.h>
#include <qfontmetrics.h>
#include <qbrush.h>
#include <qimagereader.h>

#ifndef QT_NO_CSSPARSER

QT_BEGIN_NAMESPACE

#include "qcssscanner.cpp"

using namespace QCss;

const char *Scanner::tokenName(QCss::TokenType t)
{
    switch (t) {
        case NONE: return "NONE";
        case S: return "S";
        case CDO: return "CDO";
        case CDC: return "CDC";
        case INCLUDES: return "INCLUDES";
        case DASHMATCH: return "DASHMATCH";
        case LBRACE: return "LBRACE";
        case PLUS: return "PLUS";
        case GREATER: return "GREATER";
        case COMMA: return "COMMA";
        case STRING: return "STRING";
        case INVALID: return "INVALID";
        case IDENT: return "IDENT";
        case HASH: return "HASH";
        case ATKEYWORD_SYM: return "ATKEYWORD_SYM";
        case EXCLAMATION_SYM: return "EXCLAMATION_SYM";
        case LENGTH: return "LENGTH";
        case PERCENTAGE: return "PERCENTAGE";
        case NUMBER: return "NUMBER";
        case FUNCTION: return "FUNCTION";
        case COLON: return "COLON";
        case SEMICOLON: return "SEMICOLON";
        case RBRACE: return "RBRACE";
        case SLASH: return "SLASH";
        case MINUS: return "MINUS";
        case DOT: return "DOT";
        case STAR: return "STAR";
        case LBRACKET: return "LBRACKET";
        case RBRACKET: return "RBRACKET";
        case EQUAL: return "EQUAL";
        case LPAREN: return "LPAREN";
        case RPAREN: return "RPAREN";
        case OR: return "OR";
    }
    return "";
}

struct QCssKnownValue
{
    const char *name;
    quint64 id;
};

static const QCssKnownValue properties[NumProperties - 1] = {
    { "-qt-background-role", QtBackgroundRole },
    { "-qt-block-indent", QtBlockIndent },
    { "-qt-list-indent", QtListIndent },
    { "-qt-paragraph-type", QtParagraphType },
    { "-qt-style-features", QtStyleFeatures },
    { "-qt-table-type", QtTableType },
    { "-qt-user-state", QtUserState },
    { "alternate-background-color", QtAlternateBackground },
    { "background", Background },
    { "background-attachment", BackgroundAttachment },
    { "background-clip", BackgroundClip },
    { "background-color", BackgroundColor },
    { "background-image", BackgroundImage },
    { "background-origin", BackgroundOrigin },
    { "background-position", BackgroundPosition },
    { "background-repeat", BackgroundRepeat },
    { "border", Border },
    { "border-bottom", BorderBottom },
    { "border-bottom-color", BorderBottomColor },
    { "border-bottom-left-radius", BorderBottomLeftRadius },
    { "border-bottom-right-radius", BorderBottomRightRadius },
    { "border-bottom-style", BorderBottomStyle },
    { "border-bottom-width", BorderBottomWidth },
    { "border-color", BorderColor },
    { "border-image", BorderImage },
    { "border-left", BorderLeft },
    { "border-left-color", BorderLeftColor },
    { "border-left-style", BorderLeftStyle },
    { "border-left-width", BorderLeftWidth },
    { "border-radius", BorderRadius },
    { "border-right", BorderRight },
    { "border-right-color", BorderRightColor },
    { "border-right-style", BorderRightStyle },
    { "border-right-width", BorderRightWidth },
    { "border-style", BorderStyles },
    { "border-top", BorderTop },
    { "border-top-color", BorderTopColor },
    { "border-top-left-radius", BorderTopLeftRadius },
    { "border-top-right-radius", BorderTopRightRadius },
    { "border-top-style", BorderTopStyle },
    { "border-top-width", BorderTopWidth },
    { "border-width", BorderWidth },
    { "bottom", Bottom },
    { "color", Color },
    { "float", Float },
    { "font", Font },
    { "font-family", FontFamily },
    { "font-size", FontSize },
    { "font-style", FontStyle },
    { "font-variant", FontVariant },
    { "font-weight", FontWeight },
    { "height", Height },
    { "image", QtImage },
    { "image-position", QtImageAlignment },
    { "left", Left },
    { "list-style", ListStyle },
    { "list-style-type", ListStyleType },
    { "margin" , Margin },
    { "margin-bottom", MarginBottom },
    { "margin-left", MarginLeft },
    { "margin-right", MarginRight },
    { "margin-top", MarginTop },
    { "max-height", MaximumHeight },
    { "max-width", MaximumWidth },
    { "min-height", MinimumHeight },
    { "min-width", MinimumWidth },
    { "outline", Outline },
    { "outline-bottom-left-radius", OutlineBottomLeftRadius },
    { "outline-bottom-right-radius", OutlineBottomRightRadius },
    { "outline-color", OutlineColor },
    { "outline-offset", OutlineOffset },
    { "outline-radius", OutlineRadius },
    { "outline-style", OutlineStyle },
    { "outline-top-left-radius", OutlineTopLeftRadius },
    { "outline-top-right-radius", OutlineTopRightRadius },
    { "outline-width", OutlineWidth },
    { "padding", Padding },
    { "padding-bottom", PaddingBottom },
    { "padding-left", PaddingLeft },
    { "padding-right", PaddingRight },
    { "padding-top", PaddingTop },
    { "page-break-after", PageBreakAfter },
    { "page-break-before", PageBreakBefore },
    { "position", Position },
    { "right", Right },
    { "selection-background-color", QtSelectionBackground },
    { "selection-color", QtSelectionForeground },
    { "spacing", QtSpacing },
    { "subcontrol-origin", QtOrigin },
    { "subcontrol-position", QtPosition },
    { "text-align", TextAlignment },
    { "text-decoration", TextDecoration },
    { "text-indent", TextIndent },
    { "text-transform", TextTransform },
    { "text-underline-style", TextUnderlineStyle },
    { "top", Top },
    { "vertical-align", VerticalAlignment },
    { "white-space", Whitespace },
    { "width", Width }
};

static const QCssKnownValue values[NumKnownValues - 1] = {
    { "active", Value_Active },
    { "alternate-base", Value_AlternateBase },
    { "always", Value_Always },
    { "auto", Value_Auto },
    { "base", Value_Base },
    { "bold", Value_Bold },
    { "bottom", Value_Bottom },
    { "bright-text", Value_BrightText },
    { "button", Value_Button },
    { "button-text", Value_ButtonText },
    { "center", Value_Center },
    { "circle", Value_Circle },
    { "dark", Value_Dark },
    { "dashed", Value_Dashed },
    { "decimal", Value_Decimal },
    { "disabled", Value_Disabled },
    { "disc", Value_Disc },
    { "dot-dash", Value_DotDash },
    { "dot-dot-dash", Value_DotDotDash },
    { "dotted", Value_Dotted },
    { "double", Value_Double },
    { "groove", Value_Groove },
    { "highlight", Value_Highlight },
    { "highlighted-text", Value_HighlightedText },
    { "inset", Value_Inset },
    { "italic", Value_Italic },
    { "large", Value_Large },
    { "left", Value_Left },
    { "light", Value_Light },
    { "line-through", Value_LineThrough },
    { "link", Value_Link },
    { "link-visited", Value_LinkVisited },
    { "lower-alpha", Value_LowerAlpha },
    { "lowercase", Value_Lowercase },
    { "medium", Value_Medium },
    { "mid", Value_Mid },
    { "middle", Value_Middle },
    { "midlight", Value_Midlight },
    { "native", Value_Native },
    { "none", Value_None },
    { "normal", Value_Normal },
    { "nowrap", Value_NoWrap },
    { "oblique", Value_Oblique },
    { "off", Value_Off },
    { "on", Value_On },
    { "outset", Value_Outset },
    { "overline", Value_Overline },
    { "pre", Value_Pre },
    { "pre-wrap", Value_PreWrap },
    { "ridge", Value_Ridge },
    { "right", Value_Right },
    { "selected", Value_Selected },
    { "shadow", Value_Shadow },
    { "small" , Value_Small },
    { "small-caps", Value_SmallCaps },
    { "solid", Value_Solid },
    { "square", Value_Square },
    { "sub", Value_Sub },
    { "super", Value_Super },
    { "text", Value_Text },
    { "top", Value_Top },
    { "transparent", Value_Transparent },
    { "underline", Value_Underline },
    { "upper-alpha", Value_UpperAlpha },
    { "uppercase", Value_Uppercase },
    { "wave", Value_Wave },
    { "window", Value_Window },
    { "window-text", Value_WindowText },
    { "x-large", Value_XLarge },
    { "xx-large", Value_XXLarge }
};

static const QCssKnownValue pseudos[NumPseudos - 1] = {
    { "active", PseudoClass_Active },
    { "adjoins-item", PseudoClass_Item },
    { "alternate", PseudoClass_Alternate },
    { "bottom", PseudoClass_Bottom },
    { "checked", PseudoClass_Checked },
    { "closable", PseudoClass_Closable },
    { "closed", PseudoClass_Closed },
    { "default", PseudoClass_Default },
    { "disabled", PseudoClass_Disabled },
    { "edit-focus", PseudoClass_EditFocus },
    { "editable", PseudoClass_Editable },
    { "enabled", PseudoClass_Enabled },
    { "exclusive", PseudoClass_Exclusive },
    { "first", PseudoClass_First },
    { "flat", PseudoClass_Flat },
    { "floatable", PseudoClass_Floatable },
    { "focus", PseudoClass_Focus },
    { "has-children", PseudoClass_Children },
    { "has-siblings", PseudoClass_Sibling },
    { "horizontal", PseudoClass_Horizontal },
    { "hover", PseudoClass_Hover },
    { "indeterminate" , PseudoClass_Indeterminate },
    { "last", PseudoClass_Last },
    { "left", PseudoClass_Left },
    { "maximized", PseudoClass_Maximized },
    { "middle", PseudoClass_Middle },
    { "minimized", PseudoClass_Minimized },
    { "movable", PseudoClass_Movable },
    { "next-selected", PseudoClass_NextSelected },
    { "no-frame", PseudoClass_Frameless },
    { "non-exclusive", PseudoClass_NonExclusive },
    { "off", PseudoClass_Unchecked },
    { "on", PseudoClass_Checked },
    { "only-one", PseudoClass_OnlyOne },
    { "open", PseudoClass_Open },
    { "pressed", PseudoClass_Pressed },
    { "previous-selected", PseudoClass_PreviousSelected },
    { "read-only", PseudoClass_ReadOnly },
    { "right", PseudoClass_Right },
    { "selected", PseudoClass_Selected },
    { "top", PseudoClass_Top },
    { "unchecked" , PseudoClass_Unchecked },
    { "vertical", PseudoClass_Vertical },
    { "window", PseudoClass_Window }
};

static const QCssKnownValue origins[NumKnownOrigins - 1] = {
    { "border", Origin_Border },
    { "content", Origin_Content },
    { "margin", Origin_Margin }, // not in css
    { "padding", Origin_Padding }
};

static const QCssKnownValue repeats[NumKnownRepeats - 1] = {
    { "no-repeat", Repeat_None },
    { "repeat-x", Repeat_X },
    { "repeat-xy", Repeat_XY },
    { "repeat-y", Repeat_Y }
};

static const QCssKnownValue tileModes[NumKnownTileModes - 1] = {
    { "repeat", TileMode_Repeat },
    { "round", TileMode_Round },
    { "stretch", TileMode_Stretch },
};

static const QCssKnownValue positions[NumKnownPositionModes - 1] = {
    { "absolute", PositionMode_Absolute },
    { "fixed", PositionMode_Fixed },
    { "relative", PositionMode_Relative },
    { "static", PositionMode_Static }
};

static const QCssKnownValue attachments[NumKnownAttachments - 1] = {
    { "fixed", Attachment_Fixed },
    { "scroll", Attachment_Scroll }
};

static const QCssKnownValue styleFeatures[NumKnownStyleFeatures - 1] = {
    { "background-color", StyleFeature_BackgroundColor },
    { "background-gradient", StyleFeature_BackgroundGradient },
    { "none", StyleFeature_None }
};

static bool operator<(const QString &name, const QCssKnownValue &prop)
{
    return QString::compare(name, QLatin1String(prop.name), Qt::CaseInsensitive) < 0;
}

static bool operator<(const QCssKnownValue &prop, const QString &name)
{
    return QString::compare(QLatin1String(prop.name), name, Qt::CaseInsensitive) < 0;
}

static quint64 findKnownValue(const QString &name, const QCssKnownValue *start, int numValues)
{
    const QCssKnownValue *end = &start[numValues - 1];
    const QCssKnownValue *prop = qBinaryFind(start, end, name);
    if (prop == end)
        return 0;
    return prop->id;
}

///////////////////////////////////////////////////////////////////////////////
// Value Extractor
ValueExtractor::ValueExtractor(const QVector<Declaration> &decls, const QPalette &pal)
: declarations(decls), adjustment(0), fontExtracted(false), pal(pal)
{
}

int ValueExtractor::lengthValue(const Value& v)
{
    QString s = v.variant.toString();
    s.reserve(s.length());
    enum { None, Px, Ex, Em } unit = None;
    if (s.endsWith(QLatin1String("px"), Qt::CaseInsensitive))
        unit = Px;
    else if (s.endsWith(QLatin1String("ex"), Qt::CaseInsensitive))
        unit = Ex;
    else if (s.endsWith(QLatin1String("em"), Qt::CaseInsensitive))
        unit = Em;

    if (unit != None)
        s.chop(2);

    bool ok;
    qreal number = s.toDouble(&ok);
    if (!ok)
        return 0;

    QFontMetrics fm(f);
    if (unit == Ex)
        return qRound(fm.xHeight() * number);
    else if (unit == Em)
        return qRound(fm.height() * number);

    return qRound(number);
}

int ValueExtractor::lengthValue(const Declaration &decl)
{
    if (decl.values.count() < 1)
        return 0;
    return lengthValue(decl.values.first());
}

void ValueExtractor::lengthValues(const Declaration &decl, int *m)
{
    int i;
    for (i = 0; i < qMin(decl.values.count(), 4); i++)
        m[i] = lengthValue(decl.values[i]);

    if (i == 0) m[0] = m[1] = m[2] = m[3] = 0;
    else if (i == 1) m[3] = m[2] = m[1] = m[0];
    else if (i == 2) m[2] = m[0], m[3] = m[1];
    else if (i == 3) m[3] = m[1];
}

bool ValueExtractor::extractGeometry(int *w, int *h, int *minw, int *minh, int *maxw, int *maxh)
{
    extractFont();
    bool hit = false;
    for (int i = 0; i < declarations.count(); i++) {
        const Declaration &decl = declarations.at(i);
        switch (decl.propertyId) {
        case Width: *w = lengthValue(decl); break;
        case Height: *h = lengthValue(decl); break;
        case MinimumWidth: *minw = lengthValue(decl); break;
        case MinimumHeight: *minh = lengthValue(decl); break;
        case MaximumWidth: *maxw = lengthValue(decl); break;
        case MaximumHeight: *maxh = lengthValue(decl); break;
        default: continue;
        }
        hit = true;
    }

    return hit;
}

bool ValueExtractor::extractPosition(int *left, int *top, int *right, int *bottom, QCss::Origin *origin,
                                     Qt::Alignment *position, QCss::PositionMode *mode, Qt::Alignment *textAlignment)
{
    extractFont();
    bool hit = false;
    for (int i = 0; i < declarations.count(); i++) {
        const Declaration &decl = declarations.at(i);
        switch (decl.propertyId) {
        case Left: *left = lengthValue(decl); break;
        case Top: *top = lengthValue(decl); break;
        case Right: *right = lengthValue(decl); break;
        case Bottom: *bottom = lengthValue(decl); break;
        case QtOrigin: *origin = decl.originValue(); break;
        case QtPosition: *position = decl.alignmentValue(); break;
        case TextAlignment: *textAlignment = decl.alignmentValue(); break;
        case Position: *mode = decl.positionValue(); break;
        default: continue;
        }
        hit = true;
    }

    return hit;
}

bool ValueExtractor::extractBox(int *margins, int *paddings, int *spacing)
{
    extractFont();
    bool hit = false;
    for (int i = 0; i < declarations.count(); i++) {
        const Declaration &decl = declarations.at(i);
        switch (decl.propertyId) {
        case PaddingLeft: paddings[LeftEdge] = lengthValue(decl); break;
        case PaddingRight: paddings[RightEdge] = lengthValue(decl); break;
        case PaddingTop: paddings[TopEdge] = lengthValue(decl); break;
        case PaddingBottom: paddings[BottomEdge] = lengthValue(decl); break;
        case Padding: lengthValues(decl, paddings); break;

        case MarginLeft: margins[LeftEdge] = lengthValue(decl); break;
        case MarginRight: margins[RightEdge] = lengthValue(decl); break;
        case MarginTop: margins[TopEdge] = lengthValue(decl); break;
        case MarginBottom: margins[BottomEdge] = lengthValue(decl); break;
        case Margin: lengthValues(decl, margins); break;
        case QtSpacing: if (spacing) *spacing = lengthValue(decl); break;

        default: continue;
        }
        hit = true;
    }

    return hit;
}

int ValueExtractor::extractStyleFeatures()
{
    int features = StyleFeature_None;
    for (int i = 0; i < declarations.count(); i++) {
        const Declaration &decl = declarations.at(i);
        if (decl.propertyId == QtStyleFeatures)
            features = decl.styleFeaturesValue();
    }
    return features;
}

QSize ValueExtractor::sizeValue(const Declaration &decl)
{
    int x[2] = { 0, 0 };
    if (decl.values.count() > 0)
        x[0] = lengthValue(decl.values.at(0));
    if (decl.values.count() > 1)
        x[1] = lengthValue(decl.values.at(1));
    else
        x[1] = x[0];
    return QSize(x[0], x[1]);
}

void ValueExtractor::sizeValues(const Declaration &decl, QSize *radii)
{
    radii[0] = sizeValue(decl);
    for (int i = 1; i < 4; i++)
        radii[i] = radii[0];
}

bool ValueExtractor::extractBorder(int *borders, QBrush *colors, BorderStyle *styles,
                                   QSize *radii)
{
    extractFont();
    bool hit = false;
    for (int i = 0; i < declarations.count(); i++) {
        const Declaration &decl = declarations.at(i);
        switch (decl.propertyId) {
        case BorderLeftWidth: borders[LeftEdge] = lengthValue(decl); break;
        case BorderRightWidth: borders[RightEdge] = lengthValue(decl); break;
        case BorderTopWidth: borders[TopEdge] = lengthValue(decl); break;
        case BorderBottomWidth: borders[BottomEdge] = lengthValue(decl); break;
        case BorderWidth: lengthValues(decl, borders); break;

        case BorderLeftColor: colors[LeftEdge] = decl.brushValue(pal); break;
        case BorderRightColor: colors[RightEdge] = decl.brushValue(pal); break;
        case BorderTopColor: colors[TopEdge] = decl.brushValue(pal); break;
        case BorderBottomColor: colors[BottomEdge] = decl.brushValue(pal); break;
        case BorderColor: decl.brushValues(colors, pal); break;

        case BorderTopStyle: styles[TopEdge] = decl.styleValue(); break;
        case BorderBottomStyle: styles[BottomEdge] = decl.styleValue(); break;
        case BorderLeftStyle: styles[LeftEdge] = decl.styleValue(); break;
        case BorderRightStyle: styles[RightEdge] = decl.styleValue(); break;
        case BorderStyles:  decl.styleValues(styles); break;

        case BorderTopLeftRadius: radii[0] = sizeValue(decl); break;
        case BorderTopRightRadius: radii[1] = sizeValue(decl); break;
        case BorderBottomLeftRadius: radii[2] = sizeValue(decl); break;
        case BorderBottomRightRadius: radii[3] = sizeValue(decl); break;
        case BorderRadius: sizeValues(decl, radii); break;

        case BorderLeft:
            borderValue(decl, &borders[LeftEdge], &styles[LeftEdge], &colors[LeftEdge]);
            break;
        case BorderTop:
            borderValue(decl, &borders[TopEdge], &styles[TopEdge], &colors[TopEdge]);
            break;
        case BorderRight:
            borderValue(decl, &borders[RightEdge], &styles[RightEdge], &colors[RightEdge]);
            break;
        case BorderBottom:
            borderValue(decl, &borders[BottomEdge], &styles[BottomEdge], &colors[BottomEdge]);
            break;
        case Border:
            borderValue(decl, &borders[LeftEdge], &styles[LeftEdge], &colors[LeftEdge]);
            borders[TopEdge] = borders[RightEdge] = borders[BottomEdge] = borders[LeftEdge];
            styles[TopEdge] = styles[RightEdge] = styles[BottomEdge] = styles[LeftEdge];
            colors[TopEdge] = colors[RightEdge] = colors[BottomEdge] = colors[LeftEdge];
            break;

        default: continue;
        }
        hit = true;
    }

    return hit;
}

bool ValueExtractor::extractOutline(int *borders, QBrush *colors, BorderStyle *styles,
                                   QSize *radii, int *offsets)
{
    extractFont();
    bool hit = false;
    for (int i = 0; i < declarations.count(); i++) {
        const Declaration &decl = declarations.at(i);
        switch (decl.propertyId) {
        case OutlineWidth: lengthValues(decl, borders); break;
        case OutlineColor: decl.brushValues(colors, pal); break;
        case OutlineStyle:  decl.styleValues(styles); break;

        case OutlineTopLeftRadius: radii[0] = sizeValue(decl); break;
        case OutlineTopRightRadius: radii[1] = sizeValue(decl); break;
        case OutlineBottomLeftRadius: radii[2] = sizeValue(decl); break;
        case OutlineBottomRightRadius: radii[3] = sizeValue(decl); break;
        case OutlineRadius: sizeValues(decl, radii); break;
        case OutlineOffset: lengthValues(decl, offsets); break;

        case Outline:
            borderValue(decl, &borders[LeftEdge], &styles[LeftEdge], &colors[LeftEdge]);
            borders[TopEdge] = borders[RightEdge] = borders[BottomEdge] = borders[LeftEdge];
            styles[TopEdge] = styles[RightEdge] = styles[BottomEdge] = styles[LeftEdge];
            colors[TopEdge] = colors[RightEdge] = colors[BottomEdge] = colors[LeftEdge];
            break;

        default: continue;
        }
        hit = true;
    }

    return hit;
}

static Qt::Alignment parseAlignment(const Value *values, int count)
{
    Qt::Alignment a[2] = { 0, 0 };
    for (int i = 0; i < qMin(2, count); i++) {
        if (values[i].type != Value::KnownIdentifier)
            break;
        switch (values[i].variant.toInt()) {
        case Value_Left: a[i] = Qt::AlignLeft; break;
        case Value_Right: a[i] = Qt::AlignRight; break;
        case Value_Top: a[i] = Qt::AlignTop; break;
        case Value_Bottom: a[i] = Qt::AlignBottom; break;
        case Value_Center: a[i] = Qt::AlignCenter; break;
        default: break;
        }
    }

    if (a[0] == Qt::AlignCenter && a[1] != 0 && a[1] != Qt::AlignCenter)
        a[0] = (a[1] == Qt::AlignLeft || a[1] == Qt::AlignRight) ? Qt::AlignVCenter : Qt::AlignHCenter;
    if ((a[1] == 0 || a[1] == Qt::AlignCenter) && a[0] != Qt::AlignCenter)
        a[1] = (a[0] == Qt::AlignLeft || a[0] == Qt::AlignRight) ? Qt::AlignVCenter : Qt::AlignHCenter;
    return a[0] | a[1];
}

static QColor parseColorValue(Value v, const QPalette &pal)
{
    if (v.type == Value::Identifier || v.type == Value::String) {
        v.variant.convert(QVariant::Color);
        v.type = Value::Color;
    }

    if (v.type == Value::Color)
        return qvariant_cast<QColor>(v.variant);

    if (v.type == Value::KnownIdentifier && v.variant.toInt() == Value_Transparent)
        return Qt::transparent;

    if (v.type != Value::Function)
        return QColor();

    QStringList lst = v.variant.toStringList();
    if (lst.count() != 2)
        return QColor();

    if ((lst.at(0).compare(QLatin1String("palette"), Qt::CaseInsensitive)) == 0) {
        int role = findKnownValue(lst.at(1), values, NumKnownValues);
        if (role >= Value_FirstColorRole && role <= Value_LastColorRole)
            return pal.color((QPalette::ColorRole)(role-Value_FirstColorRole));

        return QColor();
    }

    bool rgb = lst.at(0).startsWith(QLatin1String("rgb"));

    Parser p(lst.at(1));
    if (!p.testExpr())
        return QColor();

    QVector<Value> colorDigits;
    if (!p.parseExpr(&colorDigits))
        return QColor();

    for (int i = 0; i < qMin(colorDigits.count(), 7); i += 2) {
        if (colorDigits.at(i).type == Value::Percentage) {
            colorDigits[i].variant = colorDigits.at(i).variant.toDouble() * 255. / 100.;
            colorDigits[i].type = Value::Number;
        } else if (colorDigits.at(i).type != Value::Number) {
            return QColor();
        }
    }

    int v1 = colorDigits.at(0).variant.toInt();
    int v2 = colorDigits.at(2).variant.toInt();
    int v3 = colorDigits.at(4).variant.toInt();
    int alpha = colorDigits.count() >= 7 ? colorDigits.at(6).variant.toInt() : 255;

    return rgb ? QColor::fromRgb(v1, v2, v3, alpha)
               : QColor::fromHsv(v1, v2, v3, alpha);
}

static QBrush parseBrushValue(Value v, const QPalette &pal)
{
    QColor c = parseColorValue(v, pal);
    if (c.isValid())
        return QBrush(c);

    if (v.type != Value::Function)
        return QBrush();

    QStringList lst = v.variant.toStringList();
    if (lst.count() != 2)
        return QBrush();

    QStringList gradFuncs;
    gradFuncs << QLatin1String("qlineargradient") << QLatin1String("qradialgradient") << QLatin1String("qconicalgradient") << QLatin1String("qgradient");
    int gradType = -1;

    if ((gradType = gradFuncs.indexOf(lst.at(0).toLower())) == -1)
        return QBrush();

    QHash<QString, qreal> vars;
    QVector<QGradientStop> stops;

    int spread = -1;
    QStringList spreads;
    spreads << QLatin1String("pad") << QLatin1String("reflect") << QLatin1String("repeat");

    Parser parser(lst.at(1));
    while (parser.hasNext()) {
        parser.skipSpace();
        if (!parser.test(IDENT))
            return QBrush();
        QString attr = parser.lexem();
        parser.skipSpace();
        if (!parser.test(COLON))
            return QBrush();
        parser.skipSpace();
        if (attr.compare(QLatin1String("stop"), Qt::CaseInsensitive) == 0) {
            Value stop, color;
            parser.next();
            if (!parser.parseTerm(&stop)) return QBrush();
            parser.skipSpace();
            parser.next();
            if (!parser.parseTerm(&color)) return QBrush();
            stops.append(QGradientStop(stop.variant.toDouble(), parseColorValue(color, pal)));
        } else {
            parser.next();
            Value value;
            parser.parseTerm(&value);
            if (attr.compare(QLatin1String("spread"), Qt::CaseInsensitive) == 0) {
                spread = spreads.indexOf(value.variant.toString());
            } else {
                vars[attr] = value.variant.toString().toDouble();
            }
        }
        parser.skipSpace();
        parser.test(COMMA);
    }

    if (gradType == 0) {
        QLinearGradient lg(vars.value(QLatin1String("x1")), vars.value(QLatin1String("y1")),
                           vars.value(QLatin1String("x2")), vars.value(QLatin1String("y2")));
        lg.setCoordinateMode(QGradient::ObjectBoundingMode);
        lg.setStops(stops);
        if (spread != -1)
            lg.setSpread(QGradient::Spread(spread));
        return QBrush(lg);
    }

    if (gradType == 1) {
        QRadialGradient rg(vars.value(QLatin1String("cx")), vars.value(QLatin1String("cy")),
                           vars.value(QLatin1String("radius")), vars.value(QLatin1String("fx")),
                           vars.value(QLatin1String("fy")));
        rg.setCoordinateMode(QGradient::ObjectBoundingMode);
        rg.setStops(stops);
        if (spread != -1)
            rg.setSpread(QGradient::Spread(spread));
        return QBrush(rg);
    }

    if (gradType == 2) {
        QConicalGradient cg(vars.value(QLatin1String("cx")), vars.value(QLatin1String("cy")),
                            vars.value(QLatin1String("angle")));
        cg.setCoordinateMode(QGradient::ObjectBoundingMode);
        cg.setStops(stops);
        if (spread != -1)
            cg.setSpread(QGradient::Spread(spread));
        return QBrush(cg);
    }

    return QBrush();
}

static BorderStyle parseStyleValue(Value v)
{
    if (v.type == Value::KnownIdentifier) {
        switch (v.variant.toInt()) {
        case Value_None:
            return BorderStyle_None;
        case Value_Dotted:
            return BorderStyle_Dotted;
        case Value_Dashed:
            return BorderStyle_Dashed;
        case Value_Solid:
            return BorderStyle_Solid;
        case Value_Double:
            return BorderStyle_Double;
        case Value_DotDash:
            return BorderStyle_DotDash;
        case Value_DotDotDash:
            return BorderStyle_DotDotDash;
        case Value_Groove:
            return BorderStyle_Groove;
        case Value_Ridge:
            return BorderStyle_Ridge;
        case Value_Inset:
            return BorderStyle_Inset;
        case Value_Outset:
            return BorderStyle_Outset;
        case Value_Native:
            return BorderStyle_Native;
        default:
            break;
        }
    }

    return BorderStyle_Unknown;
}

void ValueExtractor::borderValue(const Declaration &decl, int *width, QCss::BorderStyle *style, QBrush *color)
{
    *width = 0;
    *style = BorderStyle_None;
    *color = QColor();

    if (decl.values.isEmpty())
        return;

    int i = 0;
    if (decl.values.at(i).type == Value::Length || decl.values.at(i).type == Value::Number) {
        *width = lengthValue(decl.values.at(i));
        if (++i >= decl.values.count())
            return;
    }

    *style = parseStyleValue(decl.values.at(i));
    if (*style != BorderStyle_Unknown) {
        if (++i >= decl.values.count())
            return;
    } else {
        *style = BorderStyle_None;
    }

    *color = parseBrushValue(decl.values.at(i), pal);
}

static void parseShorthandBackgroundProperty(const QVector<Value> &values, QBrush *brush, QString *image, Repeat *repeat, Qt::Alignment *alignment, const QPalette &pal)
{
    *brush = QBrush();
    *image = QString();
    *repeat = Repeat_XY;
    *alignment = Qt::AlignTop | Qt::AlignLeft;

    for (int i = 0; i < values.count(); ++i) {
        const Value v = values.at(i);
        if (v.type == Value::Uri) {
            *image = v.variant.toString();
            continue;
        } else if (v.type == Value::KnownIdentifier && v.variant.toInt() == Value_None) {
            *image = QString();
            continue;
        }

        Repeat repeatAttempt = static_cast<Repeat>(findKnownValue(v.variant.toString(),
                                                   repeats, NumKnownRepeats));
        if (repeatAttempt != Repeat_Unknown) {
            *repeat = repeatAttempt;
            continue;
        }

        if (v.type == Value::KnownIdentifier) {
            const int start = i;
            int count = 1;
            if (i < values.count() - 1
                && values.at(i + 1).type == Value::KnownIdentifier) {
                ++i;
                ++count;
            }
            Qt::Alignment a = parseAlignment(values.constData() + start, count);
            if (int(a) != 0) {
                *alignment = a;
                continue;
            }
            i -= count - 1;
        }

        *brush = parseBrushValue(v, pal);
    }
}

bool ValueExtractor::extractBackground(QBrush *brush, QString *image, Repeat *repeat,
                                       Qt::Alignment *alignment, Origin *origin, Attachment *attachment,
                                       Origin *clip)
{
    bool hit = false;
    for (int i = 0; i < declarations.count(); ++i) {
        const Declaration &decl = declarations.at(i);
        if (decl.values.isEmpty())
            continue;
        const Value val = decl.values.first();
        switch (decl.propertyId) {
            case BackgroundColor:
                *brush = parseBrushValue(val, pal);
                break;
            case BackgroundImage:
                if (val.type == Value::Uri)
                    *image = val.variant.toString();
                break;
            case BackgroundRepeat:
                *repeat = static_cast<Repeat>(findKnownValue(val.variant.toString(),
                                              repeats, NumKnownRepeats));
                break;
            case BackgroundPosition:
                *alignment = decl.alignmentValue();
                break;
            case BackgroundOrigin:
                *origin = decl.originValue();
                break;
            case BackgroundClip:
                *clip = decl.originValue();
                break;
            case Background:
                parseShorthandBackgroundProperty(decl.values, brush, image, repeat, alignment, pal);
                break;
            case BackgroundAttachment:
                *attachment = decl.attachmentValue();
                break;
            default: continue;
        }
        hit = true;
    }
    return hit;
}

static bool setFontSizeFromValue(Value value, QFont *font, int *fontSizeAdjustment)
{
    if (value.type == Value::KnownIdentifier) {
        bool valid = true;
        switch (value.variant.toInt()) {
            case Value_Small: *fontSizeAdjustment = -1; break;
            case Value_Medium: *fontSizeAdjustment = 0; break;
            case Value_Large: *fontSizeAdjustment = 1; break;
            case Value_XLarge: *fontSizeAdjustment = 2; break;
            case Value_XXLarge: *fontSizeAdjustment = 3; break;
            default: valid = false; break;
        }
        return valid;
    }
    if (value.type != Value::Length)
        return false;

    bool valid = false;
    QString s = value.variant.toString();
    if (s.endsWith(QLatin1String("pt"), Qt::CaseInsensitive)) {
        s.chop(2);
        value.variant = s;
        if (value.variant.convert(QVariant::Double)) {
            font->setPointSizeF(value.variant.toDouble());
            valid = true;
        }
    } else if (s.endsWith(QLatin1String("px"), Qt::CaseInsensitive)) {
        s.chop(2);
        value.variant = s;
        if (value.variant.convert(QVariant::Int)) {
            font->setPixelSize(value.variant.toInt());
            valid = true;
        }
    }
    return valid;
}

static bool setFontStyleFromValue(const Value &value, QFont *font)
{
    if (value.type != Value::KnownIdentifier)
        return false ;
    switch (value.variant.toInt()) {
        case Value_Normal: font->setStyle(QFont::StyleNormal); return true;
        case Value_Italic: font->setStyle(QFont::StyleItalic); return true;
        case Value_Oblique: font->setStyle(QFont::StyleOblique); return true;
        default: break;
    }
    return false;
}

static bool setFontWeightFromValue(const Value &value, QFont *font)
{
    if (value.type == Value::KnownIdentifier) {
        switch (value.variant.toInt()) {
            case Value_Normal: font->setWeight(QFont::Normal); return true;
            case Value_Bold: font->setWeight(QFont::Bold); return true;
            default: break;
        }
        return false;
    }
    if (value.type != Value::Number)
        return false;
    font->setWeight(qMin(value.variant.toInt() / 8, 99));
    return true;
}

static bool setFontFamilyFromValues(const QVector<Value> &values, QFont *font)
{
    QString family;
    for (int i = 0; i < values.count(); ++i) {
        const Value &v = values.at(i);
        if (v.type == Value::TermOperatorComma)
            break;
        const QString str = v.variant.toString();
        if (str.isEmpty())
            break;
        family += str;
        family += QLatin1Char(' ');
    }
    family = family.simplified();
    if (family.isEmpty())
        return false;
    font->setFamily(family);
    return true;
}

static void setTextDecorationFromValues(const QVector<Value> &values, QFont *font)
{
    for (int i = 0; i < values.count(); ++i) {
        if (values.at(i).type != Value::KnownIdentifier)
            continue;
        switch (values.at(i).variant.toInt()) {
            case Value_Underline: font->setUnderline(true); break;
            case Value_Overline: font->setOverline(true); break;
            case Value_LineThrough: font->setStrikeOut(true); break;
            case Value_None:
                font->setUnderline(false);
                font->setOverline(false);
                font->setStrikeOut(false);
                break;
            default: break;
        }
    }
}

static void parseShorthandFontProperty(const QVector<Value> &values, QFont *font, int *fontSizeAdjustment)
{
    font->setStyle(QFont::StyleNormal);
    font->setWeight(QFont::Normal);
    *fontSizeAdjustment = 0;

    int i = 0;
    while (i < values.count()) {
        if (setFontStyleFromValue(values.at(i), font)
            || setFontWeightFromValue(values.at(i), font))
            ++i;
        else
            break;
    }

    if (i < values.count()) {
        setFontSizeFromValue(values.at(i), font, fontSizeAdjustment);
        ++i;
    }

    if (i < values.count()) {
        QString fam = values.at(i).variant.toString();
        if (!fam.isEmpty())
            font->setFamily(fam);
    }
}

static void setFontVariantFromValue(const Value &value, QFont *font)
{
    if (value.type == Value::KnownIdentifier) {
        switch (value.variant.toInt()) {
            case Value_Normal: font->setCapitalization(QFont::MixedCase); break;
            case Value_SmallCaps: font->setCapitalization(QFont::SmallCaps); break;
            default: break;
        }
    }
}

static void setTextTransformFromValue(const Value &value, QFont *font)
{
    if (value.type == Value::KnownIdentifier) {
        switch (value.variant.toInt()) {
            case Value_None: font->setCapitalization(QFont::MixedCase); break;
            case Value_Uppercase: font->setCapitalization(QFont::AllUppercase); break;
            case Value_Lowercase: font->setCapitalization(QFont::AllLowercase); break;
            default: break;
        }
    }
}

bool ValueExtractor::extractFont(QFont *font, int *fontSizeAdjustment)
{
    if (fontExtracted) {
        *font = f;
        *fontSizeAdjustment = adjustment;
        return fontExtracted == 1;
    }

    bool hit = false;
    for (int i = 0; i < declarations.count(); ++i) {
        const Declaration &decl = declarations.at(i);
        if (decl.values.isEmpty())
            continue;
        const Value val = decl.values.first();
        switch (decl.propertyId) {
            case FontSize: setFontSizeFromValue(val, font, fontSizeAdjustment); break;
            case FontStyle: setFontStyleFromValue(val, font); break;
            case FontWeight: setFontWeightFromValue(val, font); break;
            case FontFamily: setFontFamilyFromValues(decl.values, font); break;
            case TextDecoration: setTextDecorationFromValues(decl.values, font); break;
            case Font: parseShorthandFontProperty(decl.values, font, fontSizeAdjustment); break;
            case FontVariant: setFontVariantFromValue(val, font); break;
            case TextTransform: setTextTransformFromValue(val, font); break;
            default: continue;
        }
        hit = true;
    }

    f = *font;
    adjustment = *fontSizeAdjustment;
    fontExtracted = hit ? 1 : 2;
    return hit;
}

bool ValueExtractor::extractPalette(QBrush *fg, QBrush *sfg, QBrush *sbg, QBrush *abg)
{
    bool hit = false;
    for (int i = 0; i < declarations.count(); ++i) {
        const Declaration &decl = declarations.at(i);
        switch (decl.propertyId) {
        case Color: *fg = decl.brushValue(pal); break;
        case QtSelectionForeground: *sfg = decl.brushValue(pal); break;
        case QtSelectionBackground: *sbg = decl.brushValue(pal); break;
        case QtAlternateBackground: *abg = decl.brushValue(pal); break;
        default: continue;
        }
        hit = true;
    }
    return hit;
}

void ValueExtractor::extractFont()
{
    if (fontExtracted)
        return;
    int dummy = -255;
    extractFont(&f, &dummy);
}

bool ValueExtractor::extractImage(QIcon *icon, Qt::Alignment *a, QSize *size)
{
    bool hit = false;
    for (int i = 0; i < declarations.count(); ++i) {
        const Declaration &decl = declarations.at(i);
        switch (decl.propertyId) {
        case QtImage:
            *icon = decl.iconValue();
            if (decl.values.count() > 0 && decl.values.at(0).type == Value::Uri) {
                // try to pull just the size from the image...
                QImageReader imageReader(decl.values.at(0).variant.toString());
                if ((*size = imageReader.size()).isNull()) {
                    // but we'll have to load the whole image if the
                    // format doesn't support just reading the size
                    *size = imageReader.read().size();
                }
            }
            break;
        case QtImageAlignment: *a = decl.alignmentValue();  break;
        default: continue;
        }
        hit = true;
    }
    return hit;
}

///////////////////////////////////////////////////////////////////////////////
// Declaration
QColor Declaration::colorValue(const QPalette &pal) const
{
    if (values.count() != 1)
        return QColor();

    return parseColorValue(values.first(), pal);
}

QBrush Declaration::brushValue(const QPalette &pal) const
{
    if (values.count() != 1)
        return QBrush();

    return parseBrushValue(values.first(), pal);
}

void Declaration::brushValues(QBrush *c, const QPalette &pal) const
{
    int i;
    for (i = 0; i < qMin(values.count(), 4); i++)
        c[i] = parseBrushValue(values.at(i), pal);
    if (i == 0) c[0] = c[1] = c[2] = c[3] = QBrush();
    else if (i == 1) c[3] = c[2] = c[1] = c[0];
    else if (i == 2) c[2] = c[0], c[3] = c[1];
    else if (i == 3) c[3] = c[1];
}

bool Declaration::realValue(qreal *real, const char *unit) const
{
    if (values.count() != 1)
        return false;
    const Value &v = values.first();
    if (unit && v.type != Value::Length)
        return false;
    QString s = v.variant.toString();
    if (unit) {
        if (!s.endsWith(QLatin1String(unit), Qt::CaseInsensitive))
            return false;
        s.chop(qstrlen(unit));
    }
    bool ok = false;
    qreal val = s.toDouble(&ok);
    if (ok)
        *real = val;
    return ok;
}

static bool intValueHelper(const Value &v, int *i, const char *unit)
{
    if (unit && v.type != Value::Length)
        return false;
    QString s = v.variant.toString();
    if (unit) {
        if (!s.endsWith(QLatin1String(unit), Qt::CaseInsensitive))
            return false;
        s.chop(qstrlen(unit));
    }
    bool ok = false;
    int val = s.toInt(&ok);
    if (ok)
        *i = val;
    return ok;
}

bool Declaration::intValue(int *i, const char *unit) const
{
    if (values.count() != 1)
        return false;
    return intValueHelper(values.first(), i, unit);
}

QSize Declaration::sizeValue() const
{
    int x[2] = { 0, 0 };
    if (values.count() > 0)
        intValueHelper(values.at(0), &x[0], "px");
    if (values.count() > 1)
        intValueHelper(values.at(1), &x[1], "px");
    else
        x[1] = x[0];
    return QSize(x[0], x[1]);
}

QRect Declaration::rectValue() const
{
    if (values.count() != 1)
        return QRect();
    const Value &v = values.first();
    if (v.type != Value::Function)
        return QRect();
    QStringList func = v.variant.toStringList();
    if (func.count() != 2 || func.first().compare(QLatin1String("rect")) != 0)
        return QRect();
    QStringList args = func[1].split(QLatin1String(" "), QString::SkipEmptyParts);
    if (args.count() != 4)
        return QRect();
    return QRect(args[0].toInt(), args[1].toInt(), args[2].toInt(), args[3].toInt());
}

void Declaration::colorValues(QColor *c, const QPalette &pal) const
{
    int i;
    for (i = 0; i < qMin(values.count(), 4); i++)
        c[i] = parseColorValue(values.at(i), pal);
    if (i == 0) c[0] = c[1] = c[2] = c[3] = QColor();
    else if (i == 1) c[3] = c[2] = c[1] = c[0];
    else if (i == 2) c[2] = c[0], c[3] = c[1];
    else if (i == 3) c[3] = c[1];
}

BorderStyle Declaration::styleValue() const
{
    if (values.count() != 1)
        return BorderStyle_None;
    return parseStyleValue(values.first());
}

void Declaration::styleValues(BorderStyle *s) const
{
    int i;
    for (i = 0; i < qMin(values.count(), 4); i++)
        s[i] = parseStyleValue(values.at(i));
    if (i == 0) s[0] = s[1] = s[2] = s[3] = BorderStyle_None;
    else if (i == 1) s[3] = s[2] = s[1] = s[0];
    else if (i == 2) s[2] = s[0], s[3] = s[1];
    else if (i == 3) s[3] = s[1];
}

Repeat Declaration::repeatValue() const
{
    if (values.count() != 1)
        return Repeat_Unknown;
    return static_cast<Repeat>(findKnownValue(values.first().variant.toString(),
                                repeats, NumKnownRepeats));
}

Origin Declaration::originValue() const
{
    if (values.count() != 1)
        return Origin_Unknown;
    return static_cast<Origin>(findKnownValue(values.first().variant.toString(),
                               origins, NumKnownOrigins));
}

PositionMode Declaration::positionValue() const
{
    if (values.count() != 1)
        return PositionMode_Unknown;
    return static_cast<PositionMode>(findKnownValue(values.first().variant.toString(),
                                     positions, NumKnownPositionModes));
}

Attachment Declaration::attachmentValue() const
{
    if (values.count() != 1)
        return Attachment_Unknown;
    return static_cast<Attachment>(findKnownValue(values.first().variant.toString(),
                                   attachments, NumKnownAttachments));
}

int Declaration::styleFeaturesValue() const
{
    int features = StyleFeature_None;
    for (int i = 0; i < values.count(); i++) {
        features |= static_cast<int>(findKnownValue(values.value(i).variant.toString(),
                                     styleFeatures, NumKnownStyleFeatures));
    }
    return features;
}

QString Declaration::uriValue() const
{
    if (values.isEmpty() || values.first().type != Value::Uri)
        return QString();
    return values.first().variant.toString();
}

Qt::Alignment Declaration::alignmentValue() const
{
    if (values.isEmpty() || values.count() > 2)
        return Qt::AlignLeft | Qt::AlignTop;

    return parseAlignment(values.constData(), values.count());
}

void Declaration::borderImageValue(QString *image, int *cuts,
                                   TileMode *h, TileMode *v) const
{
    *image = uriValue();
    for (int i = 0; i < 4; i++)
        cuts[i] = -1;
    *h = *v = TileMode_Stretch;

    if (values.count() < 2)
        return;

    if (values.at(1).type == Value::Number) { // cuts!
        int i;
        for (i = 0; i < qMin(values.count()-1, 4); i++) {
            const Value& v = values.at(i+1);
            if (v.type != Value::Number)
                break;
            cuts[i] = v.variant.toString().toInt();
        }
        if (i == 0) cuts[0] = cuts[1] = cuts[2] = cuts[3] = 0;
        else if (i == 1) cuts[3] = cuts[2] = cuts[1] = cuts[0];
        else if (i == 2) cuts[2] = cuts[0], cuts[3] = cuts[1];
        else if (i == 3) cuts[3] = cuts[1];
    }

    if (values.last().type == Value::Identifier) {
        *v = static_cast<TileMode>(findKnownValue(values.last().variant.toString(),
                                      tileModes, NumKnownTileModes));
    }
    if (values[values.count() - 2].type == Value::Identifier) {
        *h = static_cast<TileMode>
                        (findKnownValue(values[values.count()-2].variant.toString(),
                                        tileModes, NumKnownTileModes));
    } else
        *h = *v;
}

QIcon Declaration::iconValue() const
{
    QIcon icon;
    for (int i = 0; i < values.count();) {
        Value value = values.at(i++);
        if (value.type != Value::Uri)
            break;
        QString uri = value.variant.toString();
        QIcon::Mode mode = QIcon::Normal;
        QIcon::State state = QIcon::Off;
        for (int j = 0; j < 2; j++) {
            if (i != values.count() && values.at(i).type == Value::KnownIdentifier) {
                switch (values.at(i).variant.toInt()) {
                case Value_Disabled: mode = QIcon::Disabled; break;
                case Value_Active: mode = QIcon::Active; break;
                case Value_Selected: mode = QIcon::Selected; break;
                case Value_Normal: mode = QIcon::Normal; break;
                case Value_On: state = QIcon::On; break;
                case Value_Off: state = QIcon::Off; break;
                default: break;
                }
                ++i;
            } else {
                break;
            }
        }

        // QIcon is soo broken
        if (icon.isNull())
            icon = QIcon(uri);
        else
            icon.addPixmap(uri, mode, state);

        if (i == values.count())
            break;

        if (values.at(i).type == Value::TermOperatorComma)
            i++;
    }
    return icon;
}

///////////////////////////////////////////////////////////////////////////////
// Selector
int Selector::specificity() const
{
    int val = 0;
    for (int i = 0; i < basicSelectors.count(); ++i) {
        const BasicSelector &sel = basicSelectors.at(i);
        if (!sel.elementName.isEmpty())
            val += 1;

        val += (sel.pseudos.count() + sel.attributeSelectors.count()) * 0x10;
        val += sel.ids.count() * 0x100;
    }
    return val;
}

QString Selector::pseudoElement() const
{
    const BasicSelector& bs = basicSelectors.last();
    if (!bs.pseudos.isEmpty() && bs.pseudos.first().type == PseudoClass_Unknown)
        return bs.pseudos.first().name;
    return QString();
}

quint64 Selector::pseudoClass(quint64 *negated) const
{
    const BasicSelector& bs = basicSelectors.last();
    if (bs.pseudos.isEmpty())
        return PseudoClass_Unspecified;
    quint64 pc = PseudoClass_Unknown;
    for (int i = !pseudoElement().isEmpty(); i < bs.pseudos.count(); i++) {
        const Pseudo &pseudo = bs.pseudos.at(i);
        if (pseudo.type == PseudoClass_Unknown)
            return PseudoClass_Unknown;
        if (!pseudo.negated)
            pc |= pseudo.type;
        else if (negated)
            *negated |= pseudo.type;
    }
    return pc;
}

///////////////////////////////////////////////////////////////////////////////
// StyleSelector
StyleSelector::~StyleSelector()
{
}

QStringList StyleSelector::nodeIds(NodePtr node) const
{
    return QStringList(attribute(node, QLatin1String("id")));
}

bool StyleSelector::selectorMatches(const Selector &selector, NodePtr node)
{
    if (selector.basicSelectors.isEmpty())
        return false;

    if (selector.basicSelectors.first().relationToNext == BasicSelector::NoRelation) {
        if (selector.basicSelectors.count() != 1)
            return false;
        return basicSelectorMatches(selector.basicSelectors.first(), node);
    }
    if (selector.basicSelectors.count() <= 1)
        return false;

    int i = selector.basicSelectors.count() - 1;
    node = duplicateNode(node);
    bool match = true;

    BasicSelector sel = selector.basicSelectors.at(i);
    do {
        match = basicSelectorMatches(sel, node);
        if (!match) {
            if (sel.relationToNext == BasicSelector::MatchNextSelectorIfParent
                || i == selector.basicSelectors.count() - 1) // first element must always match!
                break;
        }

        if (match || sel.relationToNext != BasicSelector::MatchNextSelectorIfAncestor)
            --i;

        if (i < 0)
            break;

        sel = selector.basicSelectors.at(i);
        if (sel.relationToNext == BasicSelector::MatchNextSelectorIfAncestor
            || sel.relationToNext == BasicSelector::MatchNextSelectorIfParent) {

            NodePtr nextParent = parentNode(node);
            freeNode(node);
            node = nextParent;
       } else if (sel.relationToNext == BasicSelector::MatchNextSelectorIfPreceeds) {
            NodePtr previousSibling = previousSiblingNode(node);
            freeNode(node);
            node = previousSibling;
       }
        if (isNullNode(node)) {
            match = false;
            break;
        }
   } while (i >= 0 && (match || sel.relationToNext == BasicSelector::MatchNextSelectorIfAncestor));

    freeNode(node);

    return match;
}

bool StyleSelector::basicSelectorMatches(const BasicSelector &sel, NodePtr node)
{
    if (!sel.attributeSelectors.isEmpty()) {
        if (!hasAttributes(node))
            return false;

        for (int i = 0; i < sel.attributeSelectors.count(); ++i) {
            const QCss::AttributeSelector &a = sel.attributeSelectors.at(i);
            if (!hasAttribute(node, a.name))
                return false;

            const QString attrValue = attribute(node, a.name);

            if (a.valueMatchCriterium == QCss::AttributeSelector::MatchContains) {

                QStringList lst = attrValue.split(QLatin1Char(' '));
                if (!lst.contains(a.value))
                    return false;
            } else if (
                (a.valueMatchCriterium == QCss::AttributeSelector::MatchEqual
                 && attrValue != a.value)
                ||
                (a.valueMatchCriterium == QCss::AttributeSelector::MatchBeginsWith
                 && !attrValue.startsWith(a.value))
               )
                return false;
        }
    }

    if (!sel.elementName.isEmpty()
        && !nodeNameEquals(node, sel.elementName))
            return false;

    if (!sel.ids.isEmpty()
        && sel.ids != nodeIds(node))
            return false;

    return true;
}

static inline bool qcss_selectorLessThan(const QPair<int, QCss::StyleRule> &lhs, const QPair<int, QCss::StyleRule> &rhs)
{
    return lhs.first < rhs.first;
}

void StyleSelector::matchRules(NodePtr node, const QVector<StyleRule> &rules, StyleSheetOrigin origin,
                               int depth, QVector<QPair<int, StyleRule> > *weightedRules)
{
    for (int i = 0; i < rules.count(); ++i) {
        const StyleRule &rule = rules.at(i);
        for (int j = 0; j < rule.selectors.count(); ++j) {
            const Selector& selector = rule.selectors.at(j);
            if (selectorMatches(selector, node)) {
                QPair<int, StyleRule> weightedRule;
                weightedRule.first = selector.specificity()
                                     + (origin == StyleSheetOrigin_Inline)*0x1000*depth;
                weightedRule.second.selectors.append(selector);
                weightedRule.second.declarations = rule.declarations;
                weightedRules->append(weightedRule);
            }
        }
    }
}

// Returns style rules that are in ascending order of specificity
// Each of the StyleRule returned will contain exactly one Selector
QVector<StyleRule> StyleSelector::styleRulesForNode(NodePtr node)
{
    QVector<StyleRule> rules;
    if (styleSheets.isEmpty())
        return rules;

    QVector<QPair<int, StyleRule> > weightedRules; // (spec, rule) that will be sorted below
    for (int sheetIdx = 0; sheetIdx < styleSheets.count(); ++sheetIdx) {
        const StyleSheet &styleSheet = styleSheets.at(sheetIdx);

        matchRules(node, styleSheet.styleRules, styleSheet.origin, styleSheet.depth, &weightedRules);
        if (!medium.isEmpty()) {
            for (int i = 0; i < styleSheet.mediaRules.count(); ++i) {
                if (styleSheet.mediaRules.at(i).media.contains(medium, Qt::CaseInsensitive)) {
                    matchRules(node, styleSheet.mediaRules.at(i).styleRules, styleSheet.origin,
                               styleSheet.depth, &weightedRules);
                }
            }
        }
    }

    qStableSort(weightedRules.begin(), weightedRules.end(), qcss_selectorLessThan);

    for (int j = 0; j < weightedRules.count(); j++)
        rules += weightedRules.at(j).second;

    return rules;
}

// for qtexthtmlparser which requires just the declarations with Enabled state
// and without pseudo elements
QVector<Declaration> StyleSelector::declarationsForNode(NodePtr node, const char *extraPseudo)
{
    QVector<Declaration> decls;
    QVector<StyleRule> rules = styleRulesForNode(node);
    for (int i = 0; i < rules.count(); i++) {
        const Selector& selector = rules.at(i).selectors.at(0);
        const QString pseudoElement = selector.pseudoElement();

        if (extraPseudo && pseudoElement == QLatin1String(extraPseudo)) {
            decls += rules.at(i).declarations;
            continue;
        }

        if (!pseudoElement.isEmpty()) // skip rules with pseudo elements
            continue;
        quint64 pseudoClass = selector.pseudoClass();
        if (pseudoClass == PseudoClass_Enabled || pseudoClass == PseudoClass_Unspecified)
            decls += rules.at(i).declarations;
    }
    return decls;
}

static inline bool isHexDigit(const char c)
{
    return (c >= '0' && c <= '9')
           || (c >= 'a' && c <= 'f')
           || (c >= 'A' && c <= 'F')
           ;
}

QString Scanner::preprocess(const QString &input, bool *hasEscapeSequences)
{
    QString output = input;

    if (hasEscapeSequences)
        *hasEscapeSequences = false;

    int i = 0;
    while (i < output.size()) {
        if (output.at(i) == QLatin1Char('\\')) {

            ++i;
            // test for unicode hex escape
            int hexCount = 0;
            const int hexStart = i;
            while (i < output.size()
                   && isHexDigit(output.at(i).toLatin1())
                   && hexCount < 7) {
                ++hexCount;
                ++i;
            }
            if (hexCount == 0) {
                if (hasEscapeSequences)
                    *hasEscapeSequences = true;
                continue;
            }

            hexCount = qMin(hexCount, 6);
            bool ok = false;
            ushort code = output.mid(hexStart, hexCount).toUShort(&ok, 16);
            if (ok) {
                output.replace(hexStart - 1, hexCount + 1, QChar(code));
                i = hexStart;
            } else {
                i = hexStart;
            }
        } else {
            ++i;
        }
    }
    return output;
}

int QCssScanner_Generated::handleCommentStart()
{
    while (pos < input.size() - 1) {
        if (input.at(pos) == QLatin1Char('*')
            && input.at(pos + 1) == QLatin1Char('/')) {
            pos += 2;
            break;
        }
        ++pos;
    }
    return S;
}

void Scanner::scan(const QString &preprocessedInput, QVector<Symbol> *symbols)
{
    QCssScanner_Generated scanner(preprocessedInput);
    Symbol sym;
    int tok = scanner.lex();
    while (tok != -1) {
        sym.token = static_cast<QCss::TokenType>(tok);
        sym.text = scanner.input;
        sym.start = scanner.lexemStart;
        sym.len = scanner.lexemLength;
        symbols->append(sym);
        tok = scanner.lex();
    }
}

QString Symbol::lexem() const
{
    QString result;
    if (len > 0)
        result.reserve(len);
    for (int i = 0; i < len; ++i) {
        if (text.at(start + i) == QLatin1Char('\\') && i < len - 1)
            ++i;
        result += text.at(start + i);
    }
    return result;
}

Parser::Parser(const QString &css, bool isFile)
{
    init(css, isFile);
}

Parser::Parser()
{
    index = 0;
    errorIndex = -1;
    hasEscapeSequences = false;
}

void Parser::init(const QString &css, bool isFile)
{
    QString styleSheet = css;
    if (isFile) {
        QFile file(css);
        if (file.open(QFile::ReadOnly)) {
            sourcePath = QFileInfo(styleSheet).absolutePath() + QLatin1String("/");
            QTextStream stream(&file);
            styleSheet = stream.readAll();
        } else {
            qWarning() << "QCss::Parser - Failed to load file " << css;
            styleSheet.clear();
        }
    } else {
        sourcePath.clear();
    }

    hasEscapeSequences = false;
    symbols.resize(0);
    Scanner::scan(Scanner::preprocess(styleSheet, &hasEscapeSequences), &symbols);
    index = 0;
    errorIndex = -1;
    symbols.reserve(qMax(symbols.capacity(), symbols.size()));
}

bool Parser::parse(StyleSheet *styleSheet)
{
    if (testTokenAndEndsWith(ATKEYWORD_SYM, QLatin1String("charset"))) {
        if (!next(STRING)) return false;
        if (!next(SEMICOLON)) return false;
    }

    while (test(S) || test(CDO) || test(CDC));

    while (testImport()) {
        ImportRule rule;
        if (!parseImport(&rule)) return false;
        styleSheet->importRules.append(rule);
        while (test(S) || test(CDO) || test(CDC));
    }

    do {
        if (testMedia()) {
            MediaRule rule;
            if (!parseMedia(&rule)) return false;
            styleSheet->mediaRules.append(rule);
        } else if (testPage()) {
            PageRule rule;
            if (!parsePage(&rule)) return false;
            styleSheet->pageRules.append(rule);
        } else if (testRuleset()) {
            StyleRule rule;
            if (!parseRuleset(&rule)) return false;
            styleSheet->styleRules.append(rule);
        } else if (test(ATKEYWORD_SYM)) {
            if (!until(RBRACE)) return false;
        } else if (hasNext()) {
            return false;
        }
        while (test(S) || test(CDO) || test(CDC));
    } while (hasNext());
    return true;
}

Symbol Parser::errorSymbol()
{
    if (errorIndex == -1) return Symbol();
    return symbols.at(errorIndex);
}

static inline void removeOptionalQuotes(QString *str)
{
    if (!str->startsWith(QLatin1Char('\''))
        && !str->startsWith(QLatin1Char('\"')))
        return;
    str->remove(0, 1);
    str->chop(1);
}

bool Parser::parseImport(ImportRule *importRule)
{
    skipSpace();

    if (test(STRING)) {
        importRule->href = lexem();
    } else {
        if (!testAndParseUri(&importRule->href)) return false;
    }
    removeOptionalQuotes(&importRule->href);

    skipSpace();

    if (testMedium()) {
        if (!parseMedium(&importRule->media)) return false;

        while (test(COMMA)) {
            skipSpace();
            if (!parseNextMedium(&importRule->media)) return false;
        }
    }

    if (!next(SEMICOLON)) return false;

    skipSpace();
    return true;
}

bool Parser::parseMedia(MediaRule *mediaRule)
{
    do {
        skipSpace();
        if (!parseNextMedium(&mediaRule->media)) return false;
    } while (test(COMMA));

    if (!next(LBRACE)) return false;
    skipSpace();

    while (testRuleset()) {
        StyleRule rule;
        if (!parseRuleset(&rule)) return false;
        mediaRule->styleRules.append(rule);
    }

    if (!next(RBRACE)) return false;
    skipSpace();
    return true;
}

bool Parser::parseMedium(QStringList *media)
{
    media->append(lexem());
    skipSpace();
    return true;
}

bool Parser::parsePage(PageRule *pageRule)
{
    skipSpace();

    if (testPseudoPage())
        if (!parsePseudoPage(&pageRule->selector)) return false;

    skipSpace();
    if (!next(LBRACE)) return false;

    do {
        skipSpace();
        Declaration decl;
        if (!parseNextDeclaration(&decl)) return false;
        if (!decl.isEmpty())
            pageRule->declarations.append(decl);
    } while (test(SEMICOLON));

    if (!next(RBRACE)) return false;
    skipSpace();
    return true;
}

bool Parser::parsePseudoPage(QString *selector)
{
    if (!next(IDENT)) return false;
    *selector = lexem();
    return true;
}

bool Parser::parseNextOperator(Value *value)
{
    if (!hasNext()) return true;
    switch (next()) {
        case SLASH: value->type = Value::TermOperatorSlash; skipSpace(); break;
        case COMMA: value->type = Value::TermOperatorComma; skipSpace(); break;
        default: prev(); break;
    }
    return true;
}

bool Parser::parseCombinator(BasicSelector::Relation *relation)
{
    *relation = BasicSelector::NoRelation;
    if (lookup() == S) {
        *relation = BasicSelector::MatchNextSelectorIfAncestor;
        skipSpace();
    } else {
        prev();
    }
    if (test(PLUS)) {
        *relation = BasicSelector::MatchNextSelectorIfPreceeds;
    } else if (test(GREATER)) {
        *relation = BasicSelector::MatchNextSelectorIfParent;
    }
    skipSpace();
    return true;
}

bool Parser::parseProperty(Declaration *decl)
{
    decl->property = lexem();
    decl->propertyId = static_cast<Property>(findKnownValue(decl->property, properties, NumProperties));
    skipSpace();
    return true;
}

bool Parser::parseRuleset(StyleRule *styleRule)
{
    Selector sel;
    if (!parseSelector(&sel)) return false;
    styleRule->selectors.append(sel);

    while (test(COMMA)) {
        skipSpace();
        Selector sel;
        if (!parseNextSelector(&sel)) return false;
        styleRule->selectors.append(sel);
    }

    skipSpace();
    if (!next(LBRACE)) return false;
    const int declarationStart = index;

    do {
        skipSpace();
        Declaration decl;
        const int rewind = index;
        if (!parseNextDeclaration(&decl)) {
            index = rewind;
            const bool foundSemicolon = until(SEMICOLON);
            const int semicolonIndex = index;

            index = declarationStart;
            const bool foundRBrace = until(RBRACE);

            if (foundSemicolon && semicolonIndex < index) {
                decl = Declaration();
                index = semicolonIndex - 1;
            } else {
                skipSpace();
                return foundRBrace;
            }
        }
        if (!decl.isEmpty())
            styleRule->declarations.append(decl);
    } while (test(SEMICOLON));

    if (!next(RBRACE)) return false;
    skipSpace();
    return true;
}

bool Parser::parseSelector(Selector *sel)
{
    BasicSelector basicSel;
    if (!parseSimpleSelector(&basicSel)) return false;
    while (testCombinator()) {
        if (!parseCombinator(&basicSel.relationToNext)) return false;

        if (!testSimpleSelector()) break;
        sel->basicSelectors.append(basicSel);

        basicSel = BasicSelector();
        if (!parseSimpleSelector(&basicSel)) return false;
    }
    sel->basicSelectors.append(basicSel);
    return true;
}

bool Parser::parseSimpleSelector(BasicSelector *basicSel)
{
    int minCount = 0;
    if (lookupElementName()) {
        if (!parseElementName(&basicSel->elementName)) return false;
    } else {
        prev();
        minCount = 1;
    }
    bool onceMore;
    int count = 0;
    do {
        onceMore = false;
        if (test(HASH)) {
            QString id = lexem();
            // chop off leading #
            id.remove(0, 1);
            basicSel->ids.append(id);
            onceMore = true;
        } else if (testClass()) {
            onceMore = true;
            AttributeSelector a;
            a.name = QLatin1String("class");
            a.valueMatchCriterium = AttributeSelector::MatchContains;
            if (!parseClass(&a.value)) return false;
            basicSel->attributeSelectors.append(a);
        } else if (testAttrib()) {
            onceMore = true;
            AttributeSelector a;
            if (!parseAttrib(&a)) return false;
            basicSel->attributeSelectors.append(a);
        } else if (testPseudo()) {
            onceMore = true;
            Pseudo ps;
            if (!parsePseudo(&ps)) return false;
            basicSel->pseudos.append(ps);
        }
        if (onceMore) ++count;
    } while (onceMore);
    return count >= minCount;
}

bool Parser::parseClass(QString *name)
{
    if (!next(IDENT)) return false;
    *name = lexem();
    return true;
}

bool Parser::parseElementName(QString *name)
{
    switch (lookup()) {
        case STAR: name->clear(); break;
        case IDENT: *name = lexem(); break;
        default: return false;
    }
    return true;
}

bool Parser::parseAttrib(AttributeSelector *attr)
{
    skipSpace();
    if (!next(IDENT)) return false;
    attr->name = lexem();
    skipSpace();

    if (test(EQUAL)) {
        attr->valueMatchCriterium = AttributeSelector::MatchEqual;
    } else if (test(INCLUDES)) {
        attr->valueMatchCriterium = AttributeSelector::MatchContains;
    } else if (test(DASHMATCH)) {
        attr->valueMatchCriterium = AttributeSelector::MatchBeginsWith;
    } else {
        return next(RBRACKET);
    }

    skipSpace();

    if (!test(IDENT) && !test(STRING)) return false;
    attr->value = unquotedLexem();

    skipSpace();
    return next(RBRACKET);
}

bool Parser::parsePseudo(Pseudo *pseudo)
{
    test(COLON);
    pseudo->negated = test(EXCLAMATION_SYM);
    if (test(IDENT)) {
        pseudo->name = lexem();
        pseudo->type = static_cast<quint64>(findKnownValue(pseudo->name, pseudos, NumPseudos));
        return true;
    }
    if (!next(FUNCTION)) return false;
    pseudo->function = lexem();
    // chop off trailing parenthesis
    pseudo->function.chop(1);
    skipSpace();
    if (!test(IDENT)) return false;
    pseudo->name = lexem();
    skipSpace();
    return next(RPAREN);
}

bool Parser::parseNextDeclaration(Declaration *decl)
{
    if (!testProperty())
        return true; // not an error!
    if (!parseProperty(decl)) return false;
    if (!next(COLON)) return false;
    skipSpace();
    if (!parseNextExpr(&decl->values)) return false;
    if (testPrio())
        if (!parsePrio(decl)) return false;
    return true;
}

bool Parser::testPrio()
{
    const int rewind = index;
    if (!test(EXCLAMATION_SYM)) return false;
    skipSpace();
    if (!test(IDENT)) {
        index = rewind;
        return false;
    }
    if (lexem().compare(QLatin1String("important"), Qt::CaseInsensitive) != 0) {
        index = rewind;
        return false;
    }
    return true;
}

bool Parser::parsePrio(Declaration *declaration)
{
    declaration->important = true;
    skipSpace();
    return true;
}

bool Parser::parseExpr(QVector<Value> *values)
{
    Value val;
    if (!parseTerm(&val)) return false;
    values->append(val);
    bool onceMore;
    do {
        onceMore = false;
        val = Value();
        if (!parseNextOperator(&val)) return false;
        if (val.type != QCss::Value::Unknown)
            values->append(val);
        if (testTerm()) {
            onceMore = true;
            val = Value();
            if (!parseTerm(&val)) return false;
            values->append(val);
        }
    } while (onceMore);
    return true;
}

bool Parser::testTerm()
{
    return test(PLUS) || test(MINUS)
           || test(NUMBER)
           || test(PERCENTAGE)
           || test(LENGTH)
           || test(STRING)
           || test(IDENT)
           || testHexColor()
           || testFunction();
}

bool Parser::parseTerm(Value *value)
{
    QString str = lexem();
    bool haveUnary = false;
    if (lookup() == PLUS || lookup() == MINUS) {
        haveUnary = true;
        if (!hasNext()) return false;
        next();
        str += lexem();
    }

    value->variant = str;
    value->type = QCss::Value::String;
    switch (lookup()) {
        case NUMBER:
            value->type = Value::Number;
            value->variant.convert(QVariant::Double);
            break;
        case PERCENTAGE:
            value->type = Value::Percentage;
            str.chop(1); // strip off %
            value->variant = str;
            break;
        case LENGTH:
            value->type = Value::Length;
            break;

        case STRING:
            if (haveUnary) return false;
            value->type = Value::String;
            str.chop(1);
            str.remove(0, 1);
            value->variant = str;
            break;
        case IDENT: {
            if (haveUnary) return false;
            value->type = Value::Identifier;
            const int id = findKnownValue(str, values, NumKnownValues);
            if (id != 0) {
                value->type = Value::KnownIdentifier;
                value->variant = id;
            }
            break;
        }
        default: {
            if (haveUnary) return false;
            prev();
            if (testHexColor()) {
                QColor col;
                if (!parseHexColor(&col)) return false;
                value->type = Value::Color;
                value->variant = col;
            } else if (testFunction()) {
                QString name, args;
                if (!parseFunction(&name, &args)) return false;
                if (name == QLatin1String("url")) {
                    value->type = Value::Uri;
                    removeOptionalQuotes(&args);
                    if (QFileInfo(args).isRelative() && !sourcePath.isEmpty()) {
                        args.prepend(sourcePath);
                    }
                    value->variant = args;
                } else {
                    value->type = Value::Function;
                    value->variant = QStringList() << name << args;
                }
            } else {
                return recordError();
            }
            return true;
        }
    }
    skipSpace();
    return true;
}

bool Parser::parseFunction(QString *name, QString *args)
{
    *name = lexem();
    name->chop(1);
    skipSpace();
    const int start = index;
    if (!until(RPAREN)) return false;
    for (int i = start; i < index - 1; ++i)
        args->append(symbols.at(i).lexem());
    /*
    if (!nextExpr(&arguments)) return false;
    if (!next(RPAREN)) return false;
    */
    skipSpace();
    return true;
}

bool Parser::parseHexColor(QColor *col)
{
    col->setNamedColor(lexem());
    if (!col->isValid()) return false;
    skipSpace();
    return true;
}

bool Parser::testAndParseUri(QString *uri)
{
    const int rewind = index;
    if (!testFunction()) return false;

    QString name, args;
    if (!parseFunction(&name, &args)) {
        index = rewind;
        return false;
    }
    if (name.toLower() != QLatin1String("url")) {
        index = rewind;
        return false;
    }
    *uri = args;
    removeOptionalQuotes(uri);
    return true;
}

bool Parser::testSimpleSelector()
{
    return testElementName()
           || (test(HASH))
           || testClass()
           || testAttrib()
           || testPseudo();
}

bool Parser::next(QCss::TokenType t)
{
    if (hasNext() && next() == t)
        return true;
    return recordError();
}

bool Parser::test(QCss::TokenType t)
{
    if (index >= symbols.count())
        return false;
    if (symbols.at(index).token == t) {
        ++index;
        return true;
    }
    return false;
}

QString Parser::unquotedLexem() const
{
    QString s = lexem();
    if (lookup() == STRING) {
        s.chop(1);
        s.remove(0, 1);
    }
    return s;
}

QString Parser::lexemUntil(QCss::TokenType t)
{
    QString lexem;
    while (hasNext() && next() != t)
        lexem += symbol().lexem();
    return lexem;
}

bool Parser::until(QCss::TokenType target, QCss::TokenType target2)
{
    int braceCount = 0;
    int brackCount = 0;
    int parenCount = 0;
    if (index) {
        switch(symbols.at(index-1).token) {
        case LBRACE: ++braceCount; break;
        case LBRACKET: ++brackCount; break;
        case FUNCTION:
        case LPAREN: ++parenCount; break;
        default: ;
        }
    }
    while (index < symbols.size()) {
        QCss::TokenType t = symbols.at(index++).token;
        switch (t) {
        case LBRACE: ++braceCount; break;
        case RBRACE: --braceCount; break;
        case LBRACKET: ++brackCount; break;
        case RBRACKET: --brackCount; break;
        case FUNCTION:
        case LPAREN: ++parenCount; break;
        case RPAREN: --parenCount; break;
        default: break;
        }
        if ((t == target || (target2 != NONE && t == target2))
            && braceCount <= 0
            && brackCount <= 0
            && parenCount <= 0)
            return true;

        if (braceCount < 0 || brackCount < 0 || parenCount < 0) {
            --index;
            break;
        }
    }
    return false;
}

bool Parser::testTokenAndEndsWith(QCss::TokenType t, const QLatin1String &str)
{
    if (!test(t)) return false;
    if (!lexem().endsWith(str, Qt::CaseInsensitive)) {
        prev();
        return false;
    }
    return true;
}

QT_END_NAMESPACE
#endif // QT_NO_CSSPARSER
