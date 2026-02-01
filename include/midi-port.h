#pragma once

#include "midi-parser.h"

class MidiPort {
public:
    struct sig_MidiParser midiParser;

    void initParser(
        sig_MidiParser_MessageCallback onMIDIMessage,
        sig_MidiParser_SysexChunkCallback onSysexChunk,
        void* userData = NULL) {
        sig_MidiParser_init(&this->midiParser,
            onMIDIMessage, onSysexChunk, userData);
    }
};


