/*
 * This file is part of the WebKit project.
 *
 * Copyright (C) 2008 Trolltech ASA
 *
 * Copyright (C) 2006 Zack Rusin <zack@kde.org>
 *               2006 Dirk Mueller <mueller@kde.org>
 *               2006 Nikolas Zimmermann <zimmermann@kde.org>
 *
 * All rights reserved.
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
 *
 */

#include "config.h"

#include "qwebpage.h"
#include "RenderThemeQt.h"
#include "ChromeClientQt.h"
#include "NotImplemented.h"

#include <QApplication>
#include <QColor>
#include <QDebug>
#include <QWidget>
#include <QPainter>
#include <QPushButton>
#include <QStyleFactory>
#include <QStyleOptionButton>
#include <QStyleOptionFrameV2>

#include "Color.h"
#include "Document.h"
#include "Page.h"
#include "Font.h"
#include "RenderTheme.h"
#include "GraphicsContext.h"

namespace WebCore {

StylePainter::StylePainter(const RenderObject::PaintInfo& paintInfo)
{
    painter = (paintInfo.context ? static_cast<QPainter*>(paintInfo.context->platformContext()) : 0);
    widget = 0;
    QPaintDevice* dev = 0;
    if (painter)
        dev = painter->device();
    if (dev && dev->devType() == QInternal::Widget)
        widget = static_cast<QWidget*>(dev);
    style = (widget ? widget->style() : QApplication::style());

    if (painter) {
        // the styles often assume being called with a pristine painter where no brush is set,
        // so reset it manually
        oldBrush = painter->brush();
        painter->setBrush(Qt::NoBrush);
    }
}

StylePainter::~StylePainter()
{
    if (painter)
        painter->setBrush(oldBrush);
}

RenderTheme* theme()
{
    static RenderThemeQt rt;
    return &rt;
}

RenderThemeQt::RenderThemeQt()
    : RenderTheme()
{
    QPushButton button;
    button.setAttribute(Qt::WA_MacSmallSize);
    QFont defaultButtonFont = QApplication::font(&button);
    QFontInfo fontInfo(defaultButtonFont);
    m_buttonFontFamily = defaultButtonFont.family();
    m_buttonFontPixelSize = fontInfo.pixelSize();

    m_fallbackStyle = 0;
}

RenderThemeQt::~RenderThemeQt()
{
    delete m_fallbackStyle;
}

// for some widget painting, we need to fallback to Windows style
QStyle* RenderThemeQt::fallbackStyle()
{
    if(!m_fallbackStyle)
        m_fallbackStyle = QStyleFactory::create(QLatin1String("windows"));

    if(!m_fallbackStyle)
        m_fallbackStyle = QApplication::style();

    return m_fallbackStyle;
}

bool RenderThemeQt::supportsHover(const RenderStyle*) const
{
    return true;
}

bool RenderThemeQt::supportsFocusRing(const RenderStyle* style) const
{
    return true; // Qt provides this through the style
}

short RenderThemeQt::baselinePosition(const RenderObject* o) const
{
    if (o->style()->appearance() == CheckboxAppearance ||
        o->style()->appearance() == RadioAppearance)
        return o->marginTop() + o->height() - 2; // Same as in old khtml
    return RenderTheme::baselinePosition(o);
}

bool RenderThemeQt::controlSupportsTints(const RenderObject* o) const
{
    if (!isEnabled(o))
        return false;

    // Checkboxes only have tint when checked.
    if (o->style()->appearance() == CheckboxAppearance)
        return isChecked(o);

    // For now assume other controls have tint if enabled.
    return true;
}

bool RenderThemeQt::supportsControlTints() const
{
    return true;
}

static QRect inflateButtonRect(const QRect& originalRect)
{
    QStyleOptionButton option;
    option.state |= QStyle::State_Small;
    option.rect = originalRect;

    QRect layoutRect = QApplication::style()->subElementRect(QStyle::SE_PushButtonLayoutItem,
                                                                  &option, 0);
    if (!layoutRect.isNull()) {
        int paddingLeft = layoutRect.left() - originalRect.left();
        int paddingRight = originalRect.right() - layoutRect.right();
        int paddingTop = layoutRect.top() - originalRect.top();
        int paddingBottom = originalRect.bottom() - layoutRect.bottom();

        return originalRect.adjusted(-paddingLeft, -paddingTop, paddingRight, paddingBottom);
    } else {
        return originalRect;
    }
}

void RenderThemeQt::adjustRepaintRect(const RenderObject* o, IntRect& r)
{
    switch (o->style()->appearance()) {
    case CheckboxAppearance: {
        break;
    }
    case RadioAppearance: {
        break;
    }
    case PushButtonAppearance:
    case ButtonAppearance: {
        QRect inflatedRect = inflateButtonRect(r);
        r = IntRect(inflatedRect.x(), inflatedRect.y(), inflatedRect.width(), inflatedRect.height());
        break;
    }
    case MenulistAppearance: {
        break;
    }
    default:
        break;
    }
}

bool RenderThemeQt::isControlStyled(const RenderStyle* style, const BorderData& border,
                                     const BackgroundLayer& background, const Color& backgroundColor) const
{
    if (style->appearance() == TextFieldAppearance
            || style->appearance() == TextAreaAppearance
            || style->appearance() == ListboxAppearance) {
        return style->border() != border;
    }

    return RenderTheme::isControlStyled(style, border, background, backgroundColor);
}

Color RenderThemeQt::platformActiveSelectionBackgroundColor() const
{
    QPalette pal = QApplication::palette();
    return pal.brush(QPalette::Active, QPalette::Highlight).color();
}

Color RenderThemeQt::platformInactiveSelectionBackgroundColor() const
{
    QPalette pal = QApplication::palette();
    return pal.brush(QPalette::Inactive, QPalette::Highlight).color();
}

Color RenderThemeQt::platformActiveSelectionForegroundColor() const
{
    QPalette pal = QApplication::palette();
    return pal.brush(QPalette::Active, QPalette::HighlightedText).color();
}

Color RenderThemeQt::platformInactiveSelectionForegroundColor() const
{
    QPalette pal = QApplication::palette();
    return pal.brush(QPalette::Inactive, QPalette::HighlightedText).color();
}

void RenderThemeQt::systemFont(int propId, FontDescription& fontDescription) const
{
    // no-op
}

int RenderThemeQt::minimumMenuListSize(RenderStyle*) const
{
    const QFontMetrics &fm = QApplication::fontMetrics();
    return 7 * fm.width(QLatin1Char('x'));
}

void RenderThemeQt::adjustSliderThumbSize(RenderObject* o) const
{
    RenderTheme::adjustSliderThumbSize(o);
}

static void computeSizeBasedOnStyle(RenderStyle* renderStyle)
{
    // If the width and height are both specified, then we have nothing to do.
    if (!renderStyle->width().isIntrinsicOrAuto() && !renderStyle->height().isAuto())
        return;

    QSize size(0, 0);
    const QFontMetrics fm(renderStyle->font().font());
    QStyle* applicationStyle = QApplication::style();

    switch (renderStyle->appearance()) {
    case CheckboxAppearance: {
        QStyleOption styleOption;
        styleOption.state |= QStyle::State_Small;
        int checkBoxWidth = applicationStyle->pixelMetric(QStyle::PM_IndicatorWidth,
                                                          &styleOption);
        size = QSize(checkBoxWidth, checkBoxWidth);
        break;
    }
    case RadioAppearance: {
        QStyleOption styleOption;
        styleOption.state |= QStyle::State_Small;
        int radioWidth = applicationStyle->pixelMetric(QStyle::PM_ExclusiveIndicatorWidth,
                                                       &styleOption);
        size = QSize(radioWidth, radioWidth);
        break;
    }
    case PushButtonAppearance:
    case ButtonAppearance: {
        QStyleOptionButton styleOption;
        styleOption.state |= QStyle::State_Small;
        QSize contentSize = fm.size(Qt::TextShowMnemonic, QString::fromLatin1("X"));
        QSize pushButtonSize = applicationStyle->sizeFromContents(QStyle::CT_PushButton,
                                                                  &styleOption,
                                                                  contentSize,
                                                                  0);
        styleOption.rect = QRect(0, 0, pushButtonSize.width(), pushButtonSize.height());
        QRect layoutRect = applicationStyle->subElementRect(QStyle::SE_PushButtonLayoutItem,
                                                                  &styleOption,
                                                                  0);
        // If the style supports layout rects we use that, and
        // compensate accordingly in paintButton() below.
        if (!layoutRect.isNull()) {
            size.setHeight(layoutRect.height());
        } else {
            size.setHeight(pushButtonSize.height());
        }

        break;
    }
    case MenulistAppearance: {
        QStyleOptionComboBox styleOption;
        styleOption.state |= QStyle::State_Small;
        int contentHeight = qMax(fm.lineSpacing(), 14) + 2;
        QSize menuListSize = applicationStyle->sizeFromContents(QStyle::CT_ComboBox,
                                                        &styleOption,
                                                        QSize(0, contentHeight),
                                                        0);
        size.setHeight(menuListSize.height());
        break;
    }
    case TextFieldAppearance: {
        const int verticalMargin = 1;
        const int horizontalMargin = 2;
        int h = qMax(fm.lineSpacing(), 14) + 2*verticalMargin;
        int w = fm.width(QLatin1Char('x')) * 17 + 2*horizontalMargin;
        QStyleOptionFrameV2 opt;
        opt.lineWidth = applicationStyle->pixelMetric(QStyle::PM_DefaultFrameWidth,
                                                           &opt, 0);
        QSize sz = applicationStyle->sizeFromContents(QStyle::CT_LineEdit,
                                                           &opt,
                                                           QSize(w, h).expandedTo(QApplication::globalStrut()),
                                                           0);
        size.setHeight(sz.height());
        break;
    }
    default:
        break;
    }

    // FIXME: Check is flawed, since it doesn't take min-width/max-width into account.
    if (renderStyle->width().isIntrinsicOrAuto() && size.width() > 0)
        renderStyle->setWidth(Length(size.width(), Fixed));
    if (renderStyle->height().isAuto() && size.height() > 0)
        renderStyle->setHeight(Length(size.height(), Fixed));
}


void RenderThemeQt::setCheckboxSize(RenderStyle* style) const
{
    computeSizeBasedOnStyle(style);
}

bool RenderThemeQt::paintCheckbox(RenderObject* o, const RenderObject::PaintInfo& i, const IntRect& r)
{
    return paintButton(o, i, r);
}


void RenderThemeQt::setRadioSize(RenderStyle* style) const
{
    computeSizeBasedOnStyle(style);
}

bool RenderThemeQt::paintRadio(RenderObject* o, const RenderObject::PaintInfo& i, const IntRect& r)
{
    return paintButton(o, i, r);
}

void RenderThemeQt::adjustButtonStyle(CSSStyleSelector* selector, RenderStyle* style, Element* e) const
{
    // Ditch the border.
    style->resetBorder();

    // Height is locked to auto.
    style->setHeight(Length(Auto));

    // White-space is locked to pre
    style->setWhiteSpace(PRE);

    // Use fixed font size and family
    FontDescription fontDescription = style->fontDescription();
    fontDescription.setIsAbsoluteSize(true);
    fontDescription.setSpecifiedSize(m_buttonFontPixelSize);
    fontDescription.setComputedSize(m_buttonFontPixelSize);
    FontFamily fontFamily;
    fontFamily.setFamily(m_buttonFontFamily);
    fontDescription.setFamily(fontFamily);
    style->setFontDescription(fontDescription);
    style->setLineHeight(RenderStyle::initialLineHeight());

    setButtonSize(style);
    setButtonPadding(style);
}

void RenderThemeQt::setButtonSize(RenderStyle* style) const
{
    computeSizeBasedOnStyle(style);
}

void RenderThemeQt::setButtonPadding(RenderStyle* style) const
{
    QStyleOptionButton styleOption;
    styleOption.state |= QStyle::State_Small;

    // Fake a button rect here, since we're just computing deltas
    QRect originalRect = QRect(0, 0, 100, 30);
    styleOption.rect = originalRect;

    // Default padding is based on the button margin pixel metric
    int buttonMargin = QApplication::style()->pixelMetric(QStyle::PM_ButtonMargin,
                                                          &styleOption, 0);
    int paddingLeft = buttonMargin;
    int paddingRight = buttonMargin;
    int paddingTop = 1;
    int paddingBottom = 0;

    // Then check if the style uses layout margins
    QRect layoutRect = QApplication::style()->subElementRect(QStyle::SE_PushButtonLayoutItem,
                                                                  &styleOption, 0);
    if (!layoutRect.isNull()) {
        QRect contentsRect = QApplication::style()->subElementRect(QStyle::SE_PushButtonContents,
                                                                  &styleOption, 0);
        paddingLeft = contentsRect.left() - layoutRect.left();
        paddingRight = layoutRect.right() - contentsRect.right();
        paddingTop = contentsRect.top() - layoutRect.top();

        // Can't use this right now because we don't have the baseline to compensate
        // paddingBottom = layoutRect.bottom() - contentsRect.bottom();
    }

    style->setPaddingLeft(Length(paddingLeft, Fixed));
    style->setPaddingRight(Length(paddingRight, Fixed));
    style->setPaddingTop(Length(paddingTop, Fixed));
    style->setPaddingBottom(Length(paddingBottom, Fixed));
}

bool RenderThemeQt::paintButton(RenderObject* o, const RenderObject::PaintInfo& i, const IntRect& r)
{
    StylePainter p(i);
    if (!p.isValid())
       return true;

    QStyleOptionButton option;
    if (p.widget)
       option.initFrom(p.widget);

    option.rect = r;
    option.state |= QStyle::State_Small;

    EAppearance appearance = applyTheme(option, o);
    if(appearance == PushButtonAppearance || appearance == ButtonAppearance) {
        option.rect = inflateButtonRect(option.rect);
        p.drawControl(QStyle::CE_PushButton, option);
    } else if(appearance == RadioAppearance) {
       p.drawControl(QStyle::CE_RadioButton, option);
    } else if(appearance == CheckboxAppearance) {
       p.drawControl(QStyle::CE_CheckBox, option);
    }

    return false;
}

bool RenderThemeQt::paintTextField(RenderObject* o, const RenderObject::PaintInfo& i, const IntRect& r)
{
    StylePainter p(i);
    if (!p.isValid())
        return true;

    QStyleOptionFrameV2 panel;
    if (p.widget)
        panel.initFrom(p.widget);

    panel.rect = r;
    panel.lineWidth = p.style->pixelMetric(QStyle::PM_DefaultFrameWidth, &panel, p.widget);
    panel.state |= QStyle::State_Sunken;
    panel.features = QStyleOptionFrameV2::None;

    // Get the correct theme data for a text field
    EAppearance appearance = applyTheme(panel, o);
    if (appearance != TextFieldAppearance
        && appearance != SearchFieldAppearance
        && appearance != TextAreaAppearance
        && appearance != ListboxAppearance)
        return true;

    // Now paint the text field.
    p.drawPrimitive(QStyle::PE_PanelLineEdit, panel);

    return false;
}

void RenderThemeQt::adjustTextFieldStyle(CSSStyleSelector*, RenderStyle* style, Element*) const
{
    style->setBackgroundColor(Color::transparent);
}

bool RenderThemeQt::paintTextArea(RenderObject* o, const RenderObject::PaintInfo& i, const IntRect& r)
{
    return paintTextField(o, i, r);
}

void RenderThemeQt::adjustTextAreaStyle(CSSStyleSelector* selector, RenderStyle* style, Element* element) const
{
    adjustTextFieldStyle(selector, style, element);
}

void RenderThemeQt::adjustMenuListStyle(CSSStyleSelector*, RenderStyle* style, Element*) const
{
    style->resetBorder();

    // Height is locked to auto.
    style->setHeight(Length(Auto));

    // White-space is locked to pre
    style->setWhiteSpace(PRE);

    computeSizeBasedOnStyle(style);

    // Add in the padding that we'd like to use.
    setPopupPadding(style);
}

void RenderThemeQt::setPopupPadding(RenderStyle* style) const
{
    const int padding = 8;
    style->setPaddingLeft(Length(padding, Fixed));

    QStyleOptionComboBox opt;
    int w = QApplication::style()->pixelMetric(QStyle::PM_ButtonIconSize, &opt, 0);
    style->setPaddingRight(Length(padding + w, Fixed));

    style->setPaddingTop(Length(2, Fixed));
    style->setPaddingBottom(Length(0, Fixed));
}


bool RenderThemeQt::paintMenuList(RenderObject* o, const RenderObject::PaintInfo& i, const IntRect& r)
{
    StylePainter p(i);
    if (!p.isValid())
        return true;

    QStyleOptionComboBox opt;
    if (p.widget)
        opt.initFrom(p.widget);
    EAppearance appearance = applyTheme(opt, o);

    const QPoint topLeft = r.topLeft();
    p.painter->translate(topLeft);
    opt.rect.moveTo(QPoint(0,0));
    opt.rect.setSize(r.size());

    opt.frame = false;

    p.drawComplexControl(QStyle::CC_ComboBox, opt);
    p.painter->translate(-topLeft);
    return false;
}


bool RenderThemeQt::paintMenuListButton(RenderObject* o, const RenderObject::PaintInfo& i,
                                        const IntRect& r)
{
    StylePainter p(i);
    if (!p.isValid())
        return true;

    QStyleOptionComboBox option;
    if (p.widget)
        option.initFrom(p.widget);
    applyTheme(option, o);
    option.rect = r;

    // for drawing the combo box arrow, rely only on the fallback style
    p.style = fallbackStyle();
    option.subControls = QStyle::SC_ComboBoxArrow;
    p.drawComplexControl(QStyle::CC_ComboBox, option);

    return false;
}

void RenderThemeQt::adjustMenuListButtonStyle(CSSStyleSelector* selector, RenderStyle* style,
                                              Element* e) const
{
    // WORKAROUND because html4.css specifies -webkit-border-radius for <select> so we override it here
    // see also http://bugs.webkit.org/show_bug.cgi?id=18399
    style->resetBorderRadius();

    // Height is locked to auto.
    style->setHeight(Length(Auto));

    // White-space is locked to pre
    style->setWhiteSpace(PRE);

    computeSizeBasedOnStyle(style);

    // Add in the padding that we'd like to use.
    setPopupPadding(style);
}

bool RenderThemeQt::paintSliderTrack(RenderObject* o, const RenderObject::PaintInfo& pi,
                                     const IntRect& r)
{
    notImplemented();
    return RenderTheme::paintSliderTrack(o, pi, r);
}

bool RenderThemeQt::paintSliderThumb(RenderObject* o, const RenderObject::PaintInfo& pi,
                                     const IntRect& r)
{
    notImplemented();
    return RenderTheme::paintSliderThumb(o, pi, r);
}

bool RenderThemeQt::paintSearchField(RenderObject* o, const RenderObject::PaintInfo& pi,
                                     const IntRect& r)
{
    paintTextField(o, pi, r);
    return false;
}

void RenderThemeQt::adjustSearchFieldStyle(CSSStyleSelector* selector, RenderStyle* style,
                                           Element* e) const
{
    notImplemented();
    RenderTheme::adjustSearchFieldStyle(selector, style, e);
}

void RenderThemeQt::adjustSearchFieldCancelButtonStyle(CSSStyleSelector* selector, RenderStyle* style,
                                                       Element* e) const
{
    notImplemented();
    RenderTheme::adjustSearchFieldCancelButtonStyle(selector, style, e);
}

bool RenderThemeQt::paintSearchFieldCancelButton(RenderObject* o, const RenderObject::PaintInfo& pi,
                                                 const IntRect& r)
{
    notImplemented();
    return RenderTheme::paintSearchFieldCancelButton(o, pi, r);
}

void RenderThemeQt::adjustSearchFieldDecorationStyle(CSSStyleSelector* selector, RenderStyle* style,
                                                     Element* e) const
{
    notImplemented();
    RenderTheme::adjustSearchFieldDecorationStyle(selector, style, e);
}

bool RenderThemeQt::paintSearchFieldDecoration(RenderObject* o, const RenderObject::PaintInfo& pi,
                                               const IntRect& r)
{
    notImplemented();
    return RenderTheme::paintSearchFieldDecoration(o, pi, r);
}

void RenderThemeQt::adjustSearchFieldResultsDecorationStyle(CSSStyleSelector* selector, RenderStyle* style,
                                                            Element* e) const
{
    notImplemented();
    RenderTheme::adjustSearchFieldResultsDecorationStyle(selector, style, e);
}

bool RenderThemeQt::paintSearchFieldResultsDecoration(RenderObject* o, const RenderObject::PaintInfo& pi,
                                                      const IntRect& r)
{
    notImplemented();
    return RenderTheme::paintSearchFieldResultsDecoration(o, pi, r);
}

bool RenderThemeQt::supportsFocus(EAppearance appearance) const
{
    switch (appearance) {
        case PushButtonAppearance:
        case ButtonAppearance:
        case TextFieldAppearance:
        case TextAreaAppearance:
        case ListboxAppearance:
        case MenulistAppearance:
        case RadioAppearance:
        case CheckboxAppearance:
            return true;
        default: // No for all others...
            return false;
    }
}

EAppearance RenderThemeQt::applyTheme(QStyleOption& option, RenderObject* o) const
{
    // Default bits: no focus, no mouse over
    option.state &= ~(QStyle::State_HasFocus | QStyle::State_MouseOver);

    if (!isEnabled(o))
        option.state &= ~QStyle::State_Enabled;

    if (isReadOnlyControl(o))
        // Readonly is supported on textfields.
        option.state |= QStyle::State_ReadOnly;

    if (supportsFocus(o->style()->appearance()) && isFocused(o))
        option.state |= QStyle::State_HasFocus;

    if (isHovered(o))
        option.state |= QStyle::State_MouseOver;

    EAppearance result = o->style()->appearance();

    switch (result) {
        case PushButtonAppearance:
        case SquareButtonAppearance:
        case ButtonAppearance:
        case ButtonBevelAppearance:
        case ListItemAppearance:
        case MenulistButtonAppearance:
        case ScrollbarButtonLeftAppearance:
        case ScrollbarButtonRightAppearance:
        case ScrollbarTrackHorizontalAppearance:
        case ScrollbarTrackVerticalAppearance:
        case ScrollbarThumbHorizontalAppearance:
        case ScrollbarThumbVerticalAppearance:
        case SearchFieldResultsButtonAppearance:
        case SearchFieldCancelButtonAppearance: {
            if (isPressed(o))
                option.state |= QStyle::State_Sunken;
            else if (result == PushButtonAppearance)
                option.state |= QStyle::State_Raised;
            break;
        }
    }

    if(result == RadioAppearance || result == CheckboxAppearance)
        option.state |= (isChecked(o) ? QStyle::State_On : QStyle::State_Off);

    // If the webview has a custom palette, use it
    Page *page = o->document()->page();
    if (page) {
        QWidget *view = static_cast<ChromeClientQt*>(page->chrome()->client())->m_webPage->view();
        if (view)
            option.palette = view->palette();
    }

    return result;
}

}

// vim: ts=4 sw=4 et
