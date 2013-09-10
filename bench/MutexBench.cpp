/*
 * Copyright 2011 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */
#include "SkBenchmark.h"
#include "SkThread.h"

class MutexBench : public SkBenchmark {
public:
    MutexBench(void* param) : INHERITED(param) {
        fIsRendering = false;
    }
protected:
    virtual const char* onGetName() {
        return "mutex";
    }

    virtual void onDraw(SkCanvas*) {
        SK_DECLARE_STATIC_MUTEX(mu);
        for (int i = 0; i < this->getLoops(); i++) {
            mu.acquire();
            mu.release();
        }
    }

private:
    typedef SkBenchmark INHERITED;
};

///////////////////////////////////////////////////////////////////////////////

static SkBenchmark* Fact(void* p) { return new MutexBench(p); }

static BenchRegistry gReg01(Fact);
