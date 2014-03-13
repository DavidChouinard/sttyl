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
    {VINTR, "intr"},
    {VQUIT, "vquit"},
    {VERASE, "erase"},
    {VEOF, "veof"},
    {VEOL, "veol"},
    {VEOL2, "veol2"},
    {VSTART, "vstart"},
    {VSTOP, "vstop"},
    {VMIN, "min"},
    {VTIME, "time"},
    {0, NULL}
};

// A subset of relevant flags
const struct trecord input_flags[] = {
    /*{IGNBRK, "ignbrk"},*/
    /*{BRKINT, "brkint"},*/
    /*{IGNPAR, "ignpar"},*/
    /*{PARMRK, "parmrk"},*/
    /*{INPCK, "inpck"},*/
    /*{ISTRIP, "istrip"},*/
    /*{INLCR, "inlcr"},*/
    /*{IGNCR, "igncr"},*/
    /*{IUCLC, "iuclc"},*/
    /*{IXON, "ixon"},*/
    /*{IXANY, "ixany"},*/
    /*{IXOFF, "ixoff"},*/
    /*{IMAXBEL, "imaxbel"},*/
    /*{IUTF8, "iutf8"},*/
    {ICRNL, "icrnl"},
    {0, NULL}
};

const struct trecord output_flags[] = {
    {ONLCR, "onlcr"},
    {OLCUC, "olcuc"}, // Not standard in POSIX
    {0, NULL}
};

const struct trecord local_flags[] = {
    {ECHO, "echo"},
    {ECHOE, "echoe"},
    {ISIG, "isig"},
    {ICANON, "icanon"},
    {0, NULL}
};

tcflag_t get_flag_mask(char *flag_name, const struct trecord *table) {
    for (int i = 0; table[i].name != NULL; i++) {
        if (strcmp(table[i].name, flag_name) == 0)
            return table[i].value;
    }

    return -1;
}
