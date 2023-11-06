/**
 * This example shows how to use the matrix keyboard support that's built into
 * IoAbstraction, it can be used out the box with either a 3x4 or 4x4 keypad,
 * but you can modify it to use any matrix keyboard quite easily. It just sends
 * the characters that are typed on the keyboard to Serial. The keyboard in This
 * example is connected directly to Arduino pins, but could just as easily be
 * connected over a PCF8574, MCP23017 or other IoAbstraction. For interrupt
 * mode, you cannot use a PCF8574 because the interrupt on the device would be
 * triggered by the output changes when scanning. Only MCP23017 and device pins
 * can be used in interrupt mode.
 *
 * Documentation and reference:
 *
 * https://www.thecoderscorner.com/products/arduino-downloads/io-abstraction/
 * https://www.thecoderscorner.com/ref-docs/ioabstraction/html/index.html
 */

#include <Arduino.h>
#include <IoAbstraction.h>
#include <IoAbstractionWire.h>
#include <IoLogging.h>
#include <KeyboardManager.h>
#include <TaskManagerIO.h>

struct ToggleSwitch {
  int button;
  int pin;
};

struct PushButton {
  int button;
  int pin;
};

struct Encoder {
  int buttonLeft;
  int buttonRight;
  int buttonClick;

  int pinA;
  int pinB;
  int pinClick;

  EncoderListener *encoderListener;
  SwitchListener *switchListener;
};

// row one: two toggle buttons + one big button
const auto BUTTON_1_1 = ToggleSwitch{.button = 0, .pin = -1};
const auto BUTTON_1_2 = ToggleSwitch{.button = 1, .pin = -1};
const auto BUTTON_1_3 = PushButton{.button = 2, .pin = -1};

// row two: five toggle switches
const auto BUTTON_2_1 = ToggleSwitch{.button = 3, .pin = -1};
const auto BUTTON_2_2 = ToggleSwitch{.button = 4, .pin = -1};
const auto BUTTON_2_3 = ToggleSwitch{.button = 5, .pin = -1};
const auto BUTTON_2_4 = ToggleSwitch{.button = 6, .pin = -1};
const auto BUTTON_2_5 = ToggleSwitch{.button = 7, .pin = -1};

// row three: four rotary encoders
auto BUTTON_3_1 = Encoder{.buttonLeft = 8,
                          .buttonRight = 9,
                          .buttonClick = 10,
                          .pinA = 23,
                          .pinB = 22,
                          .pinClick = 21};
auto BUTTON_3_2 = Encoder{.buttonLeft = 11,
                          .buttonRight = 12,
                          .buttonClick = 13,
                          .pinA = 41,
                          .pinB = 40,
                          .pinClick = 39};
auto BUTTON_3_3 = Encoder{.buttonLeft = 14,
                          .buttonRight = 15,
                          .buttonClick = 16,
                          .pinA = 38,
                          .pinB = 37,
                          .pinClick = 36};
auto BUTTON_3_4 = Encoder{.buttonLeft = 17,
                          .buttonRight = 18,
                          .buttonClick = 19,
                          .pinA = 35,
                          .pinB = 34,
                          .pinClick = 33};

// row four: matrix buttons (5x3)
const auto BUTTON_4_1 = PushButton{.button = 20, .pin = -1};
const auto BUTTON_4_2 = PushButton{.button = 21, .pin = -1};
const auto BUTTON_4_3 = PushButton{.button = 22, .pin = -1};
const auto BUTTON_4_4 = PushButton{.button = 23, .pin = -1};
const auto BUTTON_4_5 = PushButton{.button = 24, .pin = -1};
const auto BUTTON_5_1 = PushButton{.button = 25, .pin = -1};
const auto BUTTON_5_2 = PushButton{.button = 26, .pin = -1};
const auto BUTTON_5_3 = PushButton{.button = 27, .pin = -1};
const auto BUTTON_5_4 = PushButton{.button = 28, .pin = -1};
const auto BUTTON_5_5 = PushButton{.button = 29, .pin = -1};
const auto BUTTON_6_1 = PushButton{.button = 30, .pin = -1};
const auto BUTTON_6_2 = PushButton{.button = 31, .pin = -1};
const auto BUTTON_6_3 = PushButton{.button = 32, .pin = -1};
const auto BUTTON_6_4 = PushButton{.button = 33, .pin = -1};
const auto BUTTON_6_5 = PushButton{.button = 34, .pin = -1};

