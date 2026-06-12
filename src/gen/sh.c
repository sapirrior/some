#include "ast.h"

static const char *sh_keywords[] = {
    "if", "then", "elif", "else", "fi", "for", "while", "until", "in", "do",
    "done", "case", "esac", "function", "select", "local", "return", "exit",
    "echo", "alias", "export", "declare", "readonly", "true", "false"
};

static const syntax_def_t sh_syntax_def = {
    .extension = ".sh",
    .keywords = sh_keywords,
    .num_keywords = sizeof(sh_keywords) / sizeof(sh_keywords[0]),
    .types = NULL,
    .num_types = 0,
    .line_comment = "#",
    .block_comment_start = NULL,
    .block_comment_end = NULL,
    .format_fn = NULL,
    .highlight_fn = NULL
};

const syntax_def_t* get_sh_syntax_def(void) {
    return &sh_syntax_def;
}
