/*
 * Copyright 2014 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "SKPBench.h"
#include "SkCommandLineFlags.h"
#include "SkMultiPictureDraw.h"
#include "SkSurface.h"

DEFINE_int32(benchTileW, 1600, "Tile width  used for SKP playback.");
DEFINE_int32(benchTileH, 512, "Tile height used for SKP playback.");

SKPBench::SKPBench(const char* name, const SkPicture* pic, const SkIRect& clip, SkScalar scale,
                   bool useMultiPictureDraw)
    : fPic(SkRef(pic))
    , fClip(clip)
    , fScale(scale)
    , fName(name)
    , fUseMultiPictureDraw(useMultiPictureDraw) {
    fUniqueName.printf("%s_%.2g", name, scale);  // Scale makes this unqiue for perf.skia.org traces.
    if (useMultiPictureDraw) {
        fUniqueName.append("_mpd");
    }
}

SKPBench::~SKPBench() {
    for (int i = 0; i < fSurfaces.count(); ++i) {
        fSurfaces[i]->unref();
    }
}

const char* SKPBench::onGetName() {
    return fName.c_str();
}

const char* SKPBench::onGetUniqueName() {
    return fUniqueName.c_str();
}

void SKPBench::onPerCanvasPreDraw(SkCanvas* canvas) {
    SkIRect bounds;
    SkAssertResult(canvas->getClipDeviceBounds(&bounds));

    int tileW = SkTMin(FLAGS_benchTileW, bounds.width());
    int tileH = SkTMin(FLAGS_benchTileH, bounds.height());

    int xTiles = SkScalarCeilToInt(bounds.width()  / SkIntToScalar(tileW));
    int yTiles = SkScalarCeilToInt(bounds.height() / SkIntToScalar(tileH));

    fSurfaces.setReserve(xTiles * yTiles);
    fTileRects.setReserve(xTiles * yTiles);

    SkImageInfo ii = canvas->imageInfo().makeWH(tileW, tileH);

    for (int y = bounds.fTop; y < bounds.fBottom; y += tileH) {
        for (int x = bounds.fLeft; x < bounds.fRight; x += tileW) {
            const SkIRect tileRect = SkIRect::MakeXYWH(x, y, tileW, tileH);
            *fTileRects.append() = tileRect;
            *fSurfaces.push() = canvas->newSurface(ii);

            // Never want the contents of a tile to include stuff the parent
            // canvas clips out
            SkRect clip = SkRect::Make(bounds);
            clip.offset(-SkIntToScalar(tileRect.fLeft), -SkIntToScalar(tileRect.fTop));
            fSurfaces.top()->getCanvas()->clipRect(clip);

            fSurfaces.top()->getCanvas()->setMatrix(canvas->getTotalMatrix());
            fSurfaces.top()->getCanvas()->scale(fScale, fScale);
        }
    }
}

void SKPBench::onPerCanvasPostDraw(SkCanvas* canvas) {
    // Draw the last set of tiles into the master canvas in case we're
    // saving the images
    for (int i = 0; i < fTileRects.count(); ++i) {
        SkAutoTUnref<SkImage> image(fSurfaces[i]->newImageSnapshot());
        canvas->drawImage(image,
                          SkIntToScalar(fTileRects[i].fLeft), SkIntToScalar(fTileRects[i].fTop));
        SkSafeSetNull(fSurfaces[i]);
    }

    fSurfaces.rewind();
    fTileRects.rewind();
}

bool SKPBench::isSuitableFor(Backend backend) {
    return backend != kNonRendering_Backend;
}

SkIPoint SKPBench::onGetSize() {
    return SkIPoint::Make(fClip.width(), fClip.height());
}

void SKPBench::onDraw(const int loops, SkCanvas* canvas) {
    if (fUseMultiPictureDraw) {
        for (int i = 0; i < loops; i++) {
            SkMultiPictureDraw mpd;

            for (int j = 0; j < fTileRects.count(); ++j) {
                SkMatrix trans;
                trans.setTranslate(-fTileRects[j].fLeft/fScale,
                                   -fTileRects[j].fTop/fScale);
                mpd.add(fSurfaces[j]->getCanvas(), fPic, &trans);
            }

            mpd.draw();

            for (int j = 0; j < fTileRects.count(); ++j) {
                fSurfaces[j]->getCanvas()->flush();
            }
        }
    } else {
        for (int i = 0; i < loops; i++) {
            for (int j = 0; j < fTileRects.count(); ++j) {
                const SkMatrix trans = SkMatrix::MakeTrans(-fTileRects[j].fLeft / fScale,
                                                           -fTileRects[j].fTop / fScale);
                fSurfaces[j]->getCanvas()->drawPicture(fPic, &trans, NULL);
            }

            for (int j = 0; j < fTileRects.count(); ++j) {
                fSurfaces[j]->getCanvas()->flush();
            }
        }
    }
}
