#include "ast.h"

static const char *sql_keywords[] = {
    "select", "insert", "update", "delete", "from", "where", "join", "left",
    "right", "inner", "outer", "on", "group", "by", "order", "having", "limit",
    "offset", "create", "table", "database", "index", "drop", "alter", "add",
    "constraint", "primary", "key", "foreign", "references", "values", "into",
    "set", "null", "not", "and", "or", "in", "exists", "like", "between", "as",
    "union", "all", "any", "some", "case", "when", "then", "else", "end"
};

static const char *sql_types[] = {
    "int", "integer", "varchar", "char", "text", "boolean", "date", "time",
    "timestamp", "numeric", "decimal", "float", "double", "blob"
};

static const syntax_def_t sql_syntax_def = {
    .extension = ".sql",
    .keywords = sql_keywords,
    .num_keywords = sizeof(sql_keywords) / sizeof(sql_keywords[0]),
    .types = sql_types,
    .num_types = sizeof(sql_types) / sizeof(sql_types[0]),
    .line_comment = "--",
    .block_comment_start = "/*",
    .block_comment_end = "*/",
    .format_fn = NULL,
    .highlight_fn = NULL
};

const syntax_def_t* get_sql_syntax_def(void) {
    return &sql_syntax_def;
}
