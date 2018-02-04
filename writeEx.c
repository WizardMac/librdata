#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <math.h>
#include <fcntl.h>

#include <rdata.h>

static ssize_t write_data(const void *bytes, size_t len, void *ctx) {
    int fd = *(int *)ctx;
    return write(fd, bytes, len);
}

int main() {
    int row_count = 3;
    int fd = open("somewhere.rdata", O_CREAT | O_WRONLY, 0644);
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

    printf("Done\n");
    exit(0);
}
