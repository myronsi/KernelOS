#include "../include/kb.h"

string readStr()
{
    string buffstr = (string) malloc(200);
    uint8 i = 0;
    uint8 reading = 1;
    uint8 ctrl_pressed = 0;

    while (reading)
    {
        if (inportb(0x64) & 0x1)
        {
            uint8 scancode = inportb(0x60);

            if (scancode == 0x1D || scancode == 0xE0) {
                ctrl_pressed = 1;
                continue;
            } else if (scancode == 0x9D || scancode == 0xE0 + 0x80) {
                ctrl_pressed = 0;
                continue;
            }

            if (ctrl_pressed) {
                if (scancode == 31) {
                    buffstr[0] = '\x01';
                    buffstr[1] = 0;
                    reading = 0;
                    continue;
                } else if (scancode == 45 || scancode == 0x2D || scancode == 0xAD) {
                    buffstr[0] = '\x02';
                    buffstr[1] = 0;
                    reading = 0;
                    continue;
                }
                continue;
            }

            switch (scancode)
            {
                case 2:
                    if (i < 199) {
                        printch('1');
                        buffstr[i] = '1';
                        i++;
                    }
                    break;
                case 3:
                    if (i < 199) {
                        printch('2');
                        buffstr[i] = '2';
                        i++;
                    }
                    break;
                case 4:
                    if (i < 199) {
                        printch('3');
                        buffstr[i] = '3';
                        i++;
                    }
                    break;
                case 5:
                    if (i < 199) {
                        printch('4');
                        buffstr[i] = '4';
                        i++;
                    }
                    break;
                case 6:
                    if (i < 199) {
                        printch('5');
                        buffstr[i] = '5';
                        i++;
                    }
                    break;
                case 7:
                    if (i < 199) {
                        printch('6');
                        buffstr[i] = '6';
                        i++;
                    }
                    break;
                case 8:
                    if (i < 199) {
                        printch('7');
                        buffstr[i] = '7';
                        i++;
                    }
                    break;
                case 9:
                    if (i < 199) {
                        printch('8');
                        buffstr[i] = '8';
                        i++;
                    }
                    break;
                case 10:
                    if (i < 199) {
                        printch('9');
                        buffstr[i] = '9';
                        i++;
                    }
                    break;
                case 11:
                    if (i < 199) {
                        printch('0');
                        buffstr[i] = '0';
                        i++;
                    }
                    break;
                case 12:
                    if (i < 199) {
                        printch('-');
                        buffstr[i] = '-';
                        i++;
                    }
                    break;
                case 13:
                    if (i < 199) {
                        printch('=');
                        buffstr[i] = '=';
                        i++;
                    }
                    break;
                case 14:
                    printch('\b');
                    if (i > 0) {
                        i--;
                        buffstr[i] = 0;
                    }
                    break;
                case 16:
                    if (i < 199) {
                        printch('q');
                        buffstr[i] = 'q';
                        i++;
                    }
                    break;
                case 17:
                    if (i < 199) {
                        printch('w');
                        buffstr[i] = 'w';
                        i++;
                    }
                    break;
                case 18:
                    if (i < 199) {
                        printch('e');
                        buffstr[i] = 'e';
                        i++;
                    }
                    break;
                case 19:
                    if (i < 199) {
                        printch('r');
                        buffstr[i] = 'r';
                        i++;
                    }
                    break;
                case 20:
                    if (i < 199) {
                        printch('t');
                        buffstr[i] = 't';
                        i++;
                    }
                    break;
                case 21:
                    if (i < 199) {
                        printch('y');
                        buffstr[i] = 'y';
                        i++;
                    }
                    break;
                case 22:
                    if (i < 199) {
                        printch('u');
                        buffstr[i] = 'u';
                        i++;
                    }
                    break;
                case 23:
                    if (i < 199) {
                        printch('i');
                        buffstr[i] = 'i';
                        i++;
                    }
                    break;
                case 24:
                    if (i < 199) {
                        printch('o');
                        buffstr[i] = 'o';
                        i++;
                    }
                    break;
                case 25:
                    if (i < 199) {
                        printch('p');
                        buffstr[i] = 'p';
                        i++;
                    }
                    break;
                case 26:
                    if (i < 199) {
                        printch('[');
                        buffstr[i] = '[';
                        i++;
                    }
                    break;
                case 27:
                    if (i < 199) {
                        printch(']');
                        buffstr[i] = ']';
                        i++;
                    }
                    break;
                case 28:
                    buffstr[i] = '\n';
                    buffstr[i + 1] = 0;
                    reading = 0;
                    break;
                case 30:
                    if (i < 199) {
                        printch('a');
                        buffstr[i] = 'a';
                        i++;
                    }
                    break;
                case 31:
                    if (i < 199) {
                        printch('s');
                        buffstr[i] = 's';
                        i++;
                    }
                    break;
                case 32:
                    if (i < 199) {
                        printch('d');
                        buffstr[i] = 'd';
                        i++;
                    }
                    break;
                case 33:
                    if (i < 199) {
                        printch('f');
                        buffstr[i] = 'f';
                        i++;
                    }
                    break;
                case 34:
                    if (i < 199) {
                        printch('g');
                        buffstr[i] = 'g';
                        i++;
                    }
                    break;
                case 35:
                    if (i < 199) {
                        printch('h');
                        buffstr[i] = 'h';
                        i++;
                    }
                    break;
                case 36:
                    if (i < 199) {
                        printch('j');
                        buffstr[i] = 'j';
                        i++;
                    }
                    break;
                case 37:
                    if (i < 199) {
                        printch('k');
                        buffstr[i] = 'k';
                        i++;
                    }
                    break;
                case 38:
                    if (i < 199) {
                        printch('l');
                        buffstr[i] = 'l';
                        i++;
                    }
                    break;
                case 39:
                    if (i < 199) {
                        printch(';');
                        buffstr[i] = ';';
                        i++;
                    }
                    break;
                case 40:
                    if (i < 199) {
                        printch('\'');
                        buffstr[i] = '\'';
                        i++;
                    }
                    break;
                case 41:
                    if (i < 199) {
                        printch('`');
                        buffstr[i] = '`';
                        i++;
                    }
                    break;
                case 44:
                    if (i < 199) {
                        printch('z');
                        buffstr[i] = 'z';
                        i++;
                    }
                    break;
                case 45:
                    if (i < 199) {
                        printch('x');
                        buffstr[i] = 'x';
                        i++;
                    }
                    break;
                case 46:
                    if (i < 199) {
                        printch('c');
                        buffstr[i] = 'c';
                        i++;
                    }
                    break;
                case 47:
                    if (i < 199) {
                        printch('v');
                        buffstr[i] = 'v';
                        i++;
                    }
                    break;
                case 48:
                    if (i < 199) {
                        printch('b');
                        buffstr[i] = 'b';
                        i++;
                    }
                    break;
                case 49:
                    if (i < 199) {
                        printch('n');
                        buffstr[i] = 'n';
                        i++;
                    }
                    break;
                case 50:
                    if (i < 199) {
                        printch('m');
                        buffstr[i] = 'm';
                        i++;
                    }
                    break;
                case 51:
                    if (i < 199) {
                        printch(',');
                        buffstr[i] = ',';
                        i++;
                    }
                    break;
                case 52:
                    if (i < 199) {
                        printch('.');
                        buffstr[i] = '.';
                        i++;
                    }
                    break;
                case 53:
                    if (i < 199) {
                        printch('/');
                        buffstr[i] = '/';
                        i++;
                    }
                    break;
                case 54:
                    if (i < 199) {
                        printch('.');
                        buffstr[i] = '.';
                        i++;
                    }
                    break;
                case 55:
                    if (i < 199) {
                        printch('/');
                        buffstr[i] = '/';
                        i++;
                    }
                    break;
                case 57:
                    if (i < 199) {
                        printch(' ');
                        buffstr[i] = ' ';
                        i++;
                    }
                    break;
            }
        }
    }
    buffstr[i] = 0;
    return buffstr;
}
