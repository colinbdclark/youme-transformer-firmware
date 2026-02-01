#pragma once

#include "midi_uart_lib.h"
#include "midi-port.h"

struct UARTConfig {
    uint8_t uartNum;
    uint8_t txGPIO;
    uint8_t rxGPIO;
};

static const UARTConfig DEFAULT_UART_CONFIG = {
    .uartNum = 1,
    .txGPIO = 6,
    .rxGPIO = 7
};

template<size_t readBufferSize>
class UARTMidiPort: MidiPort {
public:
    uint8_t readBuffer[readBufferSize] = {0};
    size_t numTXBytesDropped = 0;
    void* midi_uart;

    void init(UARTConfig config,
        sig_MidiParser_MessageCallback onMIDIMessage,
        sig_MidiParser_SysexChunkCallback onSysexChunk,
        void* userData = NULL) {
        this->midi_uart = midi_uart_configure(
            config.uartNum, config.txGPIO, config.rxGPIO);
        this->initParser(onMIDIMessage, onSysexChunk, userData);
    }

    size_t read() {
        uint8_t numBytesRead = midi_uart_poll_rx_buffer(
            midi_uart, readBuffer, readBufferSize);

        if (numBytesRead == 0) {
            return 0;
        }

        sig_MidiParser_feedBytes(&this->midiParser, this->readBuffer, numBytesRead);

        return numBytesRead;
    }

    size_t write(uint8_t* buffer, uint32_t numBytes) {
        uint8_t bytesWritten = midi_uart_write_tx_buffer(
                midi_uart, buffer, numBytes);

        if (bytesWritten < numBytes) {
            numTXBytesDropped += (numBytes - bytesWritten);
        }

        midi_uart_drain_tx_buffer(midi_uart);

        return bytesWritten;
    }
};
