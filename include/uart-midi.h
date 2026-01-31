#include "midi_uart_lib.h"

template<size_t readBufferSize>
class UARTMidi {
public:
    uint8_t readBuffer[readBufferSize] = {0};
    size_t numTXBytesDropped = 0;
    size_t numRXBytesDropped = 0;
    void* midi_uart;

    void init(uint8_t uartNum = 1, uint8_t txGPIO = 6, uint8_t rxGPIO = 7) {
        this->midi_uart = midi_uart_configure(uartNum, txGPIO, rxGPIO);
    }

    uint8_t read() {
        return midi_uart_poll_rx_buffer(midi_uart, readBuffer, readBufferSize);
    }

    uint8_t write(uint8_t* buffer, uint32_t numBytes) {
        uint8_t bytesWritten = midi_uart_write_tx_buffer(
                midi_uart, buffer, numBytes);

        if (bytesWritten < numBytes) {
            numTXBytesDropped += (numBytes - bytesWritten);
        }

        midi_uart_drain_tx_buffer(midi_uart);

        return bytesWritten;
    }
};
