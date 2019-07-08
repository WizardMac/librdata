#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>

#include "../rdata.h"

#include "test_buffer.h"
#include "test_buffer_io.h"

#ifdef _WIN32
#define timegm _mkgmtime
#endif

typedef struct test_rdata_ctx_s {
    int column_count;
    int row_count;
    const char *table_name;
    time_t timestamp;
    struct tm date;
} test_rdata_ctx_t;

static void handle_error(const char *error_message, void *ctx) {
    printf("%s\n", error_message);
}

static ssize_t write_data(const void *bytes, size_t len, void *ctx) {
    rt_buffer_t *buffer = (rt_buffer_t *)ctx;
    buffer_grow(buffer, len);
    if (buffer->bytes == NULL) {
        return -1;
    }
    memcpy(buffer->bytes + buffer->used, bytes, len);
    buffer->used += len;
    return len;
}

static int handle_table(const char *name, void *ctx) {
    test_rdata_ctx_t *test_ctx = (test_rdata_ctx_t *)ctx;
    return (name != NULL && strcmp(name, test_ctx->table_name) != 0);
}

static int handle_column_name(const char *name, int index, void *ctx) {
    if (index == 0)
        return strcmp(name, "column1");
    if (index == 1)
        return strcmp(name, "column2");
    if (index == 2)
        return strcmp(name, "column3");
    if (index == 3)
        return strcmp(name, "column4");

    return 1;
}

static int handle_column(const char *name, rdata_type_t type,
                         void *data, long count, void *ctx) {
    test_rdata_ctx_t *test_ctx = (test_rdata_ctx_t *)ctx;
    if (name != NULL && strcmp(name, "column1") == 0) {
        if (type != RDATA_TYPE_REAL)
            return 1;
    } else if (name != NULL && strcmp(name, "column2") == 0) {
        return (type != RDATA_TYPE_STRING);
    } else if (name != NULL && strcmp(name, "column3") == 0) {
        if (type != RDATA_TYPE_TIMESTAMP)
            return 1;
    } else if (name != NULL && strcmp(name, "column4") == 0) {
        if (type != RDATA_TYPE_DATE)
            return 1;
    } else if (name != NULL) {
        return 1;
    }

    if (data == NULL)
        return 0;

    double *dp = data;

    if (count != test_ctx->row_count) {
        printf("Unexpected row count: %ld\n", count);
        return 1;
    }

    if (type == RDATA_TYPE_REAL) {
        if (dp[0] != 0.0) {
            printf("Unexpected real value[0]: %lf\n", dp[0]);
            return 1;
        }

        if (dp[1] != 100.0) {
            printf("Unexpected real value[1]: %lf\n", dp[1]);
            return 1;
        }

        if (!isnan(dp[2])) {
            printf("Unexpected real value[2]: %lf\n", dp[2]);
            return 1;
        }
    }
    if (type == RDATA_TYPE_TIMESTAMP) {
        int i;
        for (i=0; i<3; i++) {
            if (dp[i] != test_ctx->timestamp) {
                printf("Unexpected timestamp value[%d]: %lf\n", i, dp[i]);
                return 1;
            }
        }
    }
    if (type == RDATA_TYPE_DATE) {
        int i;
        for (i=0; i<3; i++) {
            if (dp[i] * 86400 != timegm(&test_ctx->date)) {
                printf("Unexpected date value[%d]: %lf\n", i, dp[i]);
                return 1;
            }
        }
    }

    return 0;
}

static int handle_text_value(const char *value, int index, void *ctx) {
    if (index == 0)
        return strcmp(value, "hello");
    if (index == 1)
        return strcmp(value, "goodbye");
    if (index == 2)
        return value != NULL;

    return 1;
}

int main(int argc, char *argv[]) {
    struct timeval time;
    gettimeofday(&time, NULL);

    test_rdata_ctx_t ctx = { .row_count = 3, .table_name = "table1",
        .timestamp = time.tv_sec, .date = { .tm_year = 95, .tm_mon = 7, .tm_mday = 15 } };
    rt_buffer_t *buffer = buffer_init();
    rdata_writer_t *writer = rdata_writer_init(&write_data, RDATA_WORKSPACE);
    rdata_column_t *col1 = rdata_add_column(writer, "column1", RDATA_TYPE_REAL);
    rdata_column_t *col2 = rdata_add_column(writer, "column2", RDATA_TYPE_STRING);
    rdata_column_t *col3 = rdata_add_column(writer, "column3", RDATA_TYPE_TIMESTAMP);
    rdata_column_t *col4 = rdata_add_column(writer, "column4", RDATA_TYPE_DATE);

    rdata_begin_file(writer, buffer);
    rdata_begin_table(writer, ctx.table_name);

    rdata_begin_column(writer, col1, ctx.row_count);
    rdata_append_real_value(writer, 0.0);
    rdata_append_real_value(writer, 100.0);
    rdata_append_real_value(writer, NAN);
    rdata_end_column(writer, col1);

    rdata_begin_column(writer, col2, ctx.row_count);
    rdata_append_string_value(writer, "hello");
    rdata_append_string_value(writer, "goodbye");
    rdata_append_string_value(writer, NULL);
    rdata_end_column(writer, col2);

    rdata_begin_column(writer, col3, ctx.row_count);
    rdata_append_timestamp_value(writer, ctx.timestamp);
    rdata_append_timestamp_value(writer, ctx.timestamp);
    rdata_append_timestamp_value(writer, ctx.timestamp);
    rdata_end_column(writer, col3);

    rdata_begin_column(writer, col4, ctx.row_count);
    rdata_append_date_value(writer, &ctx.date);
    rdata_append_date_value(writer, &ctx.date);
    rdata_append_date_value(writer, &ctx.date);
    rdata_end_column(writer, col4);

    rdata_end_table(writer, ctx.row_count, "My data set");
    rdata_end_file(writer);

    rt_buffer_ctx_t *buffer_ctx = buffer_ctx_init(buffer);

    rdata_parser_t *parser = rdata_parser_init();
    rdata_set_open_handler(parser, rt_open_handler);
    rdata_set_close_handler(parser, rt_close_handler);
    rdata_set_seek_handler(parser, rt_seek_handler);
    rdata_set_read_handler(parser, rt_read_handler);
    rdata_set_update_handler(parser, rt_update_handler);
    rdata_set_io_ctx(parser, buffer_ctx);

    rdata_set_table_handler(parser, &handle_table);
    rdata_set_column_handler(parser, &handle_column);
    rdata_set_column_name_handler(parser, &handle_column_name);
    rdata_set_text_value_handler(parser, &handle_text_value);

    rdata_error_t err = rdata_parse(parser, "example.RData", &ctx);

    if (err != RDATA_OK) {
        printf("Returned: %s\n", rdata_error_message(err));
        const char *path = "/tmp/rdata_test.RData";
        int fd = open("/tmp/rdata_test.RData", O_CREAT | O_TRUNC | O_WRONLY, 0644);
        write(fd, buffer->bytes, buffer->used);
        close(fd);
        printf("Wrote test file out to %s\n", path);
    }

    return (err != RDATA_OK);
}
