#include <stdint.h>
#include <stdlib.h>

#define MOD_ADLER 65521
uint32_t adler32(uint8_t *data, size_t len);
int toHex(uint8_t *buf, int len, char *out);

