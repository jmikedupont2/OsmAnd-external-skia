
/*
 * Copyright 2012 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef SkSweepGradient_DEFINED
#define SkSweepGradient_DEFINED

#include "SkGradientShaderPriv.h"

class SkSweepGradient : public SkGradientShaderBase {
public:
    SkSweepGradient(SkScalar cx, SkScalar cy, const Descriptor&);
    virtual void shadeSpan(int x, int y, SkPMColor dstC[], int count) SK_OVERRIDE;
    virtual void shadeSpan16(int x, int y, uint16_t dstC[], int count) SK_OVERRIDE;

    virtual BitmapType asABitmap(SkBitmap* bitmap,
                                 SkMatrix* matrix,
                                 TileMode* xy) const SK_OVERRIDE;

    virtual GradientType asAGradient(GradientInfo* info) const SK_OVERRIDE;

    virtual GrEffectRef* asNewEffect(GrContext* context, const SkPaint&) const SK_OVERRIDE;

    SK_DEVELOPER_TO_STRING()
    SK_DECLARE_PUBLIC_FLATTENABLE_DESERIALIZATION_PROCS(SkSweepGradient)

protected:
    SkSweepGradient(SkReadBuffer& buffer);
    virtual void flatten(SkWriteBuffer& buffer) const SK_OVERRIDE;

private:
    typedef SkGradientShaderBase INHERITED;
    const SkPoint fCenter;
};

#endif
