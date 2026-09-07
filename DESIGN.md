# Paper Design — Simple Line Editor in C

(Write this out by hand for the "Paper design" deliverable — this file is your
reference/typed-up copy of what to put on paper. Photograph the handwritten
version and submit it as-is.)

## 1. Data structure choice

**Chosen: dynamic array of `char *` (an array of string pointers), grown with
`realloc`.**

```c
typedef struct {
    char **lines;     // array of heap-allocated line strings
    int    count;      // number of lines currently stored
    int    capacity;   // allocated slots in `lines`
} Editor;
```

### Trade-off justification (say this out loud to the grader)

- **Array of strings (chosen)**
  - Display is O(n) with great cache locality — just walk the array.
  - Indexing by line number is O(1) (`lines[i]`), which matters because every
    command (`insert`, `delete`, `search`, `replace`) is addressed *by line
    number*, not by position/cursor.
  - Insert/delete in the middle is O(n) because we have to shift pointers —
    but for a "small text document" (this problem's stated scope) that's
    fine; n is small, so the shift is cheap in practice.
  - Save/load maps directly onto array iteration, no extra bookkeeping.

- **Linked list of lines (rejected)**
  - Insert/delete become O(1) *once you're at the right node*, but getting to
    line number `k` is still O(k) since there's no random access — no better
    than the array's O(n) shift, and it adds pointer-chasing overhead and
    more complex memory management (extra malloc per node, more failure
    points).
  - Only wins decisively for very large documents with insert-heavy,
    cursor-local workloads — not this problem.

- **Plain dynamic array of fixed-size char buffers (rejected)**
  - Wastes memory (every line padded to worst-case length) or truncates long
    lines. `char *` + `strdup` costs one extra allocation per line but has no
    such ceiling.

**Conclusion:** array of `char *` gives O(1) line lookup (used by every
command), simple save/load, and acceptable O(n) shifts for a small in-memory
document — the best fit for this problem's access pattern.

## 2. Command set

All commands typed at a `> ` prompt, one per line, first token is the command
letter:

| Command | Syntax                        | Effect                                             |
|---------|-------------------------------|-----------------------------------------------------|
| `i`     | `i <line#> <text...>`         | Insert `text` at `line#`, shifting rest down        |
| `d`     | `d <line#>`                   | Delete the line at `line#`, shifting rest up        |
| `p`     | `p`                            | Print (display) the whole document with line #s     |
| `s`     | `s <filename>`                 | Save the document to `<filename>`                    |
| `l`     | `l <filename>`                 | Load `<filename>`, replacing current document        |
| `f`     | `f <word>`                     | Search: print line #s containing `<word>`             |
| `c`     | `c`                            | Print line count and total word count                |
| `h`     | `h`                            | Print help text                                       |
| `q`     | `q`                            | Quit                                                   |

Line numbers are 1-indexed and shown that way in `p`, matching how a human
thinks about "line 3 of my file."

## 3. Function outline (write these signatures on paper before coding)

```c
void   init_editor(Editor *ed);
void   free_editor(Editor *ed);
void   ensure_capacity(Editor *ed);           // grow array via realloc when full

int    insert_line(Editor *ed, int lineno, const char *text); // returns 0/-1
int    delete_line(Editor *ed, int lineno);                    // returns 0/-1
void   display(const Editor *ed);

int    save_file(const Editor *ed, const char *filename);
int    load_file(Editor *ed, const char *filename);

void   search_word(const Editor *ed, const char *word);        // bonus
void   print_stats(const Editor *ed);                          // bonus

void   print_help(void);
void   run_repl(Editor *ed);                                    // main command loop
```

## 4. Edge cases to handle (call these out on paper)

- Insert at `count + 1` (i.e., append at the very end) must succeed.
- Insert/delete with `line# < 1` or `line# > count (+1 for insert)` → print
  an error, don't crash.
- `delete` on an empty document → error message, no crash.
- `display` on an empty document → print something like "(empty document)".
- `load` on a missing file → error message, keep the current in-memory
  document unchanged.
- Very long input lines → read with `fgets` into a reasonably sized buffer
  and don't overflow it.
