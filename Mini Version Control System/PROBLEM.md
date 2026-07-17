# LLD Machine Coding – Mini Version Control System (C++)

## Problem Statement

Design and implement a **Mini Version Control System** (a simplified Git) supporting staging, commits, branches, checkout, diff, and merge with conflict detection — all in memory, using Object-Oriented Programming in C++.

This problem is asked at Google, Atlassian, GitHub, and Nutanix interviews. It is an advanced test of **immutable snapshot modelling**, **DAG traversal (commit history, lowest common ancestor)**, **content hashing**, and diff/merge algorithms.

---

## Functional Requirements

1. Working directory + staging:
   - `writeFile(path, content)` — modify the working directory.
   - `add(path)` — stage a file's current content.
   - `status()` — files that are modified/added/deleted relative to staging and last commit.
2. Commits:
   - `commit(message)` — snapshot the staged files as an immutable commit pointing to its parent(s).
   - `log()` — commit history from HEAD backwards (id, message, timestamp).
3. Branches:
   - `createBranch(name)` (at current HEAD), `checkout(branchName)` — switches branch and restores its snapshot into the working directory.
4. Diff:
   - `diff(commitA, commitB)` — files added / removed / modified between two commits (line-level diff is a bonus; file-level required).
5. Merge:
   - `merge(otherBranch)` — three-way merge using the **lowest common ancestor (LCA)**:
     - Changes on only one side apply cleanly.
     - Changes to the **same file on both sides** → report a **conflict** and abort the merge (leave state untouched).
     - A clean merge creates a **merge commit with two parents**.

---

## Business Rules

- Commits are **immutable**; content is stored once per unique blob (deduplicate by content hash) — a commit maps path → blob hash.
- `commit` with an empty staging area is rejected ("nothing to commit").
- `checkout` with **uncommitted staged changes** is rejected (document this simplification).
- File deletion is tracked: deleting a file and staging that deletion removes it from the next snapshot.
- Merging a branch that is already an **ancestor** of HEAD is a no-op ("already up to date"); if HEAD is an ancestor of the other branch, perform a **fast-forward** (move the pointer, no merge commit).
- Branch names are unique; checkout of a nonexistent branch fails clearly.

---

## Non-Functional Requirements

- In-memory only (no real files on disk).
- Standard C++ library only (`unordered_map`, `set`, `functional`/custom hash, etc.).
- Follow OOP principles; commit graph logic separated from working-directory logic.
- No UI; demonstrate via `main()` printing status/log/diff/merge results.

---

## Design Expectations (What Interviewers Look For)

### 1. Class Modelling

Expected core classes:
- `Blob` (content + content hash)
- `Commit` (id, message, timestamp, parent ids [1 or 2], snapshot: path → blob hash)
- `Branch` (name → commit id) and a `HEAD` reference
- `StagingArea` (path → blob hash or deletion marker)
- `WorkingDirectory` (path → content)
- `Repository` (public API: add/commit/branch/checkout/log/diff/merge)
- `DiffResult` / `MergeResult` (added, removed, modified, conflicts)

---

### 2. Snapshot & Hashing Discipline
- Snapshots are **full maps**, not deltas — diffs are computed, not stored.
- Identical content across commits shares one blob (hash-keyed store).

---

### 3. Graph Algorithms
- `log` walks parents (handle merge commits with two parents — BFS by timestamp or visited-set DFS).
- **LCA** of two commits for the merge base (BFS from both, first common ancestor).
- Ancestor checks power fast-forward and "already up to date" detection.

---

### 4. Merge Correctness (What Gets Probed)
- Same file changed identically on both sides → NOT a conflict.
- File deleted on one side, modified on the other → conflict.
- Merge aborts atomically on conflict — no half-merged state.

---

## Constraints

- File-level merge granularity is sufficient (line-level three-way merge is a bonus).
- No remotes, push/pull, rebase, or tags.
- No permissions or multiple users; single repository.
- Single-threaded.

---

## Example Scenario

```text
writeFile(a.txt, "v1"); add(a.txt); commit("c1")        → main: c1
createBranch(feature); checkout(feature)
writeFile(a.txt, "v2"); add(a.txt); commit("c2")        → feature: c1→c2
checkout(main)
writeFile(b.txt, "hello"); add(b.txt); commit("c3")     → main: c1→c3

merge(feature):
  LCA(c3, c2) = c1
  a.txt: changed only on feature → take v2
  b.txt: changed only on main    → keep
  → merge commit c4 (parents c3, c2) | a.txt=v2, b.txt=hello

checkout(feature); writeFile(b.txt, "bye"); add; commit("c5")
checkout(main);   writeFile(b.txt, "hey"); add; commit("c6")
merge(feature) → CONFLICT on b.txt → merge aborted, main untouched

diff(c1, c4) → added: b.txt | modified: a.txt
```

---

## Bonus Challenges (If Time Permits)

- **Line-level diff** (LCS-based) and line-level three-way merge with conflict markers.
- `revert(commitId)` — a new commit that undoes a prior commit's changes.
- **Tags** and `checkout(commitId)` (detached HEAD).
- Garbage collection of unreachable commits/blobs after branch deletion.
- Persist the object store to disk in a content-addressed layout and reload it.
