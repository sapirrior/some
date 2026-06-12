#include "ast.h"

static const char *rb_keywords[] = {
    "def", "end", "class", "module", "require", "include", "extend", "if", "unless",
    "else", "elsif", "while", "until", "for", "in", "begin", "rescue", "ensure",
    "raise", "fail", "return", "yield", "self", "true", "false", "nil", "and", "or",
    "not", "attr_reader", "attr_writer", "attr_accessor", "puts"
};

static const syntax_def_t rb_syntax_def = {
    .extension = ".rb",
    .keywords = rb_keywords,
    .num_keywords = sizeof(rb_keywords) / sizeof(rb_keywords[0]),
    .types = NULL,
    .num_types = 0,
    .line_comment = "#",
    .block_comment_start = NULL,
    .block_comment_end = NULL,
    .format_fn = NULL,
    .highlight_fn = NULL
};

const syntax_def_t* get_rb_syntax_def(void) {
    return &rb_syntax_def;
}
