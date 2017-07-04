# ESP8266 RTOS HomeKit Accessory (Work In Progress) 
Native Apple's HomeKit Accessory Implementation for the ESP8266 based on FreeRTOS ([esp-open-rtos](https://github.com/SuperHouse/esp-open-rtos)).

### Implemented Functions
- [x] mDNS TXT Discovery
- [x] TCP/IP HTTP 1.1 Keep-Alive Server
- [x] HTTP Request Hexadecimal
- [ ] TLV Decode/Encode 
- [ ] SRP Server

### Example Output
Output from the first paring process (M1).  
```
// iOS Device >> Accessory (via TCP HTTP 1.1)
[TCP] New client connected!
[TCP] Request received.
[DEBUG] Header have 119 bytes.
[DEBUG] Payload have 6 bytes.
0x00 0x01 0x00 0x06 0x01 0x01
[TCP] Closing...
```
