# LLD Machine Coding – In-Memory File System (C++)

## Problem Statement

Design and implement an **in-memory File System** supporting directories, files, path navigation, permissions, and search, using Object-Oriented Programming in C++.

This problem is asked at Amazon, Google, Dropbox, and Rippling interviews. It tests the **Composite pattern** (files vs directories), **tree traversal**, **path parsing/normalization**, and recursive operations like size computation and search.

---

## Functional Requirements

1. Path-based operations (absolute paths, `/`-separated):
   - `mkdir(path)` — create a directory; create **intermediate directories** automatically (like `mkdir -p`).
   - `createFile(path, content)` — create a file with content; parent directory must exist.
   - `readFile(path)` / `writeFile(path, content)` (overwrite) / `appendFile(path, content)`.
   - `ls(path)` — if a directory: list children sorted by name; if a file: return the file's name.
   - `rm(path)` — delete a file, or a directory **recursively**.
   - `mv(srcPath, dstPath)` — move/rename a file or directory.
2. Metadata:
   - `size(path)` — file: content length; directory: **recursive** total of contained files.
   - Creation and last-modified timestamps (injected clock).
3. Search:
   - `find(path, namePattern)` — recursively find entries under `path` whose name matches a pattern with `*` wildcard (e.g., `*.txt`).
4. Path handling must support normalization: `.` , `..`, repeated slashes (e.g., `/a/./b//../c` → `/a/c`).

---

## Business Rules

- Creating a file/directory where a node of the **other type** already exists must fail with a clear error.
- `mkdir` on an existing directory is a no-op; `createFile` on an existing file fails (use `writeFile` to overwrite).
- `mv` must reject moving a directory **into its own subtree** (e.g., `mv /a /a/b/c`).
- `rm("/")` must be rejected.
- Writing/appending updates the file's last-modified time and all **ancestor directories'** last-modified times (document if you choose otherwise).
- Names within a directory are unique; names cannot contain `/` or be empty.

---

## Non-Functional Requirements

- In-memory only.
- Standard C++ library only (`map`, `unordered_map`, `memory`, `sstream`, etc.).
- Follow OOP principles; file and directory must share a common node abstraction (Composite).
- Path resolution should be O(depth), not a scan of all nodes.
- No UI; demonstrate via `main()`.

---

## Design Expectations (What Interviewers Look For)

### 1. Class Modelling

Expected core classes:
- `FSNode` (abstract: name, parent, timestamps, `isDirectory()`, `size()`)
- `File` (content) and `Directory` (children map: name → node) — the **Composite pattern**
- `Path` (parse, normalize, split into components)
- `FileSystem` (public API; resolves paths to nodes, enforces rules)
- `Clock` abstraction for timestamps

---

### 2. Composite in Action
- `size()` is polymorphic: file returns content length; directory recurses over children.
- `find` and `rm -r` are single recursive walks over the same structure — no type-switching in calling code beyond the polymorphic interface.

---

### 3. Path Resolution
- One private helper resolves a path to a node (or its parent + final name for create/move) and is reused by every API.
- Normalization handles `.`/`..` correctly, including `..` at root staying at root.

---

### 4. Edge Cases Interviewers Probe
- `mv` into own subtree; `mv` overwriting an existing destination (define: reject or replace).
- Deep recursion on `rm`/`size` — discuss stack depth.
- Sorted `ls` output (use `std::map` for children or sort on read).

---

## Constraints

- Single-threaded; no locking required.
- No permissions/users required in core (bonus below).
- No symlinks or hard links in core (design discussion is a bonus).
- Content is plain `std::string`; no block storage simulation.

---

## Example Scenario

```text
mkdir("/a/b/c")                      → creates /a, /a/b, /a/b/c
createFile("/a/b/notes.txt", "hi")
appendFile("/a/b/notes.txt", " there")
readFile("/a/b/notes.txt")           → "hi there"
ls("/a/b")                           → [c, notes.txt]
size("/a")                           → 8   (only notes.txt has content)

createFile("/a/b/c", "x")            → ERROR (directory exists at path)
mv("/a/b", "/a/b2")                  → rename
mv("/a", "/a/b2/inside")             → ERROR (own subtree)
find("/", "*.txt")                   → [/a/b2/notes.txt]
rm("/a/b2")                          → recursive delete
ls("/a")                             → []
```

---

## Bonus Challenges (If Time Permits)

- Add **permissions** (owner + rwx bits, checked on every operation) and a current-user context.
- Add **symbolic links** with cycle-safe resolution (max hop count).
- Add **quotas** (max bytes per directory subtree, enforced on write).
- Maintain **cached recursive sizes** updated on write, and discuss the invalidation trade-off.
- Add `cp -r` with copy-on-write sharing of file content.