// row five: handle 3 rotary encoders at the bottom as one row

// big rotary encoder
const auto BUTTON_7_1 = Encoder{.buttonLeft = 35,
                                .buttonRight = 36,
                                .buttonClick = 39,
                                .pinA = 20,
                                .pinB = 19,
                                .pinClick = 18};

// smaller rotary encoders
const auto BUTTON_7_2 = Encoder{.buttonLeft = 37,
                                .buttonRight = 38,
                                .buttonClick = 39,
                                .pinA = 17,
                                .pinB = 16,
                                .pinClick = 15};
const auto BUTTON_7_3 = Encoder{.buttonLeft = 40,
                                .buttonRight = 41,
                                .buttonClick = 42,
                                .pinA = 14,
                                .pinB = 13,
                                .pinClick = 32};

//
// We need to make a keyboard layout that the manager can use. choose one of the
// below. The parameter in brackets is the variable name.
//
// MAKE_KEYBOARD_LAYOUT_3X4(keyLayout)
// MAKE_KEYBOARD_LAYOUT_4X4(keyLayout)
const char KEYBOARD_STD_3X5_KEYS[] PROGMEM =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
KeyboardLayout keyLayout(3, 5, KEYBOARD_STD_3X5_KEYS);

// this examples connects the pins directly to an arduino but you could use
// IoExpanders or shift registers instead.
IoAbstractionRef arduinoIo = ioUsingArduino();

//
// We need a keyboard manager class too
//
MatrixKeyboardManager keyboard;

// this examples connects the pins directly to an arduino but you could use
// IoExpanders or shift registers instead.
// MCP23017IoAbstraction io23017(0x20, ACTIVE_LOW_OPEN, 10);

//
// We need a class that extends from KeyboardListener. this gets notified when
// there are changes in the keyboard state.
//
class MyKeyboardListener : public KeyboardListener {
public:
  void keyPressed(char key, bool held) override {
    Serial.print("Key ");
    Serial.print(key);
    Serial.print(" is pressed, held = ");
    Serial.println(held);

    // lookup char position []char KEYBOARD_STD_3X5_KEYS
    int button = 99;
    for (int i = 0; KEYBOARD_STD_3X5_KEYS[i]; i++) {
      if (KEYBOARD_STD_3X5_KEYS[i] == key) {
        button = i;
        break;
      }
    }

    button = button + BUTTON_3_4.buttonClick + 1;
    Serial.print("Pressing button: ");
    Serial.println(button);
    Joystick.button(button, HIGH);
    digitalWrite(LED_BUILTIN, HIGH);
  }

  void keyReleased(char key) override {
    Serial.print("Released ");
    Serial.println(key);

    // lookup char position []char KEYBOARD_STD_3X5_KEYS
    int button = 99;
    for (int i = 0; KEYBOARD_STD_3X5_KEYS[i]; i++) {
      if (KEYBOARD_STD_3X5_KEYS[i] == key) {
        button = i;
        break;
      }
    }

    Serial.print("Releasing button: ");
    Serial.println(button);
    Joystick.button(button, LOW);
    digitalWrite(LED_BUILTIN, LOW);
  }
} myKeyboardListener;

class MyEncoderListener : public EncoderListener {
public:
  int buttonLeft;
  int buttonRight;

  MyEncoderListener(int buttonLeft, int buttonRight) : EncoderListener() {
    this->buttonRight = buttonRight;
    this->buttonLeft = buttonLeft;
  }

