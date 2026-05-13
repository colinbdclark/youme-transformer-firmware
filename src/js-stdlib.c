#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "cutils.h"
#include "mquickjs.h"
#include "tusb.h"
#include "pico/time.h"

void js_log_func(void *opaque, const void *buf, size_t buf_len) {
    (void) opaque;
    tud_cdc_write(buf, buf_len);
    tud_cdc_write_flush();
}

static int64_t get_time_ms(void) {
    return (int64_t)(time_us_64() / 1000);
}

static JSValue js_print(JSContext *ctx, JSValue *this_val,
    int argc, JSValue *argv) {
    (void) this_val;
    for (int i = 0; i < argc; i++) {
        if (i > 0) tud_cdc_write(" ", 1);
        JSCStringBuf buf;
        const char *str = JS_ToCString(ctx, argv[i], &buf);
        if (str) {
            tud_cdc_write(str, strlen(str));
        }
    }
    tud_cdc_write("\r\n", 2);
    tud_cdc_write_flush();
    return JS_UNDEFINED;
}

static JSValue js_gc(JSContext *ctx, JSValue *this_val,
    int argc, JSValue *argv) {
    (void) this_val; (void) argc; (void) argv;
    JS_GC(ctx);
    return JS_UNDEFINED;
}

static JSValue js_date_now(JSContext *ctx, JSValue *this_val,
    int argc, JSValue *argv) {
    (void) this_val; (void) argc; (void) argv;
    return JS_NewInt64(ctx, get_time_ms());
}

static JSValue js_performance_now(JSContext *ctx, JSValue *this_val,
    int argc, JSValue *argv) {
    (void) this_val; (void) argc; (void) argv;
    return JS_NewInt64(ctx, get_time_ms());
}

#include "js-stdlib-table.h"
