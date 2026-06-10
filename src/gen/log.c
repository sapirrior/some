#include "ast.h"
#include <string.h>
#include <stdlib.h>

ast_node_t* parse_log_to_ast(const char *input, size_t input_len) {
    ast_node_t *head = NULL, *tail = NULL;
    size_t start = 0;

    for (size_t si = 0; si <= input_len; si++) {
        if (si == input_len || input[si] == '\n' || input[si] == '\r') {
            size_t len = si - start;
            if (len > 0) {
                char *line_copy = malloc(len + 1);
                memcpy(line_copy, input + start, len);
                line_copy[len] = '\0';

                ast_node_type_t type = AST_NODE_TEXT;
                if (strstr(line_copy, "ERROR") || strstr(line_copy, "CRITICAL")) {
                    type = AST_NODE_LOG_ERROR;
                } else if (strstr(line_copy, "WARN") || strstr(line_copy, "WARNING")) {
                    type = AST_NODE_LOG_WARN;
                } else if (strstr(line_copy, "INFO")) {
                    type = AST_NODE_LOG_INFO;
                } else if (strstr(line_copy, "DEBUG")) {
                    type = AST_NODE_LOG_DEBUG;
                }
                free(line_copy);

                ast_node_t *node = ast_create_node(type, input + start, len);
                if (!head) {
                    head = node;
                } else {
                    tail->next = node;
                }
                tail = node;
            }
            if (si < input_len) {
                size_t nl_len = 1;
                if (input[si] == '\r' && si + 1 < input_len && input[si + 1] == '\n') {
                    nl_len = 2;
                }
                ast_node_t *node = ast_create_node(AST_NODE_TEXT, input + si, nl_len);
                if (!head) {
                    head = node;
                } else {
                    tail->next = node;
                }
                tail = node;
                si += nl_len - 1;
            }
            start = si + 1;
        }
    }
    return head;
}
