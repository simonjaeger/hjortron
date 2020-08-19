#include <stdbool.h>
#include "drivers/keyboard.h"
#include "cpu/io.h"
#include "cpu/irq.h"
#include "debug.h"
#include "display/display.h"

enum key_code
{
    NONE,
    RETURN,
    SPACE,
    LEFT_SHIFT,
    RIGHT_SHIFT,
    PERIOD,
    COMMA,
    DASH,
    A,
    B,
    C,
    D,
    E,
    F,
    G,
    H,
    I,
    J,
    K,
    L,
    M,
    N,
    O,
    P,
    Q,
    R,
    S,
    T,
    U,
    V,
    W,
    X,
    Y,
    Z,
    NUM_0,
    NUM_1,
    NUM_2,
    NUM_3,
    NUM_4,
    NUM_5,
    NUM_6,
    NUM_7,
    NUM_8,
    NUM_9,
    ARROW_LEFT,
    ARROW_RIGHT,
    ARROW_UP,
    ARROW_DOWN
};

char get_char(enum key_code kc, bool upper)
{
    switch (kc)
    {
    case PERIOD:
        return '.';
    case COMMA:
        return ',';
    case DASH:
        return '-';
    case SPACE:
        return ' ';
    case RETURN:
        return '\n';
    case Q:
        return upper ? 'Q' : 'q';
    case W:
        return upper ? 'W' : 'w';
    case E:
        return upper ? 'E' : 'e';
    case R:
        return upper ? 'R' : 'r';
    case T:
        return upper ? 'T' : 't';
    case Y:
        return upper ? 'Y' : 'y';
    case U:
        return upper ? 'U' : 'u';
    case I:
        return upper ? 'I' : 'i';
    case O:
        return upper ? 'O' : 'o';
    case P:
        return upper ? 'P' : 'p';
    case A:
        return upper ? 'A' : 'a';
    case S:
        return upper ? 'S' : 's';
    case D:
        return upper ? 'D' : 'd';
    case F:
        return upper ? 'F' : 'f';
    case G:
        return upper ? 'G' : 'g';
    case H:
        return upper ? 'H' : 'h';
    case J:
        return upper ? 'J' : 'j';
    case K:
        return upper ? 'K' : 'k';
    case L:
        return upper ? 'L' : 'l';
    case Z:
        return upper ? 'Z' : 'z';
    case X:
        return upper ? 'X' : 'x';
    case C:
        return upper ? 'C' : 'c';
    case V:
        return upper ? 'V' : 'v';
    case B:
        return upper ? 'B' : 'b';
    case N:
        return upper ? 'N' : 'n';
    case M:
        return upper ? 'M' : 'm';
    case NUM_1:
        return '1';
    case NUM_2:
        return '2';
    case NUM_3:
        return '3';
    case NUM_4:
        return '4';
    case NUM_5:
        return '5';
    case NUM_6:
        return '6';
    case NUM_7:
        return '7';
    case NUM_8:
        return '8';
    case NUM_9:
        return '9';
    case NUM_0:
        return '0';
    default:
        return '\0';
    }
}

void handle_irq(__attribute__((unused)) const regs *r)
{
    uint8_t key = inb(0x60);
    enum key_code kc = NONE;

    switch (key)
    {
    /* Ignored scan codes. */
    case 0xFA:
    case 0x45:
    case 0xC5:
        break;

    /* Mapped scan codes. */
    case 0x02:
        kc = NUM_1;
        break;
    case 0x03:
        kc = NUM_2;
        break;
    case 0x04:
        kc = NUM_3;
        break;
    case 0x05:
        kc = NUM_4;
        break;
    case 0x06:
        kc = NUM_5;
        break;
    case 0x07:
        kc = NUM_6;
        break;
    case 0x08:
        kc = NUM_7;
        break;
    case 0x09:
        kc = NUM_8;
        break;
    case 0x0A:
        kc = NUM_9;
        break;
    case 0x0B:
        kc = NUM_0;
        break;

    case 0x10:
        kc = Q;
        break;
    case 0x11:
        kc = W;
        break;
    case 0x12:
        kc = E;
        break;
    case 0x13:
        kc = R;
        break;
    case 0x14:
        kc = T;
        break;
    case 0x15:
        kc = Y;
        break;
    case 0x16:
        kc = U;
        break;
    case 0x17:
        kc = I;
        break;
    case 0x18:
        kc = O;
        break;
    case 0x19:
        kc = P;
        break;
    case 0x1E:
        kc = A;
        break;
    case 0x1F:
        kc = S;
        break;
    case 0x20:
        kc = D;
        break;
    case 0x21:
        kc = F;
        break;
    case 0x22:
        kc = G;
        break;
    case 0x23:
        kc = H;
        break;
    case 0x24:
        kc = J;
        break;
    case 0x25:
        kc = K;
        break;
    case 0x26:
        kc = L;
        break;
    case 0x2C:
        kc = Z;
        break;
    case 0x2D:
        kc = X;
        break;
    case 0x2E:
        kc = C;
        break;
    case 0x2F:
        kc = V;
        break;
    case 0x30:
        kc = B;
        break;
    case 0x31:
        kc = N;
        break;
    case 0x32:
        kc = M;
        break;

    case 0x33:
        kc = COMMA;
        break;
    case 0x34:
        kc = PERIOD;
        break;
    case 0x35:
        kc = DASH;
        break;
    case 0x1C:
        kc = RETURN;
        break;
    case 0x39:
        kc = SPACE;
        break;

    case 0x4B:
        kc = ARROW_LEFT;
        break;
    case 0x4D:
        kc = ARROW_RIGHT;
        break;
    case 0x48:
        kc = ARROW_UP;
        break;
    case 0x50:
        kc = ARROW_DOWN;
        break;
    }

    putc(get_char(kc, false));
}

void keyboard_init()
{
    irq_init_handler(0x21, handle_irq);

    while (inb(0x64) & 0x1)
    {
        inb(0x60);
    }

    outb(0x64, 0xAE);
    outb(0x64, 0x20);
    uint8_t status = (inb(0x60) | 1) & ~0x10;
    outb(0x64, 0x60);
    outb(0x60, status);
    outb(0x60, 0xF4);
}