#include "ast.h"
#include <string.h>
#include <ctype.h>

static int is_c_keyword(const char *s, size_t len) {
    const char *keywords[] = {
        "if", "else", "while", "for", "return", "switch", "case", "break", "continue",
        "struct", "union", "enum", "typedef", "sizeof", "static", "const", "extern",
        "volatile", "goto", "register", "inline", "restrict", "default"
    };
    size_t count = sizeof(keywords) / sizeof(keywords[0]);
    for (size_t i = 0; i < count; i++) {
        if (strlen(keywords[i]) == len && strncmp(keywords[i], s, len) == 0) {
            return 1;
        }
    }
    return 0;
}

static int is_c_type(const char *s, size_t len) {
    const char *types[] = {
        "int", "char", "void", "float", "double", "short", "long", "signed", "unsigned",
        "size_t", "ssize_t", "int8_t", "int16_t", "int32_t", "int64_t",
        "uint8_t", "uint16_t", "uint32_t", "uint64_t", "bool", "FILE"
    };
    size_t count = sizeof(types) / sizeof(types[0]);
    for (size_t i = 0; i < count; i++) {
        if (strlen(types[i]) == len && strncmp(types[i], s, len) == 0) {
            return 1;
        }
    }
    return 0;
}

ast_node_t* parse_c_to_ast(const char *input, size_t input_len) {
    ast_node_t *head = NULL, *tail = NULL;
    size_t i = 0;

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

        if (input[i] == '/' && i + 1 < input_len && input[i + 1] == '/') {
            size_t comment_start = i;
            while (i < input_len && input[i] != '\n' && input[i] != '\r') {
                i++;
            }
            ast_node_t *node = ast_create_node(AST_NODE_COMMENT, input + comment_start, i - comment_start);
            if (!head) head = node; else tail->next = node;
            tail = node;
            continue;
        }

        if (input[i] == '/' && i + 1 < input_len && input[i + 1] == '*') {
            size_t comment_start = i;
            i += 2;
            while (i < input_len && !(input[i - 1] == '*' && input[i] == '/')) {
                i++;
            }
            if (i < input_len) i++;
            ast_node_t *node = ast_create_node(AST_NODE_COMMENT, input + comment_start, i - comment_start);
            if (!head) head = node; else tail->next = node;
            tail = node;
            continue;
        }

        if (input[i] == '#') {
            size_t prep_start = i;
            while (i < input_len && input[i] != '\n' && input[i] != '\r') {
                i++;
            }
            ast_node_t *node = ast_create_node(AST_NODE_PREPROCESSOR, input + prep_start, i - prep_start);
            if (!head) head = node; else tail->next = node;
            tail = node;
            continue;
        }

        if (input[i] == '"' || input[i] == '\'') {
            char quote = input[i];
            size_t str_start = i;
            i++;
            while (i < input_len && input[i] != quote) {
                if (input[i] == '\\' && i + 1 < input_len) {
                    i += 2;
                } else {
                    i++;
                }
            }
            if (i < input_len) i++;
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
            if (is_c_keyword(input + id_start, id_len)) {
                type = AST_NODE_KEYWORD;
            } else if (is_c_type(input + id_start, id_len)) {
                type = AST_NODE_TYPE;
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
