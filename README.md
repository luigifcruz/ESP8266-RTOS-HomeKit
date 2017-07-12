# ESP8266 RTOS HomeKit Accessory (Work In Progress) 
Native Apple's HomeKit Accessory Implementation for the ESP8266 based on FreeRTOS ([esp-open-rtos](https://github.com/SuperHouse/esp-open-rtos)). Updates about the development are available at [DEVELOPMENT.md](https://github.com/luigifreitas/ESP8266-RTOS-HomeKit/blob/master/DEVELOPMENT.md) file.

### Implemented Functions
- [x] mDNS TXT Discovery
- [x] TLV Decode/Encode 
- [x] SRP Server
- [x] Pairing Step M1.
- [x] Pairing Step M2.
- [ ] Pairing Step M3.
- [ ] Pairing Step M4.
- [ ] Pairing Step M5.

### Example Output
Output from the first paring process (M1-M2).  
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
