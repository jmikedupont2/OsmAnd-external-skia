/*
 * Copyright 2012 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */
#ifndef SkPathOpsRect_DEFINED
#define SkPathOpsRect_DEFINED

#include "SkPathOpsPoint.h"

struct SkDRect {
    double fLeft, fTop, fRight, fBottom;

    void add(const SkDPoint& pt) {
        fLeft = SkTMin(fLeft, pt.fX);
        fTop = SkTMin(fTop, pt.fY);
        fRight = SkTMax(fRight, pt.fX);
        fBottom = SkTMax(fBottom, pt.fY);
    }

    bool contains(const SkDPoint& pt) const {
        return approximately_between(fLeft, pt.fX, fRight)
                && approximately_between(fTop, pt.fY, fBottom);
    }

    bool intersects(const SkDRect& r) const {
        if (fLeft > fRight) {
            SkDebugf("!");
        }
        SkASSERT(fLeft <= fRight);
        SkASSERT(fTop <= fBottom);
        SkASSERT(r.fLeft <= r.fRight);
        SkASSERT(r.fTop <= r.fBottom);
        return r.fLeft <= fRight && fLeft <= r.fRight && r.fTop <= fBottom && fTop <= r.fBottom;
    }

    void set(const SkDPoint& pt) {
        fLeft = fRight = pt.fX;
        fTop = fBottom = pt.fY;
    }

    double width() const {
        return fRight - fLeft;
    }

    double height() const {
        return fBottom - fTop;
    }

    void setBounds(const SkDCubic&);
    void setBounds(const SkDQuad&);
};

#endif
