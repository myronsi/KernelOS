#ifndef KB_H
#define KB_H
#include "screen.h"
#include "util.h"

typedef char* string;
string readStr();
char scancode_to_char(uint8 scancode);
void kb_handler(uint8 scancode);

#endif
