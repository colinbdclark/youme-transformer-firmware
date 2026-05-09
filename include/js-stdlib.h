#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "mquickjs.h"

extern const JSSTDLibraryDef js_stdlib;
void tick_timers(JSContext *ctx);
void js_log_func(void *opaque, const void *buf, size_t buf_len);

#ifdef __cplusplus
}
#endif
