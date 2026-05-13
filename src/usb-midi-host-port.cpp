#include <cstdint>
#include "usb-midi-host-port.h"

USBMidiHostPortCallbackState* USBMidiHostPort_stateSingleton = nullptr;

static void readCable(uint8_t idx, uint8_t cableNum,
    USBMidiHostPortCallbackState* state) {
    size_t bytesRead = tuh_midi_stream_read(idx, &cableNum, state->readBuffer,
        state->readBufferSize);

    while (bytesRead > 0) {
        sig_MidiParser_feedBytes(state->midiParser, state->readBuffer,
            bytesRead);
        bytesRead = tuh_midi_stream_read(idx, &cableNum, state->readBuffer,
            state->readBufferSize);
    }
}

void tuh_midi_rx_cb(uint8_t idx, uint32_t xferredBytes) {
    (void) xferredBytes;

    // TODO: Handle virtual cables correctly.
    // For now, just read MIDI data from all virtual cables.
    uint8_t numCables = tuh_midi_get_rx_cable_count(idx);
    for (uint8_t i = 0; i < numCables; i++) {
        readCable(idx, i, USBMidiHostPort_stateSingleton);
    }
}
