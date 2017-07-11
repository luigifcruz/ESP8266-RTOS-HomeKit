#ifndef HOMEKIT_ENUM_H_
#define HOMEKIT_ENUM_H_

enum {
    PAIR_M1 = 0x01,
    PAIR_M2 = 0x02,
    PAIR_M3 = 0x03,
    PAIR_M4 = 0x04,
    PAIR_M5 = 0x05,
    PAIR_M6 = 0x06,
    PAIR_M7 = 0x07,
} Pair_State; 

enum {
    TYPE_METHOD = 0x00,
    TYPE_IDENTIFIER = 0x00,
    TYPE_SALT = 0x02,
    TYPE_PUBLIC_KEY = 0x03,
    TYPE_PROOF = 0X04,
    TYPE_ENCRYPTED_DATA = 0x05,
    TYPE_STATE = 0x06
} Tags; 

#endif /* HOMEKIT_ENUM_H_ */