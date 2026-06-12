#include "ast.h"

static const char *css_keywords[] = {
    "@media", "@keyframes", "@import", "@font-face", "@charset", "@supports",
    "important", "none", "auto", "inherit", "initial", "unset"
};

static const char *css_types[] = {
    "margin", "padding", "width", "height", "color", "background", "border",
    "display", "position", "top", "bottom", "left", "right", "font-family",
    "font-size", "font-weight", "line-height", "text-align", "text-decoration",
    "box-shadow", "border-radius", "opacity", "overflow", "cursor", "transition",
    "transform", "animation", "flex", "grid", "justify-content", "align-items"
};

static const syntax_def_t css_syntax_def = {
    .extension = ".css",
    .keywords = css_keywords,
    .num_keywords = sizeof(css_keywords) / sizeof(css_keywords[0]),
    .types = css_types,
    .num_types = sizeof(css_types) / sizeof(css_types[0]),
    .line_comment = NULL,
    .block_comment_start = "/*",
    .block_comment_end = "*/",
    .format_fn = NULL,
    .highlight_fn = NULL
};

const syntax_def_t* get_css_syntax_def(void) {
    return &css_syntax_def;
}