  void encoderHasChanged(int newValue) override {
    Serial.print("Encoder change button ");
    if (newValue > 0) {
      Serial.print(this->buttonRight);
      Serial.print(" ");
      Serial.println("right");
      Joystick.button(this->buttonRight, HIGH);
      Joystick.button(this->buttonRight, LOW);

    } else if (newValue < 0) {
      Serial.print(this->buttonLeft);
      Serial.print(" ");
      Serial.println("left");
      Joystick.button(this->buttonLeft, HIGH);
      Joystick.button(this->buttonLeft, LOW);
    }
  }
};

/**
 * In this method we initialise the keyboard to use the arduino pins directly.
 * We assume a 4x3 keyboard was set at the top. We use the keyboard in polling
 * mode in this case. Polling mode can be used on any device.
 */
void initialiseKeyboard3X5ForPollingDevicePins() {
  keyLayout.setRowPin(0, 0);
  keyLayout.setRowPin(1, 1);
  keyLayout.setRowPin(2, 2);
  keyLayout.setColPin(0, 3);
  keyLayout.setColPin(1, 4);
  keyLayout.setColPin(2, 5);
  keyLayout.setColPin(3, 6);
  keyLayout.setColPin(4, 7);

  // create the keyboard mapped to arduino pins and with the layout chosen
  // above. it will callback our listener
  keyboard.initialise(arduinoIo, &keyLayout, &myKeyboardListener, false);
}

class MySwitchListener : public SwitchListener {
  int button;

public:
  MySwitchListener(int button) : SwitchListener() { this->button = button; }

  void onPressed(pinid_t pin, bool held) override {
    Serial.print("Button pressed: ");
    Serial.println(this->button);
  }
  /**
   * called when a key is released
   * @param pin the key number
   * @param held true if key was held down
   */
  void onReleased(pinid_t pin, bool held) override {
    Serial.print("Button released: ");
    Serial.println(this->button);
  }
};

void initaliseEncoder(uint8_t slot, Encoder *encoder) {
  encoder->encoderListener =
      new MyEncoderListener(encoder->buttonLeft, encoder->buttonRight);
  HardwareRotaryEncoder *e = new HardwareRotaryEncoder(
      encoder->pinA, encoder->pinB, encoder->encoderListener);
  e->setUserIntention(DIRECTION_ONLY);
  switches.setEncoder(slot, e);

  encoder->switchListener = new MySwitchListener(encoder->buttonClick);
  switches.addSwitchListener(encoder->pinClick, encoder->switchListener,
                             NO_REPEAT);
}

void initialiseEncoders() {
  initaliseEncoder(0, &BUTTON_3_1);
  initaliseEncoder(1, &BUTTON_3_2);
  initaliseEncoder(2, &BUTTON_3_3);
  initaliseEncoder(3, &BUTTON_3_4);

  /* initaliseEncoder(0, &BUTTON_7_1); */
  /* initaliseEncoder(1, &BUTTON_7_2); */
  /* initaliseEncoder(2, &BUTTON_7_3); */
}

void setup() {
  Serial.begin(9600);

  startTaskManagerLogDelegate();

  // our next task is to initialise swtiches, do this BEFORE doing anything else
  // with switches. We choose to initialise in poll everything (requires no
  // interrupts), but there are other modes too: (SWITCHES_NO_POLLING -
  // interrupt only) or (SWITCHES_POLL_KEYS_ONLY - encoders on interrupt)
  switches.init(asIoRef(internalDigitalDevice()), SWITCHES_NO_POLLING, true);

  // here you can choose between two stock configurations or you could alter one
  // of the methods to meet your hardware requirements.
  // initialiseKeyboard4X4ForInterrupt23017();
  initialiseKeyboard3X5ForPollingDevicePins();

  initialiseEncoders();

  // now set up the repeat key start and interval
  // keyboard.setRepeatKeyMillis(850, 350);

  Serial.println("Keyboard is initialised!");
}

void loop() {
  // as this indirectly uses taskmanager, we must include this in loop.
  taskManager.runLoop();
}
