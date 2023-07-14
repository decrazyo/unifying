
# Logitech Unifying Protocol Implementation

![Arduino nano and nRF24L01+ on breadboard](https://github.com/decrazyo/unifying/blob/main/doc/breadboard.jpg)

This project is an attempt to re-implement the proprietary Logitech Unifying protocol as a free and open C library.
The library is intended to be Arduino compatible while remaining compiler and hardware agnostic.
The goal of this project is to enable people to create custom keyboards and mice that are compatible with Logitech Unifying receivers.

## Example
The provided Arduino example is dependent on the RF24 library.
https://github.com/nRF24/RF24

## TODO
- [ ] Add mouse movement payloads
- [ ] Add multimedia payloads
- [ ] Add wake up payloads
- [ ] Add proper HID++ response payloads
- [ ] Add documentation
- [ ] Add more examples
- [ ] General code cleanup

## Done
- [x] Timing-critical packet transmission
- [x] Pairing with a receiver
- [x] HID++ error response payloads
- [x] Encrypted keystroke payloads
