#ifndef TABLES_H
#define TABLES_H

// Table record
struct trecord {tcflag_t value; char *name; };

// Flag record (containing termios offset and table)
struct frecord {const struct trecord *table; size_t offset; };

extern const struct trecord baud_rates[];
extern const struct trecord control_characters[];
extern const struct trecord input_flags[];
extern const struct trecord output_flags[];
extern const struct trecord local_flags[];
extern const struct frecord flag_types[];
extern size_t flag_offsets[];

extern struct trecord get_flag_mask(char *name, const struct trecord *table);

#endif
