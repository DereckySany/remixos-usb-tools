/*
 * Copyright (C) 2006 Apple Computer, Inc.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */
#ifndef ImageDocument_h
#define ImageDocument_h

#include "HTMLDocument.h"

namespace WebCore {
    
class DOMImplementation;
class FrameView;
class HTMLImageElement;

class ImageDocument : public HTMLDocument
{
public:
    ImageDocument(DOMImplementation*, Frame*);

    virtual bool isImageDocument() const { return true; }
    
    virtual Tokenizer* createTokenizer();
    
    CachedImage* cachedImage();
    HTMLImageElement* imageElement() const { return m_imageElement; }
    
    void windowSizeChanged();
    void imageChanged();
    void imageClicked(int x, int y);
private:
    void createDocumentStructure();
    void resizeImageToFit();
    void restoreImageSize();
    bool imageFitsInWindow() const;
    bool shouldShrinkToFit() const;
    float scale() const;
    
    HTMLImageElement* m_imageElement;
    
    // Whether enough of the image has been loaded to determine its size
    bool m_imageSizeIsKnown;
    
    // Whether the image is shrunk to fit or not
    bool m_didShrinkImage;
    
    // Whether the image should be shrunk or not
    bool m_shouldShrinkImage;
};
    
}

#endif // ImageDocument_h
