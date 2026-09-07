# HELP — Simple Line Editor

Run the editor, then type commands at the `> ` prompt. Line numbers are
1-indexed (the first line is line 1).

## Commands

### `i <line#> <text>` — Insert
Insert `<text>` as a new line at position `<line#>`, shifting existing lines
at and after that position down by one. Use `<line#>` equal to (current line
count + 1) to append at the end.

```
> i 1 Hello world
> i 2 This is line two
> i 1 A new first line
```
Result: `A new first line`, `Hello world`, `This is line two`.

### `d <line#>` — Delete
Remove the line at `<line#>`, shifting later lines up by one.

```
> d 2
```

### `p` — Print / Display
Print every line currently in the document with its line number.

```
> p
   1 | A new first line
   2 | This is line two
```

### `s <filename>` — Save
Write the current in-memory document to `<filename>`, one line per line of
the file. Overwrites the file if it already exists.

```
> s notes.txt
Saved 2 line(s) to 'notes.txt'.
```

### `l <filename>` — Load
Read `<filename>` from disk, replacing the current in-memory document with
its contents (one document line per file line). If the file can't be
opened, the current document is left unchanged.

```
> l notes.txt
Loaded 2 line(s) from 'notes.txt'.
```

### `f <word>` — Search (bonus)
Print the line number and text of every line containing `<word>` (substring
match).

```
> f line
Found 'line' on line(s):
  1 | A new first line
  2 | This is line two
```

### `c` — Count (bonus)
Print the total number of lines and the total number of words in the
document.

```
> c
Lines: 2 | Words: 8
```

### `h` — Help
Print this command summary from within the running program.

### `q` — Quit
Exit the editor.

## Error handling
- Inserting/deleting with an out-of-range line number prints an error and
  leaves the document unchanged — it does not crash the program.
- Deleting from an empty document prints an error instead of crashing.
- Loading a file that doesn't exist prints an error and keeps whatever
  document you already had in memory.
