# Simple Line Editor in C

**Team:** Maltesh (solo) SRN - R25EJ066
**Course:** Portfolio Building — Studio Course, 3rd Semester
**Assignment:** Coding Competition — Build a Simple Line Editor in C

## Features implemented

Core (4 of the required 2–3, all implemented):
- Insert a line
- Delete a line
- Display the document
- Save / load a file

Bonus (extra credit):
- Search — find and report which line(s) contain a given word/phrase
- Line count / word count

Not implemented: find & replace, undo (left for future work — see
`DESIGN.md` for the function outline these would slot into).

## Data structure

Dynamic array of `char *` (grown with `realloc`). See `DESIGN.md` for the
full justification versus a linked list.

## Build & run

Requires `gcc` and a C11-compatible compiler. No external dependencies.

```bash
gcc -Wall -Wextra -std=c11 -o editor editor.c
./editor
```

Once running, type `h` at the `> ` prompt for the full command list, or see
`HELP.md`.

## Quick example session

```
> i 1 Hello world
> i 2 This is line two
> p
   1 | Hello world
   2 | This is line two
> s notes.txt
Saved 2 line(s) to 'notes.txt'.
> q
```

## Files in this repo

| File         | Purpose                                                        |
|--------------|-----------------------------------------------------------------|
| `editor.c`   | The full editor implementation                                  |
| `DESIGN.md`  | Paper design write-up: data structure choice, commands, functions |
| `HELP.md`    | Command reference with usage examples                            |
| `README.md`  | This file                                                        |

## Testing performed

Manually tested: insert at start/middle/end, delete from start/middle/end,
delete on an empty document, insert/delete with out-of-range line numbers,
save then load round-trip, search with matches and no matches, and word/line
count. None of these crash the program — invalid input prints an error and
returns to the prompt.
