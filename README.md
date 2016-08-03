# librdata - Read and write R data frames from C

Originally part of [ReadStat](https://github.com/WizardMac/librdata), librdata
is a small C library for reading and writing R data frames.

Features:

* Read both RData and RDS formats
* Read compressed files (requires zlib and lzma)
* Write factors, timestamps, logical vectors, and more

## Read API

Example usage:

```{C}

static int handle_table(const char *name, void *ctx) {
    printf("Read table: %s\n", name);
}

static int handle_column(const char *name, rdata_type_t type,
                         void *data, long count, void *ctx) {
    // Do something...
}

rdata_parser_t *parser = rdata_parser_init();

rdata_set_table_handler(parser, &handle_table);
rdata_set_column_handler(parser, &handle_column);

rdata_parse(parser, "/path/to/something.rdata", NULL);
```

## Write API

Example usage:

```{C}
static ssize_t write_data(const void *bytes, size_t len, void *ctx) {
    int fd = *(int *)ctx;
    return write(fd, bytes, len);
}

int row_count = 3;
int fd = open("/path/to/somewhere.rdata", O_WRONLY, 0644);
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
rdata_append_real_value(writer, "hello");
rdata_append_real_value(writer, "goodbye");
rdata_append_real_value(writer, NULL);
rdata_end_column(writer, col2);

rdata_end_table(writer, "My data set");
rdata_end_file(writer);

close(fd);

```
