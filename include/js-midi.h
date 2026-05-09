#pragma once

#include "js-stdlib.h"

class JSMidi {
public:
    static JSValue note(JSContext* ctx,
        const char* type,
        uint8_t channel,
        uint8_t noteNum, uint8_t velocity) {
        JSValue obj = JS_NewObject(ctx);

        JS_SetPropertyStr(ctx, obj, "type",
            JS_NewString(ctx, type));
        JS_SetPropertyStr(ctx, obj, "channel",
            JS_NewInt32(ctx, channel));
        JS_SetPropertyStr(ctx, obj, "note",
            JS_NewInt32(ctx, noteNum));
        JS_SetPropertyStr(ctx, obj, "velocity",
            JS_NewInt32(ctx, velocity));

        return obj;
    }

    static JSValue noteOn(JSContext* ctx,
        uint8_t channel,
        uint8_t noteNum,
        uint8_t velocity) {
        return note(ctx, "noteOn", channel, noteNum, velocity);
    }

    static JSValue noteOff(JSContext* ctx,
        uint8_t channel,
        uint8_t noteNum,
        uint8_t velocity) {
        return note(ctx, "noteOff", channel, noteNum, velocity);
    }
};
