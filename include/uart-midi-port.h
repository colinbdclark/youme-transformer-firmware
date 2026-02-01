#pragma once

#include <algorithm> // For min()
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

template<size_t messageBufferSize = 4,
    size_t sysexBufferSize = 32,
    size_t readBufferSize = 4>
class UARTMidiPort: MidiPort<
    messageBufferSize, sysexBufferSize, readBufferSize> {
public:
    void* midi_uart;

    void init(UARTConfig uartConfig = DEFAULT_UART_CONFIG,
        MidiParserConfig parserConfig = MidiParserConfig()) {
        this->midi_uart = midi_uart_configure(
            uartConfig.uartNum, uartConfig.txGPIO, uartConfig.rxGPIO);
        this->initParser(parserConfig);
    }

    void tick() {
        read();
    }

    inline size_t readBlock() {
        return midi_uart_poll_rx_buffer(midi_uart,
            this->readBuffer, readBufferSize);
    }

    void read() {
        size_t numBytesRead = readBlock();

        while (numBytesRead > 0) {
            size_t bytesToFeed = std::min(numBytesRead,
                readBufferSize);

            sig_MidiParser_feedBytes(&this->midiParser,
                this->readBuffer, bytesToFeed);

            numBytesRead = readBlock();
        }
    }

    size_t write(uint8_t* buffer, uint32_t numBytes) {
        uint8_t bytesWritten = midi_uart_write_tx_buffer(
                midi_uart, buffer, numBytes);

        if (bytesWritten < numBytes) {
            this->numTXBytesDropped += (numBytes - bytesWritten);
        }

        midi_uart_drain_tx_buffer(midi_uart);

        return bytesWritten;
    }
};
