#include "ast.h"

static const char *go_keywords[] = {
    "func", "package", "import", "var", "const", "type", "struct", "interface",
    "map", "chan", "go", "select", "defer", "panic", "recover", "return", "if",
    "else", "for", "range", "switch", "case", "default", "fallthrough", "break",
    "continue", "goto", "nil", "true", "false", "iota"
};

static const char *go_types[] = {
    "int", "int8", "int16", "int32", "int64",
    "uint", "uint8", "uint16", "uint32", "uint64", "uintptr",
    "float32", "float64", "complex64", "complex128",
    "string", "bool", "byte", "rune", "error"
};

static const syntax_def_t go_syntax_def = {
    .extension = ".go",
    .keywords = go_keywords,
    .num_keywords = sizeof(go_keywords) / sizeof(go_keywords[0]),
    .types = go_types,
    .num_types = sizeof(go_types) / sizeof(go_types[0]),
    .line_comment = "//",
    .block_comment_start = "/*",
    .block_comment_end = "*/",
    .format_fn = NULL,
    .highlight_fn = NULL
};

const syntax_def_t* get_go_syntax_def(void) {
    return &go_syntax_def;
}
