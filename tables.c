#include <stdio.h>
#include <string.h>
#include <termios.h>

#include "tables.h"

// Ideally, the values would be ints, but since we want to reuse the same
// infrastructure, we use strings
const struct trecord baud_rates[] = {
    {B0, "0"},
    {B50, "50"},
    {B75, "75"},
    {B110, "110"},
    {B134, "134"},
    {B150, "150"},
    {B200, "200"},
    {B300, "300"},
    {B600, "600"},
    {B1200, "1200"},
    {B1800, "1800"},
    {B2400, "2400"},
    {B4800, "4800"},
    {B9600, "9600"},
    {B19200, "19200"},
    {B38400, "38400"},
    {B57600, "57600"},
    {B115200, "115200"},
    {B230400, "230400"},
    {0, NULL}
};

// A subset of relevant control characters
const struct trecord control_characters[] = {
    {VERASE, "erase"},
    {VKILL, "kill"},
    {VINTR, "intr"},
    {0, NULL}
};

// A subset of relevant flags
const struct trecord input_flags[] = {
    {ICRNL, "icrnl"},
    {ICANON, "icanon"},
    {ISIG, "isig"},
    {0, NULL}
};

const struct trecord local_flags[] = {
    {ECHO, "echo"},
    {ECHOE, "echoe"},
    {0, NULL}
};

const struct trecord control_flags[] = {
    {OLCUC, "olcuc"}, // Not standard in POSIX
    {0, NULL}
};


// TODO: output flags?

/*const struct trecord tables[][8] = {input_flags, local_flags, control_flags};*/

int get_flag_mask(char *flag_name, const struct trecord *table) {
    /*symbol = */
    // TODO: lowercase

    for (int i = 0; table[i].name != NULL; i++) {
        if (strcmp(table[i].name, flag_name) == 0)
            return table[i].value;
    }

    return -1;
}
