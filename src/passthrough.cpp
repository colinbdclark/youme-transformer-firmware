#include <cstring>
#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "midi_uart_lib_config.h"
#include "led.h"
#include "midi-port.h"
#include "uart-midi-port.h"
#include "usb-midi-device-port.h"
#include "usb-midi-host-port.h"
#include "js-engine.h"
#include "js-midi.h"

#define CPU_CLOCK_SPEED_KHZ 240000

#define MIDI_UART_NUM 1
#define MIDI_UART_TX_GPIO 4
#define MIDI_UART_RX_GPIO 5
#define USB_HOST_DP_GPIO 12

LED mainLED;
LED noteLED;
UARTMidiPort uartMidiPort;
USBMidiDevicePort usbDevice;
USBMidiHostPort usbHost;
JSEngine<8192> js;
const char* src = "function onMIDI(msg) {console.log(msg.type, msg.note, msg.velocity);}";

void handleMIDINoteMessage(uint8_t* message) {
    uint8_t status = message[0];
    uint8_t noteNum = message[1];
    uint8_t velocity = message[2];

    if (sig_MIDI_MESSAGE_TYPE(status == sig_MIDI_STATUS_NOTE_ON) &&
        velocity > 0) {
        noteLED.on();

        JSValue args[] = {
            JSMidi::noteOn(js.ctx, sig_MIDI_CHANNEL(status), noteNum, velocity)
        };

        js.applyFn("onMIDI", args, 1);
    } else if (sig_MidiParser_isNoteOff(message)) {
        noteLED.off();

        JSValue args[] = {
            JSMidi::noteOff(js.ctx, sig_MIDI_CHANNEL(status), noteNum, velocity)
        };

        js.applyFn("onMIDI", args, 1);
    }
}

void writeMessageFromUART(uint8_t* message, size_t size,
    void* userData) {
    (void) userData;

    handleMIDINoteMessage(message);

    // Write to all output ports.
    uartMidiPort.write(message, size);
    usbDevice.write(message, size);
    usbHost.write(message, size);
}

void writeMessageFromUSBDevice(uint8_t* message, size_t size,
    void* userData) {
    (void) userData;

    handleMIDINoteMessage(message);

    // Only write to the UART and USB host port;
    // don't echo the message back to the USB device port.
    uartMidiPort.write(message, size);
    usbHost.write(message, size);
}

void writeMessageFromUSBHost(uint8_t* message, size_t size,
    void* userData) {
    (void) userData;

    handleMIDINoteMessage(message);

    // Only write to the UART and USB device port;
    // don't echo the message back to the USB host port.
    uartMidiPort.write(message, size);
    usbDevice.write(message, size);
}

void onSysexChunk(uint8_t* sysexData, size_t size, void* userData) {
    (void) userData;

    // TODO: Correctly handle sysex routing.
    uartMidiPort.write(sysexData, size);
    usbDevice.write(sysexData, size);
    usbHost.write(sysexData, size);
}

void onSysexEnd(uint8_t* sysexData, size_t size, void* userData) {
    (void) userData;

    // TODO: Correctly handle sysex routing.
    uartMidiPort.write(sysexData, size);
    usbDevice.write(sysexData, size);
    usbHost.write(sysexData, size);
}

int main() {
    set_sys_clock_khz(CPU_CLOCK_SPEED_KHZ, true);

    stdio_init_all();

    mainLED.init(25);
    noteLED.init(24);

    UARTConfig uartConfig = {
        .uartNum = MIDI_UART_NUM,
        .txGPIO = MIDI_UART_TX_GPIO,
        .rxGPIO = MIDI_UART_RX_GPIO
    };

    MidiParserConfig uartParserConfig = {
        .onMIDIMessage = writeMessageFromUART,
        .onSysexChunk = onSysexChunk,
        .onSysexEnd = onSysexEnd,
        .userData = &uartMidiPort
    };
    uartMidiPort.init(uartConfig, uartParserConfig);

    MidiParserConfig usbDeviceParserConfig = {
        .onMIDIMessage = writeMessageFromUSBDevice,
        .onSysexChunk = onSysexChunk,
        .onSysexEnd = onSysexEnd,
        .userData = &usbDevice
    };
    usbDevice.init(usbDeviceParserConfig);

    MidiParserConfig usbHostParserConfig = {
        .onMIDIMessage = writeMessageFromUSBHost,
        .onSysexChunk = onSysexChunk,
        .onSysexEnd = onSysexEnd,
        .userData = &usbHost
    };
    usbHost.init(USB_HOST_DP_GPIO, usbHostParserConfig);

    js.init();
    js.eval(src, strlen(src));
    mainLED.on();

    while (true) {
        uartMidiPort.tick();
        usbDevice.tick();
        usbHost.tick();
    }

    noteLED.off();
    mainLED.off();

    return 0;
}
