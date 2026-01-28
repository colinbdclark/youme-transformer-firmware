#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "midi_uart_lib_config.h"
#include "midi_uart_lib.h"
#include "led.h"

#define CPU_CLOCK_SPEED_KHZ 240000
#define LED_PIN 25
#define LED_DELAY_MS 250

#define MIDI_UART_NUM 0
#define MIDI_UART_TX_GPIO 0
#define MIDI_UART_RX_GPIO 1

#define READ_BUFFER_SIZE 128

void* midi_uart;
LED led;
uint8_t readBuffer[READ_BUFFER_SIZE] = {0};
constexpr size_t readBufferSize = sizeof(readBuffer);

void midiDataPassthroughLoop() {
    while (true) {
        uint8_t bytesRead = midi_uart_poll_rx_buffer(
            midi_uart, readBuffer, readBufferSize);

        if (bytesRead == 0) {
            break;
        }

        uint8_t bytesWritten = midi_uart_write_tx_buffer(
            midi_uart, readBuffer, bytesRead);

        // Freak out if we can't write
        // all the bytes we just read.
        hard_assert(bytesWritten == bytesRead);
    }

    midi_uart_drain_tx_buffer(midi_uart);
}

int main() {
    set_sys_clock_khz(CPU_CLOCK_SPEED_KHZ, true);

    int rc = led.init(LED_PIN);
    hard_assert(rc == PICO_OK);
    led.on();

    midi_uart = midi_uart_configure(
        MIDI_UART_NUM, MIDI_UART_TX_GPIO, MIDI_UART_RX_GPIO);

    while (true) {
        midiDataPassthroughLoop();
    }

    led.off();
    return 0;
}
