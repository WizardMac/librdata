
#include <stdint.h>
#include <sys/types.h>
#include <stdio.h>

typedef enum rdata_type_e {
    RDATA_TYPE_STRING,
    RDATA_TYPE_DOUBLE
} rdata_type_t;

typedef enum rdata_error_e {
    RDATA_OK,
    RDATA_ERROR_OPEN = 1,
    RDATA_ERROR_SEEK,
    RDATA_ERROR_READ,
    RDATA_ERROR_MALLOC,
    RDATA_ERROR_USER_ABORT,
    RDATA_ERROR_PARSE
} rdata_error_t;

typedef int (*rdata_column_handler)(const char *name, rdata_type_t type, char *format, 
        void *data, long count, void *ctx);
typedef int (*rdata_table_handler)(const char *name, void *ctx);
typedef int (*rdata_text_value_handler)(const char *value, int index, void *ctx);
typedef int (*rdata_column_name_handler)(const char *value, int index, void *ctx);
typedef void (*rdata_error_handler)(const char *error_message, void *ctx);
typedef int (*rdata_progress_handler)(double progress, void *ctx);

#if defined _WIN32 || defined __CYGWIN__
typedef _off64_t rdata_off_t;
#elif defined _AIX
typedef off64_t rdata_off_t;
#else
typedef off_t rdata_off_t;
#endif

typedef enum rdata_io_flags_e {
    RDATA_SEEK_SET,
    RDATA_SEEK_CUR,
    RDATA_SEEK_END
} rdata_io_flags_t;

typedef int (*rdata_open_handler)(const char *path, void *io_ctx);
typedef int (*rdata_close_handler)(void *io_ctx);
typedef rdata_off_t (*rdata_seek_handler)(rdata_off_t offset, rdata_io_flags_t whence, void *io_ctx);
typedef ssize_t (*rdata_read_handler)(void *buf, size_t nbyte, void *io_ctx);
typedef rdata_error_t (*rdata_update_handler)(long file_size, rdata_progress_handler progress_handler, void *user_ctx, void *io_ctx);

typedef struct rdata_io_s {
    rdata_open_handler          open;
    rdata_close_handler         close;
    rdata_seek_handler          seek;
    rdata_read_handler          read;
    rdata_update_handler        update;
    void                          *io_ctx;
    int                            external_io;
} rdata_io_t;

typedef struct rdata_parser_s {
    rdata_table_handler         table_handler;
    rdata_column_handler        column_handler;
    rdata_column_name_handler   column_name_handler;
    rdata_text_value_handler    text_value_handler;
    rdata_text_value_handler    value_label_handler;
    rdata_error_handler         error_handler;
    rdata_io_t                 *io;
} rdata_parser_t;

rdata_parser_t *rdata_parser_init();
void rdata_parser_free(rdata_parser_t *parser);

rdata_error_t rdata_set_table_handler(rdata_parser_t *parser, rdata_table_handler table_handler);
rdata_error_t rdata_set_column_handler(rdata_parser_t *parser, rdata_column_handler column_handler);
rdata_error_t rdata_set_column_name_handler(rdata_parser_t *parser, rdata_column_name_handler column_name_handler);
rdata_error_t rdata_set_text_value_handler(rdata_parser_t *parser, rdata_text_value_handler text_value_handler);
rdata_error_t rdata_set_value_label_handler(rdata_parser_t *parser, rdata_text_value_handler value_label_handler);
rdata_error_t rdata_set_error_handler(rdata_parser_t *parser, rdata_error_handler error_handler);
rdata_error_t rdata_set_open_handler(rdata_parser_t *parser, rdata_open_handler open_handler);
rdata_error_t rdata_set_close_handler(rdata_parser_t *parser, rdata_close_handler close_handler);
rdata_error_t rdata_set_seek_handler(rdata_parser_t *parser, rdata_seek_handler seek_handler);
rdata_error_t rdata_set_read_handler(rdata_parser_t *parser, rdata_read_handler read_handler);
rdata_error_t rdata_set_update_handler(rdata_parser_t *parser, rdata_update_handler update_handler);
rdata_error_t rdata_set_io_ctx(rdata_parser_t *parser, void *io_ctx);
/* rdata_parse works on RData and RDS. The table handler will be called once
 * per data frame in RData files, and zero times on RDS files. */
rdata_error_t rdata_parse(rdata_parser_t *parser, const char *filename, void *user_ctx);

