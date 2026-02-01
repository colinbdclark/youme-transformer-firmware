#pragma once

template <size_t LOG_BUFFER_SIZE>
class MIDILogger {
public:
    uint8_t midiLog[LOG_BUFFER_SIZE] = {0};
    size_t midiLogWriteIdx = 0;

    void init() {
        midiLogWriteIdx = 0;
        memset(midiLog, 0, LOG_BUFFER_SIZE);
    }

    void write(uint8_t* buffer, uint32_t numBytes) {
        if (midiLogWriteIdx + numBytes > LOG_BUFFER_SIZE) {
            return;
        }

        for (uint32_t i = 0; i < numBytes; ++i) {
            midiLog[midiLogWriteIdx++] = buffer[i];
        }
    }
};
