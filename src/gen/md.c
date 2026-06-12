#include "ast.h"
#include <string.h>
#include <ctype.h>

static void highlight_md(const char *input, size_t input_len, syntax_state_t *state, char **dest, size_t *di, size_t *cap) {
    (void)state;
    size_t start = 0;
    for (size_t si = 0; si <= input_len; si++) {
        if (si == input_len || input[si] == '\n' || input[si] == '\r') {
            size_t len = si - start;
            if (len > 0) {
                const char *line = input + start;
                size_t first_non_ws = 0;
                while (first_non_ws < len && isspace((unsigned char)line[first_non_ws])) {
                    first_non_ws++;
                }

                // 1. Check for headers: starts with '#' (after optional whitespace)
                if (first_non_ws < len && line[first_non_ws] == '#') {
                    size_t hashes = 0;
                    while (first_non_ws + hashes < len && line[first_non_ws + hashes] == '#') {
                        hashes++;
                    }
                    if (first_non_ws + hashes < len && line[first_non_ws + hashes] == ' ') {
                        // It is a header! Bold and bright blue
                        ast_append_str(dest, di, cap, "\033[1;38;2;121;192;255m");
                        for (size_t i = 0; i < len; i++) {
                            ast_append_char(dest, di, cap, line[i]);
                        }
                        ast_append_str(dest, di, cap, "\033[0m");
                        goto line_done;
                    }
                }

                // 2. Check for Horizontal Rule: `---`, `***`, `___`
                if (len - first_non_ws >= 3 && 
                    ((line[first_non_ws] == '-' && line[first_non_ws+1] == '-' && line[first_non_ws+2] == '-') ||
                     (line[first_non_ws] == '*' && line[first_non_ws+1] == '*' && line[first_non_ws+2] == '*') ||
                     (line[first_non_ws] == '_' && line[first_non_ws+1] == '_' && line[first_non_ws+2] == '_'))) {
                    // Gray out horizontal rule
                    ast_append_str(dest, di, cap, "\033[38;2;139;148;158m");
                    for (size_t i = 0; i < len; i++) {
                        ast_append_char(dest, di, cap, line[i]);
                    }
                    ast_append_str(dest, di, cap, "\033[0m");
                    goto line_done;
                }

                // 3. Line-by-line parsing for inline elements (Bold, Italic, Code, Links, Lists)
                // Let's first check if there is a list marker
                size_t content_start = first_non_ws;
                int has_list_marker = 0;
                size_t list_marker_len = 0;

                if (first_non_ws < len) {
                    char c = line[first_non_ws];
                    if ((c == '-' || c == '*' || c == '+') && first_non_ws + 1 < len && line[first_non_ws + 1] == ' ') {
                        has_list_marker = 1;
                        list_marker_len = 2;
                    } else if (isdigit((unsigned char)c)) {
                        size_t idx = first_non_ws;
                        while (idx < len && isdigit((unsigned char)line[idx])) {
                            idx++;
                        }
                        if (idx + 1 < len && line[idx] == '.' && line[idx + 1] == ' ') {
                            has_list_marker = 1;
                            list_marker_len = (idx + 2) - first_non_ws;
                        }
                    }
                }

                // Append leading whitespace
                for (size_t i = 0; i < first_non_ws; i++) {
                    ast_append_char(dest, di, cap, line[i]);
                }

                if (has_list_marker) {
                    // Highlight list marker in bright yellow/orange
                    ast_append_str(dest, di, cap, "\033[1;38;2;255;165;0m");
                    for (size_t i = 0; i < list_marker_len; i++) {
                        ast_append_char(dest, di, cap, line[first_non_ws + i]);
                    }
                    ast_append_str(dest, di, cap, "\033[0m");
                    content_start += list_marker_len;
                }

                // Scan content for inline elements
                size_t j = content_start;
                int in_backticks = 0;
                while (j < len) {
                    // Handle inline backticks
                    if (line[j] == '`') {
                        if (in_backticks) {
                            ast_append_char(dest, di, cap, '`');
                            ast_append_str(dest, di, cap, "\033[0m");
                            in_backticks = 0;
                        } else {
                            ast_append_str(dest, di, cap, "\033[38;2;240;128;128m"); // light red / coral for inline code
                            ast_append_char(dest, di, cap, '`');
                            in_backticks = 1;
                        }
                        j++;
                        continue;
                    }

                    if (in_backticks) {
                        ast_append_char(dest, di, cap, line[j++]);
                        continue;
                    }

                    // Handle Links: [text](url)
                    if (line[j] == '[' && j + 1 < len) {
                        size_t text_end = j + 1;
                        while (text_end < len && line[text_end] != ']') {
                            if (line[text_end] == '\\' && text_end + 1 < len) text_end += 2;
                            else text_end++;
                        }
                        if (text_end + 2 < len && line[text_end] == ']' && line[text_end + 1] == '(') {
                            size_t url_end = text_end + 2;
                            while (url_end < len && line[url_end] != ')') {
                                if (line[url_end] == '\\' && url_end + 1 < len) url_end += 2;
                                else url_end++;
                            }
                            if (url_end < len && line[url_end] == ')') {
                                // Highlight [text] in green, and (url) in gray
                                ast_append_str(dest, di, cap, "\033[38;2;126;231;135m");
                                ast_append_char(dest, di, cap, '[');
                                for (size_t k = j + 1; k < text_end; k++) {
                                    ast_append_char(dest, di, cap, line[k]);
                                }
                                ast_append_char(dest, di, cap, ']');
                                ast_append_str(dest, di, cap, "\033[38;2;139;148;158m");
                                ast_append_char(dest, di, cap, '(');
                                for (size_t k = text_end + 2; k < url_end; k++) {
                                    ast_append_char(dest, di, cap, line[k]);
                                }
                                ast_append_char(dest, di, cap, ')');
                                ast_append_str(dest, di, cap, "\033[0m");

                                j = url_end + 1;
                                continue;
                            }
                        }
                    }

                    // Normal character
                    ast_append_char(dest, di, cap, line[j++]);
                }
                if (in_backticks) {
                    ast_append_str(dest, di, cap, "\033[0m");
                }
            }

        line_done:
            if (si < input_len) {
                size_t nl_len = 1;
                if (input[si] == '\r' && si + 1 < input_len && input[si + 1] == '\n') {
                    nl_len = 2;
                }
                for (size_t i = 0; i < nl_len; i++) {
                    ast_append_char(dest, di, cap, input[si + i]);
                }
                si += nl_len - 1;
            }
            start = si + 1;
        }
    }
}

static const syntax_def_t md_syntax_def = {
    .extension = ".md",
    .keywords = NULL,
    .num_keywords = 0,
    .types = NULL,
    .num_types = 0,
    .line_comment = NULL,
    .block_comment_start = NULL,
    .block_comment_end = NULL,
    .format_fn = NULL,
    .highlight_fn = highlight_md
};

const syntax_def_t* get_md_syntax_def(void) {
    return &md_syntax_def;
}
