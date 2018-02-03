# librdata - Read and write R data frames from C

Originally part of [ReadStat](https://github.com/WizardMac/ReadStat), librdata
is a small C library for reading and writing R data frames.

Features:

* Read both RData and RDS formats
* Read compressed files (requires zlib and lzma)
* Write factors, timestamps, logical vectors, and more

## Read API

Example usage:

```{C}
#include "rdata.h"

static int handle_table(const char *name, void *ctx) {
    printf("Read table: %s\n", name);

    return 0; /* non-zero to abort processing */
}

// Called once for all columns with the following caveats:
// * `name` is NULL for some columns (see handle_column_name below)
// * `data` is NULL for text columns (see handle_text_value below)
static int handle_column(const char *name, rdata_type_t type,
                         void *data, long count, void *ctx) {
    /* Do something... */
    return 0;
}

// Some column names appear in the file after the data
static int handle_column_name(const char *name, int index, void *ctx) {
    if (debug) printf("Read column name: %s\n", name);
    /* Do something... */
    return 0;
}

// Called once per row for a text column
static int handle_text_value(const char *value, int index, void *ctx) {
    /* Do something... */
    return 0;
}

// Called for factor variables, once for each level
static int handle_value_label(const char *value, int index, void *ctx) {
    /* Do something... */
    return 0;
}

rdata_parser_t *parser = rdata_parser_init();

rdata_set_table_handler(parser, &handle_table);
rdata_set_column_handler(parser, &handle_column);
rdata_set_text_value_handler(parser, &handle_text_value);
rdata_set_value_label_handler(parser, &handle_value_label);

rdata_parse(parser, "/path/to/something.rdata", NULL);
```

See `rdata.h` for the full API.

## Write API

Example usage:

```{C}
#include "rdata.h"

static ssize_t write_data(const void *bytes, size_t len, void *ctx) {
    int fd = *(int *)ctx;
    return write(fd, bytes, len);
}

int row_count = 3;
int fd = open("/path/to/somewhere.rdata", O_CREAT | O_WRONLY, 0644);
rdata_writer_t *writer = rdata_writer_init(&write_data);

rdata_column_t *col1 = rdata_add_column(writer, "column1", RDATA_TYPE_REAL);
rdata_column_t *col2 = rdata_add_column(writer, "column2", RDATA_TYPE_STRING);

rdata_begin_file(writer, &fd);
rdata_begin_table(writer, "my_table", row_count);

rdata_begin_column(writer, col1);
rdata_append_real_value(writer, 0.0);
rdata_append_real_value(writer, 100.0);
rdata_append_real_value(writer, NAN);
rdata_end_column(writer, col1);

rdata_begin_column(writer, col2);
rdata_append_string_value(writer, "hello");
rdata_append_string_value(writer, "goodbye");
rdata_append_string_value(writer, NULL);
rdata_end_column(writer, col2);

rdata_end_table(writer, "My data set");
rdata_end_file(writer);

close(fd);

```

See `rdata.h` for the full API.
