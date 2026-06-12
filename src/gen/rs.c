#include "ast.h"

static const char *rs_keywords[] = {
    "fn", "let", "mut", "match", "impl", "struct", "enum", "use", "mod", "pub",
    "return", "if", "else", "loop", "while", "for", "in", "async", "await", "trait",
    "type", "crate", "super", "self", "Self", "const", "static", "where", "dyn",
    "unsafe", "ref", "move", "true", "false", "as", "break", "continue", "extern",
    "yield"
};

static const char *rs_types[] = {
    "u8", "u16", "u32", "u64", "u128", "usize",
    "i8", "i16", "i32", "i64", "i128", "isize",
    "f32", "f64", "bool", "char", "str", "String",
    "Option", "Result", "Vec"
};

static const syntax_def_t rs_syntax_def = {
    .extension = ".rs",
    .keywords = rs_keywords,
    .num_keywords = sizeof(rs_keywords) / sizeof(rs_keywords[0]),
    .types = rs_types,
    .num_types = sizeof(rs_types) / sizeof(rs_types[0]),
    .line_comment = "//",
    .block_comment_start = "/*",
    .block_comment_end = "*/",
    .format_fn = NULL,
    .highlight_fn = NULL
};

const syntax_def_t* get_rs_syntax_def(void) {
    return &rs_syntax_def;
}
