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

void writeRData() {
    int row_count = 3;
    int fd = open("example.RData", O_CREAT | O_WRONLY, 0644);
    rdata_writer_t *writer = rdata_writer_init(&write_data, RDATA_WORKSPACE);

    rdata_column_t *col1 = rdata_add_column(writer, "column1", RDATA_TYPE_REAL);
    rdata_column_t *col2 = rdata_add_column(writer, "column2", RDATA_TYPE_STRING);

    rdata_begin_file(writer, &fd);
    rdata_begin_table(writer, "my_table");

    rdata_begin_column(writer, col1, row_count);
    rdata_append_real_value(writer, 0.0);
    rdata_append_real_value(writer, 100.0);
    rdata_append_real_value(writer, NAN);
    rdata_end_column(writer, col1);

    rdata_begin_column(writer, col2, row_count);
    rdata_append_string_value(writer, "hello");
    rdata_append_string_value(writer, "goodbye");
    rdata_append_string_value(writer, NULL);
    rdata_end_column(writer, col2);

    rdata_end_table(writer, row_count, "My data set");
    rdata_end_file(writer);

    close(fd);
}

void writeRDS() {
    int row_count = 3;
    int fd = open("example.rds", O_CREAT | O_WRONLY, 0644);
    rdata_writer_t *writer = rdata_writer_init(&write_data, RDATA_SINGLE_OBJECT);

    rdata_column_t *col = rdata_add_column(writer, "column1", RDATA_TYPE_REAL);

    rdata_begin_file(writer, &fd);

    rdata_begin_column(writer, col, row_count);
    rdata_append_real_value(writer, 42.0);
    rdata_append_real_value(writer, -7.0);
    rdata_append_real_value(writer, NAN);
    rdata_end_column(writer, col);

    rdata_end_file(writer);

    close(fd);
}

int main() {
    writeRData();
    writeRDS();
    printf("Done\n");
    exit(0);
}
