#include "ast.h"

static const char *php_keywords[] = {
    "function", "class", "interface", "trait", "public", "private", "protected",
    "static", "final", "abstract", "extends", "implements", "use", "namespace",
    "return", "if", "else", "elseif", "while", "do", "foreach", "as", "switch",
    "case", "default", "break", "continue", "try", "catch", "finally", "throw",
    "new", "this", "self", "parent", "echo", "print", "require", "include",
    "require_once", "include_once", "array", "null", "true", "false"
};

static const syntax_def_t php_syntax_def = {
    .extension = ".php",
    .keywords = php_keywords,
    .num_keywords = sizeof(php_keywords) / sizeof(php_keywords[0]),
    .types = NULL,
    .num_types = 0,
    .line_comment = "//",
    .block_comment_start = "/*",
    .block_comment_end = "*/",
    .format_fn = NULL,
    .highlight_fn = NULL
};

const syntax_def_t* get_php_syntax_def(void) {
    return &php_syntax_def;
}
