#include "ast.h"

static const char *cs_keywords[] = {
    "using", "namespace", "class", "struct", "interface", "enum", "delegate", "event",
    "public", "private", "protected", "internal", "static", "readonly", "volatile",
    "virtual", "override", "abstract", "sealed", "partial", "async", "await", "var",
    "get", "set", "value", "return", "if", "else", "switch", "case", "default",
    "break", "continue", "do", "for", "foreach", "in", "while", "try", "catch",
    "finally", "throw", "new", "this", "super", "null", "true", "false", "typeof"
};

static const char *cs_types[] = {
    "int", "string", "bool", "double", "float", "decimal", "char", "object", "void",
    "byte", "sbyte", "short", "ushort", "uint", "long", "ulong", "Int32", "Int64"
};

static const syntax_def_t cs_syntax_def = {
    .extension = ".cs",
    .keywords = cs_keywords,
    .num_keywords = sizeof(cs_keywords) / sizeof(cs_keywords[0]),
    .types = cs_types,
    .num_types = sizeof(cs_types) / sizeof(cs_types[0]),
    .line_comment = "//",
    .block_comment_start = "/*",
    .block_comment_end = "*/",
    .format_fn = NULL,
    .highlight_fn = NULL
};

const syntax_def_t* get_cs_syntax_def(void) {
    return &cs_syntax_def;
}
