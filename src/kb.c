#include "../include/kb.h"

char scancode_to_char(uint8 scancode) {
    switch (scancode) {
        case 2: return '1';
        case 3: return '2';
        case 4: return '3';
        case 5: return '4';
        case 6: return '5';
        case 7: return '6';
        case 8: return '7';
        case 9: return '8';
        case 10: return '9';
        case 11: return '0';
        case 12: return '-';
        case 13: return '=';
        case 14: return '\b';
        case 16: return 'q';
        case 17: return 'w';
        case 18: return 'e';
        case 19: return 'r';
        case 20: return 't';
        case 21: return 'y';
        case 22: return 'u';
        case 23: return 'i';
        case 24: return 'o';
        case 25: return 'p';
        case 26: return '[';
        case 27: return ']';
        case 28: return '\n';
        case 30: return 'a';
        case 31: return 's';
        case 32: return 'd';
        case 33: return 'f';
        case 34: return 'g';
        case 35: return 'h';
        case 36: return 'j';
        case 37: return 'k';
        case 38: return 'l';
        case 39: return ';';
        case 40: return '\'';
        case 41: return '`';
        case 44: return 'z';
        case 45: return 'x';
        case 46: return 'c';
        case 47: return 'v';
        case 48: return 'b';
        case 49: return 'n';
        case 50: return 'm';
        case 51: return ',';
        case 52: return '.';
        case 53: return '/';
        case 54: return '.';
        case 55: return '/';
        case 57: return ' ';
        default: return 0;
    }
}

string readStr() {
    string buffstr = (string) malloc(200);
    uint8 i = 0;
    uint8 reading = 1;
    uint8 ctrl_pressed = 0;

    while (reading) {
        if (inportb(0x64) & 0x1) {
            uint8 scancode = inportb(0x60);
            if (scancode >= 0x80) {
                if (scancode == 0x9D || scancode == 0xE0 + 0x80) {
                    ctrl_pressed = 0;
                }
                continue;
            }
            if (scancode == 0x1D || scancode == 0xE0) {
                ctrl_pressed = 1;
                continue;
            }
            if (ctrl_pressed) {
                if (scancode == 31) {
                    buffstr[0] = '\x01';
                    buffstr[1] = 0;
                    reading = 0;
                    continue;
                } else if (scancode == 45 || scancode == 0x2D || scancode == 0xAD) { // Ctrl+X
                    buffstr[0] = '\x02';
                    buffstr[1] = 0;
                    reading = 0;
                    continue;
                }
                continue;
            }
            char c = scancode_to_char(scancode);
            if (c == 0) continue;
            if (c == '\n') {
                buffstr[i] = '\n';
                buffstr[i + 1] = 0;
                reading = 0;
            } else if (c == '\b') {
                if (i > 0) {
                    i--;
                    buffstr[i] = 0;
                    printch('\b');
                }
            } else if (i < 199) {
                buffstr[i] = c;
                printch(c);
                i++;
            }
        }
    }
    buffstr[i] = 0;
    return buffstr;
}

void kb_handler(uint8 scancode) {
}