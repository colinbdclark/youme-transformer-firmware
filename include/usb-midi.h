#pragma once

#include "tusb.h"
#include "class/midi/midi_device.h"
#include "midi-port.h"

template<size_t readBufferSize>
class USBMidiDevicePort: MidiPort {
public:
    uint8_t readBuffer[readBufferSize] = {0};
    size_t numTXBytesDropped = 0;
    void* midi_uart;

    void init(
        sig_MidiParser_MessageCallback onMIDIMessage,
        sig_MidiParser_SysexChunkCallback onSysexChunk,
        void* userData = NULL) {
        tusb_init();
        this->initParser(onMIDIMessage, onSysexChunk,
            userData);
    }

    void tick() {
        tud_task();
    }

    size_t read() {
        if (!tud_midi_mounted()) {
            return 0;
        }


        uint32_t numBytesRead = tud_midi_stream_read(this->readBuffer, readBufferSize);

        if (numBytesRead == 0) {
            return 0;
        }

        sig_MidiParser_feedBytes(&this->midiParser,
            this->readBuffer, numBytesRead);

        return numBytesRead;

    }

    size_t write(uint8_t* buffer, uint32_t numBytes) {
        if (!tud_midi_mounted()) {
            // Bytes that we can't write because the USB port isn't ready
            // don't count as dropped.
            return 0;
        }

        uint32_t bytesWritten = tud_midi_stream_write(
            0, buffer, numBytes);

        if (bytesWritten < numBytes) {
            numTXBytesDropped += (numBytes - bytesWritten);
        }

        return bytesWritten;
    }
};
