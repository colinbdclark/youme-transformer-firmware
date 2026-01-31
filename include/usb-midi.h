#include "tusb.h"
#include "class/midi/midi_device.h"

template<size_t readBufferSize>
class USBMidi {
public:
    uint8_t readBuffer[readBufferSize] = {0};
    size_t numTXBytesDropped = 0;
    size_t numRXBytesDropped = 0;
    void* midi_uart;

    void init() {
        tusb_init();
    }

    void tick() {
        tud_task();
    }

    uint8_t read() {
        // TODO: Implement reading from host to this MIDI device
        return 0;
    }

    uint8_t write(uint8_t* buffer, uint32_t numBytes) {
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
