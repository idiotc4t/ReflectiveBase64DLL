#include <stdio.h>

unsigned int b64_int(unsigned int ch);
unsigned int b64e_size(unsigned int in_size);


unsigned int b64d_size(unsigned int in_size);

unsigned int b64_encode(const unsigned char* in, unsigned int in_len, unsigned char* out);


unsigned int b64_decode(const unsigned char* in, unsigned int in_len, unsigned char* out);

