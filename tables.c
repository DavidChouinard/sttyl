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
    {VKILL, "kill"},
    {VERASE, "erase"},
    {0, NULL}
};

// A subset of relevant flags
const struct trecord input_flags[] = {
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

/*
 * Peforms an O(n) sequential search on the passed table for a record matching
 * the passed name paramater. Returns a {0, NULL} record if not found.
 *
 * Arguments:
 *    char *flag_name: flag name to search for
 *    const struct trecord *table: table to search
 */
struct trecord get_flag_mask(char *name, const struct trecord *table) {
    for (int i = 0; table[i].name != NULL; i++) {
        if (strcmp(table[i].name, name) == 0)
            return table[i];
    }

    // Didn't find a matching record
    return (struct trecord) {0, NULL};
}
