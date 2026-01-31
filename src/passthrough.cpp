#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "midi_uart_lib_config.h"
#include "led.h"
#include "midi-parser.h"
#include "uart-midi.h"
#include "usb-midi.h"
#include "midi-logger.h"

#define CPU_CLOCK_SPEED_KHZ 240000

#define MIDI_UART_NUM 0
#define MIDI_UART_TX_GPIO 0
#define MIDI_UART_RX_GPIO 1

#define READ_BUFFER_SIZE 128
#define LOG_BUFFER_SIZE 1024 * 100

LED mainLED;
LED noteLED;
UARTMidi<READ_BUFFER_SIZE> midiUART;
USBMidi<READ_BUFFER_SIZE> usbMIDI;
MIDILogger<LOG_BUFFER_SIZE> midiLogger;
struct sig_MidiParser midiParser;

void onMIDIMessage(uint8_t* message, size_t size, void* userData) {
    (void)userData;

    // Light up the LED when notes are on.
    if (sig_MIDI_MESSAGE_TYPE(message[0]) == sig_MIDI_STATUS_NOTE_ON &&
        message[2] > 0) {
        noteLED.on();
    } else if (sig_MidiParser_isNoteOff(message)) {
        noteLED.off();
    }

    (void) midiUART.write(message, size);
    (void) usbMIDI.write(message, size);
    midiLogger.write(message, size);
}

void onSysexChunk(uint8_t* sysexData, size_t size, bool isFinal,
    void* userData) {
    (void)userData;
    (void)isFinal;

    (void) midiUART.write(sysexData, size);
    (void) usbMIDI.write(sysexData, size);
}

void midiDataPassthroughLoop() {
    uint8_t numBytesRead = midiUART.read();

    if (numBytesRead == 0) {
        return;
    }

    sig_MidiParser_feedBytes(&midiParser, midiUART.readBuffer, numBytesRead);
}

int main() {
    set_sys_clock_khz(CPU_CLOCK_SPEED_KHZ, true);

    mainLED.init(25);
    mainLED.on();
    noteLED.init(24);

    midiUART.init(MIDI_UART_NUM, MIDI_UART_TX_GPIO, MIDI_UART_RX_GPIO);
    usbMIDI.init();

    sig_MidiParser_init(&midiParser, onMIDIMessage, onSysexChunk, NULL);

    midiLogger.init();

    while (true) {
        usbMIDI.tick();
        midiDataPassthroughLoop();
    }

    noteLED.off();
    mainLED.off();

    return 0;
}
