#ifndef HOMEKIT_ENUM_H_
#define HOMEKIT_ENUM_H_

enum {
    PAIR_M1 = 0x01,
    PAIR_M3 = 0x03,
    PAIR_M5 = 0x05,
    PAIR_M7 = 0x07,
} Pair_State; 

enum {
    TYPE_METHOD = 0x00,
    TYPE_IDENTIFIER = 0x01,
    TYPE_STATE = 0x06,
    TYPE_SALT = 0x02,
    TYPE_SRP_B = 0x03,
    TYPE_SRP_M2 = 0x04
} Tags; 

#endif /* HOMEKIT_ENUM_H_ */