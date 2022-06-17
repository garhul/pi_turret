#include <stdio.h>
#include <inttypes.h>


struct tt {
  uint8_t a;
  uint8_t b: 4;
  uint8_t c: 4;
};

union foo {
    uint8_t x : 4;
    uint8_t y : 2;
    uint8_t z;
  };

int main( void) {
  tt data = {
   .a=5,
   .b=255,
   .c=3,
  };  
  
  uint8_t buffer[3] = {0x05, 0xFF, 0x03};

  tt *da = (tt*) *buffer;

  printf("a:%d b:%d c:%d \n", data.a, data.b, data.c);  
  printf("a:%d b:%d c:%d \n", da->a, da->b, da->c);  
}