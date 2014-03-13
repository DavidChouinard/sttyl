#ifndef TABLES_H
#define TABLES_H

struct trecord {tcflag_t value; char *name; };

extern const struct trecord baud_rates[];
extern const struct trecord control_characters[];
extern const struct trecord input_flags[];
extern const struct trecord output_flags[];
extern const struct trecord local_flags[];

extern tcflag_t get_flag_mask(char *name, const struct trecord *table);

#endif
