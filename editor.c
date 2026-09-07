/*
 * editor.c — A simple in-memory line editor for the terminal.
 *
 * Data structure: dynamic array of char* (see DESIGN.md for the
 * justification vs. a linked list).
 *
 * Core features: insert, delete, display, save/load.
 * Bonus features: search, line/word count.
 *
 * Build:   gcc -Wall -Wextra -std=c11 -o editor editor.c
 * Run:     ./editor
 * Help:    type `h` at the prompt, or see HELP.md
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_CAPACITY 8
#define MAX_INPUT_LEN 1024

/* strdup is POSIX, not standard C, so provide our own for portability
   under strict -std=c11 compilation. */
static char *dup_string(const char *s) {
    size_t len = strlen(s) + 1;
    char *copy = malloc(len);
    if (copy) memcpy(copy, s, len);
    return copy;
}

typedef struct {
    char **lines;
    int count;
    int capacity;
} Editor;

/* ---------- lifecycle ---------- */

void init_editor(Editor *ed) {
    ed->count = 0;
    ed->capacity = INITIAL_CAPACITY;
    ed->lines = malloc(sizeof(char *) * (size_t)ed->capacity);
    if (!ed->lines) {
        fprintf(stderr, "Fatal: out of memory initializing editor.\n");
        exit(1);
    }
}

void free_editor(Editor *ed) {
    for (int i = 0; i < ed->count; i++) {
        free(ed->lines[i]);
    }
    free(ed->lines);
    ed->lines = NULL;
    ed->count = 0;
    ed->capacity = 0;
}

static void ensure_capacity(Editor *ed) {
    if (ed->count < ed->capacity) return;
    int new_capacity = ed->capacity * 2;
    char **grown = realloc(ed->lines, sizeof(char *) * (size_t)new_capacity);
    if (!grown) {
        fprintf(stderr, "Fatal: out of memory growing document.\n");
        exit(1);
    }
    ed->lines = grown;
    ed->capacity = new_capacity;
}

/* ---------- core: insert / delete / display ---------- */

/* line numbers are 1-indexed; inserting at count+1 appends at the end */
int insert_line(Editor *ed, int lineno, const char *text) {
    if (lineno < 1 || lineno > ed->count + 1) {
        printf("Error: line number %d is out of range (valid: 1-%d).\n",
               lineno, ed->count + 1);
        return -1;
    }

    ensure_capacity(ed);

    /* shift everything at/after lineno-1 down by one slot */
    for (int i = ed->count; i > lineno - 1; i--) {
        ed->lines[i] = ed->lines[i - 1];
    }

    char *copy = dup_string(text);
    if (!copy) {
        fprintf(stderr, "Fatal: out of memory copying line text.\n");
        exit(1);
    }
    ed->lines[lineno - 1] = copy;
    ed->count++;
    return 0;
}

int delete_line(Editor *ed, int lineno) {
    if (ed->count == 0) {
        printf("Error: document is empty, nothing to delete.\n");
        return -1;
    }
    if (lineno < 1 || lineno > ed->count) {
        printf("Error: line number %d is out of range (valid: 1-%d).\n",
               lineno, ed->count);
        return -1;
    }

    free(ed->lines[lineno - 1]);

    /* shift everything after lineno-1 up by one slot */
    for (int i = lineno - 1; i < ed->count - 1; i++) {
        ed->lines[i] = ed->lines[i + 1];
    }
    ed->count--;
    return 0;
}

void display(const Editor *ed) {
    if (ed->count == 0) {
        printf("(empty document)\n");
        return;
    }
    for (int i = 0; i < ed->count; i++) {
        printf("%4d | %s\n", i + 1, ed->lines[i]);
    }
}

/* ---------- core: save / load ---------- */

int save_file(const Editor *ed, const char *filename) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        printf("Error: could not open '%s' for writing.\n", filename);
        return -1;
    }
    for (int i = 0; i < ed->count; i++) {
        fprintf(fp, "%s\n", ed->lines[i]);
    }
    fclose(fp);
    printf("Saved %d line(s) to '%s'.\n", ed->count, filename);
    return 0;
}

