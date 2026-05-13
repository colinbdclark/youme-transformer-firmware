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

typedef struct {
    BOOL allocated;
    JSGCRef func;
    int64_t timeout;
} JSTimer;

#define MAX_TIMERS 16
static JSTimer js_timer_list[MAX_TIMERS];

static JSValue js_setTimeout(JSContext *ctx, JSValue *this_val,
    int argc, JSValue *argv) {
    (void) this_val;
    int delay, i;
    JSValue *pfunc;

    if (argc < 1 || !JS_IsFunction(ctx, argv[0]))
        return JS_ThrowTypeError(ctx, "not a function");
    delay = 0;
    if (argc >= 2 && JS_ToInt32(ctx, &delay, argv[1]))
        return JS_EXCEPTION;
    for (i = 0; i < MAX_TIMERS; i++) {
        JSTimer *th = &js_timer_list[i];
        if (!th->allocated) {
            pfunc = JS_AddGCRef(ctx, &th->func);
            *pfunc = argv[0];
            th->timeout = get_time_ms() + delay;
            th->allocated = TRUE;
            return JS_NewInt32(ctx, i);
        }
    }
    return JS_ThrowInternalError(ctx, "too many timers");
}

static JSValue js_clearTimeout(JSContext *ctx, JSValue *this_val,
    int argc, JSValue *argv) {
    (void) this_val;
    int timer_id;
    if (argc < 1)
        return JS_UNDEFINED;
    if (JS_ToInt32(ctx, &timer_id, argv[0]))
        return JS_EXCEPTION;
    if (timer_id >= 0 && timer_id < MAX_TIMERS) {
        JSTimer *th = &js_timer_list[timer_id];
        if (th->allocated) {
            JS_DeleteGCRef(ctx, &th->func);
            th->allocated = FALSE;
        }
    }
    return JS_UNDEFINED;
}

void tick_timers(JSContext *ctx) {
    int64_t cur_time = get_time_ms();
    for (int i = 0; i < MAX_TIMERS; i++) {
        JSTimer *th = &js_timer_list[i];
        if (th->allocated && cur_time >= th->timeout) {
            if (JS_StackCheck(ctx, 2))
                continue;
            JS_PushArg(ctx, th->func.val);
            JS_PushArg(ctx, JS_NULL);
            JS_DeleteGCRef(ctx, &th->func);
            th->allocated = FALSE;
            JS_Call(ctx, 0);
        }
    }
}

#include "js-stdlib-table.h"
