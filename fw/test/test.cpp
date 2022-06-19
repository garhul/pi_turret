#include <stdio.h>
#include <inttypes.h>


uint16_t num = 1984;

void toBytes(uint16_t i, uint8_t buff[]) {
  buff[0] = i >> 8 & 0xFF;
  buff[1] = i & 0xff;  
}

 uint8_t getByte(uint16_t i, uint8_t index) {
    return (i >> (index *8)) & 0xFF;
  }


int main(void) {
  uint8_t b[2];
  toBytes(num, b);
  
  printf("%lu\n", sizeof(num));  
  printf("%d - %d\n", b[0], b[1]);
  printf("%d - %d\n", getByte(num, 1), getByte(num, 0)); //little endian
  printf("%d\n", (b[0] << 8) + b[1]);
}
