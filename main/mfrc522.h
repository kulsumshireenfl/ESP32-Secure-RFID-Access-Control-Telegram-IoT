#pragma once
#include <stdint.h>
void mfrc522_init(void);
int mfrc522_read_uid(uint8_t *uid, uint8_t *len);