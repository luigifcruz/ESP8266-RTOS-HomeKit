# ESP8266 RTOS HomeKit Accessory (Work In Progress) 
Native Apple's HomeKit Accessory Implementation for the ESP8266 based on FreeRTOS ([esp-open-rtos](https://github.com/SuperHouse/esp-open-rtos)). Updates about the development are available at [DEVELOPMENT.md](https://github.com/luigifreitas/ESP8266-RTOS-HomeKit/blob/master/DEVELOPMENT.md) file.

### Required Functions
- [x] mDNS TXT Discovery.
- [x] TLV Decode/Encode.
- [x] Pairing Step M1-M2.
- [ ] Pairing Step M3-M4.
- [ ] Pairing Step M5-M6.
- [ ] Pair Verify M1-M2 & M3-M4.
- [ ] Add Pairing.
- [ ] Remove Pairing.
- [ ] List Pairing.

### Example Output
Output Pairing Step M1-M2.  
```
[TCP] New client connected!
[TCP] Request received.
[DEBUG] Header have 119 bytes.
[DEBUG] Payload have 6 bytes.
0x00 0x01 0x00 0x06 0x01 0x01
[TLV] Tag received: Pairing method.
[TLV] Tag received: Pairing process (M1)!
[TCP] Writing payload with 409 bytes.
[TCP] Response sent!
[TCP] Client disconnected...
```

### Thanks
1. [Nordic nRF51 HomeKit Library](https://github.com/aanon4/HomeKit) - With some modifications this library worked very well for the ESP8266. In this project, it handles the Crypto Stuff (TLV Encoding/Decoding and SRP Protocol) required by HomeKit. Big thanks to [Aanon4](https://github.com/aanon4).

2. [TweetNaCl](http://tweetnacl.cr.yp.to/) - Crypto SHA512 & Curve.
