#include "ast.h"

static const syntax_def_t yaml_syntax_def = {
    .extension = ".yaml",
    .keywords = NULL,
    .num_keywords = 0,
    .types = NULL,
    .num_types = 0,
    .line_comment = "#",
    .block_comment_start = NULL,
    .block_comment_end = NULL,
    .format_fn = NULL,
    .highlight_fn = NULL
};

const syntax_def_t* get_yaml_syntax_def(void) {
    return &yaml_syntax_def;
}
