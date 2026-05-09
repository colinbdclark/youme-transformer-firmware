#pragma once

#include <stddef.h>
#include <stdint.h>
#include "js-stdlib.h"

template <size_t heapSize>
class JSEngine {
public:
    JSContext* ctx;
    uint8_t heap[heapSize];

    void init() {
        ctx = JS_NewContext(heap,
            heapSize, &js_stdlib);
        JS_SetLogFunc(ctx, js_log_func);
    }

    JSValue eval(const char* src,
        size_t size,
        const char* fileName = "unknown",
        int flags = 0) {
        return JS_Eval(ctx, src, size, fileName, flags);
    }

    void tick() {
        tick_timers(ctx);
    }

    JSValue applyFn(const char* fnName, JSValue* args, size_t numArgs, JSValue thiz = JS_NULL, int flags = 1) {
        // TODO: error handling:
        // - invalid property name
        // - property isn't a function
        // - stack can't accommodate the call
        // - invalid args, numArgs, thiz
        JSValue togo = JS_UNDEFINED;

        JSValue global = JS_GetGlobalObject(ctx);
        JSValue func = JS_GetPropertyStr(ctx, global, fnName);

        if (JS_IsFunction(ctx, func)) {
            if (!JS_StackCheck(ctx, numArgs + 2)) {
                // Function call stack order:
                // args, func, this
                for (size_t i = 0; i < numArgs; i++) {
                    JS_PushArg(ctx, args[i]);
                }
                JS_PushArg(ctx, func);
                JS_PushArg(ctx, thiz);
                togo = JS_Call(ctx, flags);
            }
        }

        return togo;
    }
};
