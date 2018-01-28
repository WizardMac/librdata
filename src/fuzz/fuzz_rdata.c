#include <stdlib.h>
#include <string.h>

#include "../rdata.h"

typedef struct buffer {
    const unsigned char *data;
    size_t len;
    ssize_t pos;
} buffer_t;

int open_handler(const char *path, void *io_ctx) {
    return 0;
}

int close_handler(void *io_ctx) {
    return 0;
}

rdata_off_t seek_handler(rdata_off_t offset, rdata_io_flags_t whence, void *io_ctx) {
    buffer_t *buffer = (buffer_t *)io_ctx;
    rdata_off_t newpos = 0;
    if (whence == RDATA_SEEK_SET) {
        newpos = offset;
    } else if (whence == RDATA_SEEK_CUR) {
        newpos = buffer->pos + offset;
    } else if (whence == RDATA_SEEK_END) {
        newpos = buffer->len + offset;
    }
    if (newpos > buffer->len || newpos < 0) {
        return -1;
    }
    return (buffer->pos = newpos);
}

ssize_t read_handler(void *buf, size_t nbyte, void *io_ctx) {
    buffer_t *buffer = (buffer_t *)io_ctx;
    if (nbyte > buffer->len - buffer->pos) {
        nbyte = buffer->len - buffer->pos;
    }
    memcpy(buf, &buffer->data[buffer->pos], nbyte);
    buffer->pos += nbyte;
    return nbyte;
}

rdata_error_t update_handler(long file_size, rdata_progress_handler progress_handler, void *user_ctx, void *io_ctx) {
    return RDATA_OK;
}

int table_handler(const char *name, void *ctx) {
    return 0;
}

int column_handler(const char *name, rdata_type_t type, void *data, long count, void *ctx) {
    return 0;
}

int column_name_handler(const char *value, int index, void *ctx) {
    return 0;
}

int text_value_handler(const char *value, int index, void *ctx) {
    return 0;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    buffer_t buffer = {
        .data = Data,
        .len = Size
    };
    rdata_parser_t *parser = rdata_parser_init();

    rdata_set_table_handler(parser, &table_handler);
    rdata_set_column_handler(parser, &column_handler);
    rdata_set_column_name_handler(parser, &column_name_handler);
    rdata_set_text_value_handler(parser, &text_value_handler);
    rdata_set_value_label_handler(parser, &text_value_handler);

    rdata_set_open_handler(parser, &open_handler);
    rdata_set_close_handler(parser, &close_handler);
    rdata_set_seek_handler(parser, &seek_handler);
    rdata_set_read_handler(parser, &read_handler);
    rdata_set_update_handler(parser, &update_handler);
    rdata_set_io_ctx(parser, &buffer);

    rdata_parse(parser, NULL, NULL);

    rdata_parser_free(parser);
    return 0;
}
