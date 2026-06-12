#include "ast.h"

static const char *java_keywords[] = {
    "public", "private", "protected", "class", "interface", "extends", "implements",
    "import", "package", "static", "final", "void", "new", "this", "super", "return",
    "if", "else", "try", "catch", "finally", "throw", "throws", "instanceof",
    "synchronized", "volatile", "transient", "strictfp", "native", "assert",
    "break", "case", "continue", "default", "do", "for", "while", "switch",
    "const", "goto", "enum"
};

static const char *java_types[] = {
    "int", "double", "float", "boolean", "char", "byte", "short", "long",
    "String", "Object", "System", "Integer", "Double", "Float", "Boolean",
    "Character", "Byte", "Short", "Long", "List", "Map", "Set", "HashMap", "ArrayList"
};

static const syntax_def_t java_syntax_def = {
    .extension = ".java",
    .keywords = java_keywords,
    .num_keywords = sizeof(java_keywords) / sizeof(java_keywords[0]),
    .types = java_types,
    .num_types = sizeof(java_types) / sizeof(java_types[0]),
    .line_comment = "//",
    .block_comment_start = "/*",
    .block_comment_end = "*/",
    .format_fn = NULL,
    .highlight_fn = NULL
};

const syntax_def_t* get_java_syntax_def(void) {
    return &java_syntax_def;
}
