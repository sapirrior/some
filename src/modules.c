#include "some.h"
#include "modules.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ─────────────────────────────────────────────────────────────────────────── *
 *  Safe Dynamic String Builder                                                *
 * ─────────────────────────────────────────────────────────────────────────── */
static void append_char(char **dest, size_t *di, size_t *cap, char c) {
    if (*di + 2 >= *cap) {
        *cap = *cap ? *cap * 2 : 1024;
        *dest = realloc(*dest, *cap);
    }
    (*dest)[(*di)++] = c;
}

static void append_str(char **dest, size_t *di, size_t *cap, const char *s) {
    size_t len = strlen(s);
    if (*di + len + 2 >= *cap) {
        while (*di + len + 2 >= *cap) {
            *cap = *cap ? *cap * 2 : 1024;
        }
        *dest = realloc(*dest, *cap);
    }
    memcpy(*dest + *di, s, len);
    *di += len;
}

/* ─────────────────────────────────────────────────────────────────────────── *
 *  JSON Parser / Reformatter                                                  *
 * ─────────────────────────────────────────────────────────────────────────── */
static char* pretty_print_json(const char *input, size_t input_len, size_t *out_len) {
    size_t cap = input_len * 2 + 1024;
    char *dest = malloc(cap);
    size_t di = 0;
    int indent = 0;
    int in_string = 0;

    for (size_t si = 0; si < input_len; si++) {
        char c = input[si];

        if (in_string) {
            append_char(&dest, &di, &cap, c);
            if (c == '"' && input[si - 1] != '\\') {
                in_string = 0;
            }
            continue;
        }

        if (c == '"') {
            in_string = 1;
            append_char(&dest, &di, &cap, c);
            continue;
        }

        if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
            continue;
        }

        if (c == '{' || c == '[') {
            append_char(&dest, &di, &cap, c);
            indent += 2;
            append_char(&dest, &di, &cap, '\n');
            for (int j = 0; j < indent; j++) append_char(&dest, &di, &cap, ' ');
            continue;
        }

        if (c == '}' || c == ']') {
            indent -= 2;
            if (indent < 0) indent = 0;
            append_char(&dest, &di, &cap, '\n');
            for (int j = 0; j < indent; j++) append_char(&dest, &di, &cap, ' ');
            append_char(&dest, &di, &cap, c);
            continue;
        }

        if (c == ',') {
            append_char(&dest, &di, &cap, c);
            append_char(&dest, &di, &cap, '\n');
            for (int j = 0; j < indent; j++) append_char(&dest, &di, &cap, ' ');
            continue;
        }

        if (c == ':') {
            append_char(&dest, &di, &cap, c);
            append_char(&dest, &di, &cap, ' ');
            continue;
        }

        append_char(&dest, &di, &cap, c);
    }
    dest[di] = '\0';
    *out_len = di;
    return dest;
}

/* ─────────────────────────────────────────────────────────────────────────── *
 *  CSV/TSV Column Aligner                                                     *
 * ─────────────────────────────────────────────────────────────────────────── */
static char* align_csv(const char *input, size_t input_len, size_t *out_len, char separator) {
    int rows_count = 0;
    for (size_t i = 0; i < input_len; i++) {
        if (input[i] == '\n') rows_count++;
    }
    rows_count += 2;

    char ***table = calloc(rows_count, sizeof(char**));
    int *row_cols = calloc(rows_count, sizeof(int));
    int max_cols = 0;

    size_t start = 0;
    int row_idx = 0;
    for (size_t i = 0; i <= input_len; i++) {
        if (i == input_len || input[i] == '\n' || input[i] == '\r') {
            size_t line_len = i - start;
            if (line_len > 0 || i == input_len) {
                char *line = malloc(line_len + 1);
                memcpy(line, input + start, line_len);
                line[line_len] = '\0';

                int fields_cap = 128;
                char **fields = calloc(fields_cap, sizeof(char*));
                int cols = 0;
                size_t si = 0;
                while (si < line_len) {
                    if (cols >= fields_cap) {
                        fields_cap *= 2;
                        fields = realloc(fields, fields_cap * sizeof(char*));
                        memset(fields + cols, 0, (fields_cap - cols) * sizeof(char*));
                    }
                    char *field = malloc(line_len + 1);
                    size_t fi = 0;
                    if (line[si] == '"') {
                        si++;
                        while (si < line_len) {
                            if (line[si] == '"' && si + 1 < line_len && line[si + 1] == '"') {
                                field[fi++] = '"';
                                si += 2;
                            } else if (line[si] == '"') {
                                si++;
                                break;
                            } else {
                                field[fi++] = line[si++];
                            }
                        }
                    } else {
                        while (si < line_len && line[si] != separator) {
                            field[fi++] = line[si++];
                        }
                    }
                    field[fi] = '\0';
                    fields[cols++] = field;
                    if (line[si] == separator) si++;
                }

                table[row_idx] = fields;
                row_cols[row_idx] = cols;
                if (cols > max_cols) max_cols = cols;
                row_idx++;

                free(line);
            }
            if (i < input_len && input[i] == '\r' && input[i + 1] == '\n') i++;
            start = i + 1;
        }
    }

    int *col_widths = NULL;
    if (max_cols > 0) {
        col_widths = calloc(max_cols, sizeof(int));
        for (int r = 0; r < row_idx; r++) {
            for (int c = 0; c < row_cols[r]; c++) {
                int len = strlen(table[r][c]);
                if (len > col_widths[c]) {
                    col_widths[c] = len;
                }
            }
        }
    }

    size_t out_cap = input_len + 1024;
    char *dest = malloc(out_cap);
    size_t di = 0;

    for (int r = 0; r < row_idx; r++) {
        for (int c = 0; c < row_cols[r]; c++) {
            const char *field = table[r][c];
            append_str(&dest, &di, &out_cap, field);

            if (c < row_cols[r] - 1 && col_widths) {
                int pad = col_widths[c] - strlen(field) + 2;
                for (int p = 0; p < pad; p++) {
                    append_char(&dest, &di, &out_cap, ' ');
                }
            }
        }
        append_char(&dest, &di, &out_cap, '\n');

        for (int c = 0; c < row_cols[r]; c++) {
            free(table[r][c]);
        }
        free(table[r]);
    }
    dest[di] = '\0';

    free(table);
    free(row_cols);
    if (col_widths) free(col_widths);

    *out_len = di;
    return dest;
}

/* ─────────────────────────────────────────────────────────────────────────── *
 *  Converter Main Dispatcher                                                  *
 * ─────────────────────────────────────────────────────────────────────────── */
char* module_convert(const char *filename, const char *input, size_t input_len, size_t *out_len) {
    if (!filename) return NULL;

    const char *ext = strrchr(filename, '.');
    if (ext) {
        if (strcmp(ext, ".json") == 0) {
            return pretty_print_json(input, input_len, out_len);
        } else if (strcmp(ext, ".csv") == 0) {
            return align_csv(input, input_len, out_len, ',');
        } else if (strcmp(ext, ".tsv") == 0) {
            return align_csv(input, input_len, out_len, '\t');
        }
    }

    // Fallback: sniff first non-whitespace character for JSON
    size_t si = 0;
    while (si < input_len && (input[si] == ' ' || input[si] == '\t' || input[si] == '\n' || input[si] == '\r')) {
        si++;
    }
    if (si < input_len && (input[si] == '{' || input[si] == '[')) {
        return pretty_print_json(input, input_len, out_len);
    }

    return NULL;
}