int load_file(Editor *ed, const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        printf("Error: could not open '%s' for reading.\n", filename);
        return -1;
    }

    /* build a fresh document, then swap it in only on success */
    Editor loaded;
    init_editor(&loaded);

    char buf[MAX_INPUT_LEN];
    while (fgets(buf, sizeof(buf), fp)) {
        size_t len = strlen(buf);
        if (len > 0 && buf[len - 1] == '\n') buf[len - 1] = '\0';
        ensure_capacity(&loaded);
        char *copy = dup_string(buf);
        if (!copy) {
            fprintf(stderr, "Fatal: out of memory loading file.\n");
            fclose(fp);
            free_editor(&loaded);
            exit(1);
        }
        loaded.lines[loaded.count++] = copy;
    }
    fclose(fp);

    free_editor(ed);
    *ed = loaded;
    printf("Loaded %d line(s) from '%s'.\n", ed->count, filename);
    return 0;
}

/* ---------- bonus: search ---------- */

void search_word(const Editor *ed, const char *word) {
    int found = 0;
    for (int i = 0; i < ed->count; i++) {
        if (strstr(ed->lines[i], word) != NULL) {
            if (!found) printf("Found '%s' on line(s):\n", word);
            printf("  %d | %s\n", i + 1, ed->lines[i]);
            found = 1;
        }
    }
    if (!found) {
        printf("'%s' not found in document.\n", word);
    }
}

/* ---------- bonus: line/word count ---------- */

void print_stats(const Editor *ed) {
    long word_count = 0;
    for (int i = 0; i < ed->count; i++) {
        int in_word = 0;
        for (const char *p = ed->lines[i]; *p; p++) {
            if (*p == ' ' || *p == '\t') {
                in_word = 0;
            } else if (!in_word) {
                in_word = 1;
                word_count++;
            }
        }
    }
    printf("Lines: %d | Words: %ld\n", ed->count, word_count);
}

/* ---------- help ---------- */

void print_help(void) {
    printf(
        "Commands:\n"
        "  i <line#> <text>   Insert text at line#, shifting later lines down\n"
        "  d <line#>          Delete the line at line#\n"
        "  p                  Print (display) the whole document\n"
        "  s <filename>       Save document to filename\n"
        "  l <filename>       Load document from filename (replaces current)\n"
        "  f <word>           Search for a word/phrase, prints matching line#s\n"
        "  c                  Show line count and word count\n"
        "  h                  Show this help\n"
        "  q                  Quit\n"
    );
}

/* ---------- REPL ---------- */

static void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len > 0 && s[len - 1] == '\n') s[len - 1] = '\0';
}

void run_repl(Editor *ed) {
    char input[MAX_INPUT_LEN];

    printf("Simple Line Editor — type 'h' for help, 'q' to quit.\n");

    while (1) {
        printf("> ");
        fflush(stdout);

        if (!fgets(input, sizeof(input), stdin)) {
            printf("\n");
            break; /* EOF (e.g. Ctrl-D) */
        }
        trim_newline(input);

        if (strlen(input) == 0) continue;

        char cmd = input[0];
        char *rest = input + 1;
        while (*rest == ' ') rest++; /* skip space after command letter */

        switch (cmd) {
            case 'i': {
                int lineno;
                int consumed;
                if (sscanf(rest, "%d%n", &lineno, &consumed) != 1) {
                    printf("Usage: i <line#> <text>\n");
                    break;
                }
                char *text = rest + consumed;
                while (*text == ' ') text++;
                if (*text == '\0') {
                    printf("Usage: i <line#> <text>  (text cannot be empty)\n");
                    break;
                }
                insert_line(ed, lineno, text);
                break;
            }
            case 'd': {
                int lineno;
                if (sscanf(rest, "%d", &lineno) != 1) {
                    printf("Usage: d <line#>\n");
                    break;
                }
                delete_line(ed, lineno);
                break;
            }
            case 'p':
                display(ed);
                break;
            case 's': {
                if (*rest == '\0') {
                    printf("Usage: s <filename>\n");
                    break;
                }
                save_file(ed, rest);
                break;
            }
            case 'l': {
                if (*rest == '\0') {
                    printf("Usage: l <filename>\n");
                    break;
                }
                load_file(ed, rest);
                break;
            }
            case 'f': {
                if (*rest == '\0') {
                    printf("Usage: f <word>\n");
                    break;
                }
                search_word(ed, rest);
                break;
            }
            case 'c':
                print_stats(ed);
                break;
            case 'h':
                print_help();
                break;
            case 'q':
                return;
            default:
                printf("Unknown command '%c'. Type 'h' for help.\n", cmd);
        }
    }
}

int main(void) {
    Editor ed;
    init_editor(&ed);

    run_repl(&ed);

    free_editor(&ed);
    printf("Goodbye.\n");
    return 0;
}
