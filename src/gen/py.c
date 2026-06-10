#include "ast.h"
#include <string.h>
#include <ctype.h>

static int is_py_keyword(const char *s, size_t len) {
    const char *keywords[] = {
        "def", "class", "import", "from", "if", "elif", "else", "for", "while",
        "return", "print", "as", "in", "try", "except", "with", "self", "True",
        "False", "None", "pass", "and", "or", "not", "lambda", "global", "nonlocal",
        "assert", "break", "continue", "del", "finally", "is", "raise", "yield"
    };
    size_t count = sizeof(keywords) / sizeof(keywords[0]);
    for (size_t i = 0; i < count; i++) {
        if (strlen(keywords[i]) == len && strncmp(keywords[i], s, len) == 0) {
            return 1;
        }
    }
    return 0;
}

ast_node_t* parse_py_to_ast(const char *input, size_t input_len) {
    ast_node_t *head = NULL, *tail = NULL;
    size_t i = 0;
    int last_was_def_or_class = 0;

    while (i < input_len) {
        if (isspace((unsigned char)input[i])) {
            size_t ws_start = i;
            while (i < input_len && isspace((unsigned char)input[i])) {
                i++;
            }
            ast_node_t *node = ast_create_node(AST_NODE_TEXT, input + ws_start, i - ws_start);
            if (!head) head = node; else tail->next = node;
            tail = node;
            continue;
        }

        if (input[i] == '#') {
            size_t comment_start = i;
            while (i < input_len && input[i] != '\n' && input[i] != '\r') {
                i++;
            }
            ast_node_t *node = ast_create_node(AST_NODE_COMMENT, input + comment_start, i - comment_start);
            if (!head) head = node; else tail->next = node;
            tail = node;
            continue;
        }

        if (i + 2 < input_len && ((input[i] == '"' && input[i+1] == '"' && input[i+2] == '"') ||
                                 (input[i] == '\'' && input[i+1] == '\'' && input[i+2] == '\''))) {
            char quote = input[i];
            size_t str_start = i;
            i += 3;
            while (i + 2 < input_len && !(input[i] == quote && input[i+1] == quote && input[i+2] == quote)) {
                if (input[i] == '\\') {
                    i += 2;
                } else {
                    i++;
                }
            }
            if (i + 2 < input_len) i += 3;
            ast_node_t *node = ast_create_node(AST_NODE_STRING, input + str_start, i - str_start);
            if (!head) head = node; else tail->next = node;
            tail = node;
            continue;
        }

        if (input[i] == '"' || input[i] == '\'') {
            char quote = input[i];
            size_t str_start = i;
            i++;
            while (i < input_len && input[i] != quote && input[i] != '\n' && input[i] != '\r') {
                if (input[i] == '\\' && i + 1 < input_len) {
                    i += 2;
                } else {
                    i++;
                }
            }
            if (i < input_len && input[i] == quote) i++;
            ast_node_t *node = ast_create_node(AST_NODE_STRING, input + str_start, i - str_start);
            if (!head) head = node; else tail->next = node;
            tail = node;
            continue;
        }

        if (isdigit((unsigned char)input[i])) {
            size_t num_start = i;
            while (i < input_len && (isalnum((unsigned char)input[i]) || input[i] == '.')) {
                i++;
            }
            ast_node_t *node = ast_create_node(AST_NODE_NUMBER, input + num_start, i - num_start);
            if (!head) head = node; else tail->next = node;
            tail = node;
            continue;
        }

        if (isalpha((unsigned char)input[i]) || input[i] == '_') {
            size_t id_start = i;
            while (i < input_len && (isalnum((unsigned char)input[i]) || input[i] == '_')) {
                i++;
            }
            size_t id_len = i - id_start;
            ast_node_type_t type = AST_NODE_IDENTIFIER;
            if (is_py_keyword(input + id_start, id_len)) {
                type = AST_NODE_KEYWORD;
                if (strncmp(input + id_start, "def", id_len) == 0 || strncmp(input + id_start, "class", id_len) == 0) {
                    last_was_def_or_class = 1;
                } else {
                    last_was_def_or_class = 0;
                }
            } else {
                if (last_was_def_or_class) {
                    type = AST_NODE_FUNCTION;
                    last_was_def_or_class = 0;
                } else {
                    size_t next_idx = i;
                    while (next_idx < input_len && isspace((unsigned char)input[next_idx])) {
                        next_idx++;
                    }
                    if (next_idx < input_len && input[next_idx] == '(') {
                        type = AST_NODE_FUNCTION;
                    }
                }
            }
            ast_node_t *node = ast_create_node(type, input + id_start, id_len);
            if (!head) head = node; else tail->next = node;
            tail = node;
            continue;
        }

        if (strchr("+-*/%=&|^!~<>(){}[];:.,?", input[i])) {
            ast_node_t *node = ast_create_node(AST_NODE_OPERATOR, input + i, 1);
            if (!head) head = node; else tail->next = node;
            tail = node;
            i++;
            continue;
        }

        ast_node_t *node = ast_create_node(AST_NODE_TEXT, input + i, 1);
        if (!head) head = node; else tail->next = node;
        tail = node;
        i++;
    }

    return head;
}
