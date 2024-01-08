
#include <RF24.h>
#include <printf.h>

#include "aes.hpp"

#include "unifying.h"

#define CE_PIN 7
#define CSN_PIN 8
#define INPUT_PIN 2
#define HALT_PIN 3

#define TRANSMIT_BUFFER_SIZE 8
#define RECEIVE_BUFFER_SIZE 8
#define SLOW_TIMEOUT 110

uint8_t input_state;
uint8_t aes_key[UNIFYING_AES_BLOCK_LEN];
uint8_t address[UNIFYING_ADDRESS_LEN];

struct unifying_interface interface;
struct unifying_ring_buffer* transmit_buffer;
struct unifying_ring_buffer* receive_buffer;
struct unifying_state state;

RF24 radio(CE_PIN, CSN_PIN);

// The following functions are used by the Unifying library to interface with radio hardware.
uint8_t transmit_payload(const uint8_t* payload, uint8_t length) {
  Serial.print("Transmit: ");
  unifying_print_buffer(payload, length);
  bool status = radio.write(payload, length);
  return (uint8_t) !status;
}

uint8_t receive_payload(uint8_t* payload, uint8_t length) {
  if(!radio.available()) {
    return 0;
  }

  uint8_t payload_size = radio.getDynamicPayloadSize();
  radio.read(payload, length);

  Serial.print("Receive:  ");
  unifying_print_buffer(payload, payload_size > length ? length : payload_size);

  return payload_size;
}

bool payload_available() {
  return radio.available();
}

uint8_t payload_size() {
  return radio.getDynamicPayloadSize();
}

uint8_t set_address(const uint8_t address[UNIFYING_ADDRESS_LEN]) {
  uint8_t temp[UNIFYING_ADDRESS_LEN];
  unifying_copy_reverse(temp, address, UNIFYING_ADDRESS_LEN);
  Serial.print("Address:  ");
  unifying_print_buffer(address, UNIFYING_ADDRESS_LEN);
  radio.openWritingPipe(temp);
  return 0;
}

uint8_t set_channel(uint8_t channel) {
  Serial.print("Channel:  ");
  Serial.println(channel);
  radio.setChannel(channel);
  return 0;
}

// Check for key presses and send scancodes.
void scan_keyboard_matrix() {
  uint8_t keys[UNIFYING_KEYS_LEN] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  uint8_t modifiers = 0x00;

  // Check for key presses.
  if(input_state == HIGH && digitalRead(INPUT_PIN) == LOW) {
    Serial.println("Button pressed");
    input_state = LOW;

    // Press keys.
    keys[5] = 0x04; // Scancode for 'a'.
    modifiers = 0x02; // Modifier flag for left shift.

    // This should type "A" for as long as the input button is held down.
    unifying_encrypted_keystroke(&state, keys, modifiers);
  }
  else if(input_state == LOW && digitalRead(INPUT_PIN) == HIGH) {
    Serial.println("Button released");
    input_state = HIGH;

    // Release keys.
    keys[5] = 0x00;
    modifiers = 0x00;
    unifying_encrypted_keystroke(&state, keys, modifiers);
    unifying_set_timeout(&state, SLOW_TIMEOUT);
  }
}

void setup() {
  Serial.begin(115200);
  while(!Serial) {delay(100);}
  printf_begin();

  // Configure a pin as an input.
  // This serves as our mock keyboard matrix.
  pinMode(INPUT_PIN, INPUT_PULLUP);
  input_state = HIGH;

  // Configure a pin as an interrupt.
  // This allows us to halt the microcontroller and analyze the output.
  pinMode(HALT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(HALT_PIN), halt, FALLING);

  // Initialize random()
  randomSeed(analogRead(0));

  if(!radio.begin()) {
    Serial.println(F("Radio not responding"));
    while(true) {delay(100);}
  }

  // Assert defaults to document the correct settings.
  radio.powerUp();
  radio.stopListening();
  radio.setPALevel(RF24_PA_MAX, true);
  radio.setCRCLength(RF24_CRC_16);
  radio.setAddressWidth(sizeof(unifying_pairing_address));
  radio.setAutoAck(true);

  // Assert non-default settings.
  radio.setDataRate(RF24_2MBPS);
  radio.enableDynamicPayloads();
  radio.enableAckPayload();
  radio.setRetries(15, 10);

  // Make sure we don't have any extraneous data in the RX FIFO.
  // The TX FIFO should have already been flushed by calling stopListening()
  radio.flush_rx();

  unifying_interface_init(&interface,
                          transmit_payload,
                          receive_payload,
                          payload_available,
                          payload_size,
                          set_address,
                          set_channel,
                          millis,
                          NULL);

  transmit_buffer = unifying_ring_buffer_create(TRANSMIT_BUFFER_SIZE);
  receive_buffer = unifying_ring_buffer_create(RECEIVE_BUFFER_SIZE);

  uint32_t aes_counter = random();

  // TODO: Check if we have previously paired to a receiver.
  //       If so, load "address" and "aes_key" from non-volatile memory.

  unifying_state_init(&state,
                      &interface,
                      transmit_buffer,
                      receive_buffer,
                      address,
                      aes_key,
                      aes_counter,
                      UNIFYING_DEFAULT_TIMEOUT_KEYBOARD,
                      unifying_channels[0]);


  enum unifying_error err;

  // Try to connect to our receiver.
  err = unifying_connect(&state);

  if(err) {
    // If we can't connect to our receiver then try pairing to a new one.
    uint8_t id = random();
    uint16_t product_id = 0x1025;
    uint16_t device_type = 0x0147;
    uint32_t crypto = random();
    uint32_t serial = 0xA58094B6;
    uint16_t capabilities = 0x1E40;
    char name[] = "Hacked";
    uint8_t name_length = strlen(name);

    err = unifying_pair(&state,
                        id,
                        product_id,
                        device_type,
                        crypto,
                        serial,
                        capabilities,
                        name,
                        name_length);
  }

  Serial.println(unifying_get_error_name(err));

  // TODO: Save "address" and "aes_key" to non-volatile memory if pairing was successful.

  while(err) {
    // If pairing failed then resume trying to connect to our receiver.
    err = unifying_connect(&state);
    delay(100);
  }
}

void loop() {
  // Check if the user has pressed any keys.
  scan_keyboard_matrix();

  // Transmit buffered data.
  unifying_tick(&state);
}

void halt() {
  while(true) {}
}
