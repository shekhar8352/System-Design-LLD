# LLD Machine Coding – Role-Based Access Control (RBAC) System (C++)

## Problem Statement

Design and implement a **Role-Based Access Control system** that answers "can user U perform action A on resource R?" supporting role hierarchies, group membership, resource-scoped grants, and explicit denies, using Object-Oriented Programming in C++.

This problem is asked at Atlassian, Okta, Amazon (IAM team), and Rippling interviews. It tests **graph traversal over hierarchies**, **permission resolution ordering**, **deny-overrides semantics**, and designing a clean authorization query API.

---

## Functional Requirements

1. Core entities:
   - **Permissions**: `(action, resourceType)` pairs, e.g., `(READ, DOCUMENT)`, `(DELETE, PROJECT)`.
   - **Roles**: named bundles of permissions; roles can **inherit** from other roles (`ADMIN` inherits `EDITOR` inherits `VIEWER`).
   - **Users** and **Groups**: users belong to groups; groups can be nested inside groups.
2. Assignment:
   - Assign roles to a user or a group, either **globally** or **scoped to a resource** (e.g., `EDITOR` on `project:42` only).
   - Resources form a hierarchy (`org → project → document`); a role granted on a parent applies to all descendants.
3. Authorization query:
   - `check(userId, action, resourceId)` → ALLOW / DENY, plus an **explanation trace** (which role, via which group, granted at which scope).
4. Explicit deny:
   - A deny rule (`user/group, permission, scope`) **overrides any allow** at the same or descendant scope.
5. Administration queries:
   - All permissions a user effectively has on a resource.
   - All users who can perform `(action, resource)` — the reverse query.
   - Role/group membership listings.

---

## Business Rules

- Resolution order: gather allows from (direct roles + group roles + inherited roles) across (resource + its ancestors); then apply denies — **deny wins** over any allow.
- Role inheritance and group nesting must both reject **cycles** at mutation time.
- Deleting a role/group cleanly removes all assignments referencing it.
- An unknown user, action, or resource yields DENY (default-deny, never an exception).
- Duplicate assignments are idempotent.
- The explanation trace must show the actual grant path, e.g., `u1 → group:eng → role:EDITOR(inherits VIEWER) → scope project:42`.

---

## Non-Functional Requirements

- In-memory only.
- Standard C++ library only.
- `check()` is the hot path — resolve without rebuilding the world each call (memoize per query or maintain reasonable indexes; document invalidation).
- Follow OOP principles; policy semantics (deny-overrides) should live in one policy-decision component.
- No UI/network; demonstrate via `main()`.

---

## Design Expectations (What Interviewers Look For)

### 1. Class Modelling

Expected core classes:
- `Permission` (action, resourceType — value type, hashable)
- `Role` (name, permissions, parent roles)
- `Group` (members: users/groups), `User`
- `Resource` (id, type, parent) forming a tree
- `Assignment` (subject, role, scope) and `DenyRule` (subject, permission, scope)
- `PolicyDecisionPoint` (`check`, `effectivePermissions`, `whoCan`)
- `ExplanationTrace` (ordered grant/deny path)

---

### 2. Traversals Done Right
- Three graphs are in play: role inheritance, group nesting, resource hierarchy. Each traversal needs visited-set protection and clear direction (up the resource tree, up the group tree, down the role tree).

---

### 3. Deny-Overrides Semantics
- Evaluate allows and denies as separate passes; a single well-named function decides the final verdict. Scattered early-returns are the classic failure mode.

---

### 4. Reverse Query
- `whoCan(action, resource)` requires reverse indexes (role → subjects, group → members expanded). Discuss the cost of expansion vs maintaining materialized indexes.

---

## Constraints

- Single-threaded; no persistence; no authentication (identity is given).
- Resource tree depth ≤ 10; role inheritance depth ≤ 5.
- Attribute-based rules (ABAC) are out of scope (see bonus).

---

## Example Scenario

```text
Roles:  VIEWER{READ}, EDITOR{READ,WRITE} inherits VIEWER, ADMIN{*} inherits EDITOR
Tree:   org:acme → project:42 → doc:99
Groups: eng = {u_bob, u_carol}; leads = {u_carol}

assign(group:eng,  EDITOR, scope=project:42)
assign(u_alice,    ADMIN,  scope=org:acme)
deny  (u_bob,      (WRITE, DOCUMENT), scope=doc:99)

check(u_alice, DELETE, doc:99) → ALLOW  (ADMIN @ org:acme, inherited down)
check(u_bob,   WRITE,  doc:99) → DENY   (EDITOR allow found, but explicit deny wins)
check(u_bob,   READ,   doc:99) → ALLOW  (eng → EDITOR → inherits VIEWER)
check(u_dave,  READ,   doc:99) → DENY   (no grant anywhere)

whoCan(WRITE, doc:99) → {u_alice, u_carol}   (u_bob excluded by deny)
```

---

## Bonus Challenges (If Time Permits)

- Add **time-boxed grants** (role valid until T) with an injected clock.
- Add **ABAC conditions** on assignments (e.g., only when `resource.owner == user`).
- Add **audit logging** of every `check` with its trace, and query by user.
- Implement **permission caching** with correct invalidation on any mutation.
- Discuss multi-tenancy: isolating two organizations in one deployment.
