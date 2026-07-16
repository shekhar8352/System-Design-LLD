# LLD Machine Coding – Feature Flag & Rollout System (C++)

## Problem Statement

Design and implement a **Feature Flag System** that evaluates whether a feature is enabled for a given user, supporting boolean flags, targeting rules, percentage rollouts, and environment-scoped configuration, using Object-Oriented Programming in C++.

This problem is asked at Atlassian, LaunchDarkly-style startups, Uber, and Stripe interviews. It tests **rule evaluation order**, **deterministic percentage bucketing (hashing)**, **environment/config layering**, and audit-friendly design.

---

## Functional Requirements

1. Flag management:
   - `createFlag(key, type, defaultValue)` — types: **BOOLEAN**, **STRING variant** (e.g., `"v1"/"v2"/"control"`).
   - `setEnabled(key, env, bool)` — a kill switch per environment (`DEV`, `STAGING`, `PROD`).
   - `archiveFlag(key)` — archived flags always evaluate to the default and reject rule edits.
2. Targeting rules, evaluated in priority order (first match wins):
   - **User allowlist/denylist** (explicit user ids).
   - **Attribute rules**: e.g., `country == "IN"`, `plan in {PRO, ENTERPRISE}`, `appVersion >= 3.2`.
   - **Percentage rollout**: enable for N% of users, deterministically.
3. Evaluation:
   - `evaluate(flagKey, env, UserContext)` → value + **evaluation reason** (`KILL_SWITCH`, `ALLOWLIST`, `RULE_MATCH(rule#)`, `ROLLOUT_BUCKET`, `DEFAULT`).
4. Percentage rollout semantics:
   - The same user must always land in the same bucket for the same flag (hash of `flagKey + userId`), and raising 10% → 20% must keep the original 10% enabled.
5. Change history:
   - Every mutation (rule change, toggle, rollout %) is recorded with who/when; `getHistory(flagKey)` returns it.

---

## Business Rules

- Rule order: kill switch → denylist → allowlist → attribute rules (by priority) → percentage rollout → default.
- Evaluation must be **pure** — it never mutates state and never returns different results for identical inputs (given the same config version).
- Unknown flag keys evaluate to a supplied caller default, with reason `FLAG_NOT_FOUND` (never throw in the hot path).
- Missing user attributes make an attribute rule **not match** (never error).
- Environments are fully isolated: PROD rules never leak into DEV.
- A variant flag's rules resolve to a **variant name** that must exist on the flag.

---

## Non-Functional Requirements

- In-memory only.
- Standard C++ library only (use `std::hash` or a simple FNV hash for bucketing).
- Follow OOP principles; adding a new rule type (e.g., regex match) must not modify the evaluator core.
- No UI/network; demonstrate via `main()`.

---

## Design Expectations (What Interviewers Look For)

### 1. Class Modelling

Expected core classes:
- `Flag` (key, type, variants, default, per-env `FlagConfig`)
- `FlagConfig` (enabled, ordered rules, rollout %, allow/deny lists)
- `Rule` (abstract) → `AttributeRule` (attribute, operator, value), `PercentageRule`
- `UserContext` (userId + attribute map of string/number values)
- `EvaluationResult` (value, reason, matched rule id)
- `FlagService` (create, mutate, evaluate, history)
- `ChangeLog` / `ChangeRecord`

---

### 2. Deterministic Bucketing
- `bucket = hash(flagKey + ":" + userId) % 100`; enabled iff `bucket < rolloutPercent`.
- Show why hashing per-flag (not just per-user) avoids correlated rollouts across flags.

---

### 3. Operator Design
- Attribute operators (`==`, `in`, `>=` with semantic version comparison) modeled as polymorphic matchers, not a giant if/else on strings.

---

### 4. Reasoned Evaluation
- Returning **why** a flag evaluated the way it did is the mark of production experience — design `EvaluationResult` first-class, not as an afterthought.

---

## Constraints

- Single-threaded; no persistence; no real network/SDK distribution.
- Attribute values: strings, numbers, and semantic versions.
- At most ~50 rules per flag — clarity beats micro-optimization.

---

## Example Scenario

```text
createFlag("new_checkout", BOOLEAN, default=false)
setEnabled("new_checkout", PROD, true)
addAllowlist(PROD, ["u_alice"])
addRule(PROD, priority=1, country == "IN" → true)
setRollout(PROD, 20%)

evaluate(PROD, u_alice, {country: US})  → true  (ALLOWLIST)
evaluate(PROD, u_bob,   {country: IN})  → true  (RULE_MATCH #1)
evaluate(PROD, u_carol, {country: US})  → bucket(new_checkout:u_carol)=7  → true (ROLLOUT)
evaluate(PROD, u_dave,  {country: US})  → bucket=63 → false (DEFAULT)

setRollout(PROD, 40%) → u_carol still true (sticky), u_dave re-checked: 63 ≥ 40 → false
setEnabled("new_checkout", PROD, false) → everyone false (KILL_SWITCH), rules untouched

evaluate(DEV, u_bob, {country: IN}) → false (DEV config is independent)
```

---

## Bonus Challenges (If Time Permits)

- Add **flag prerequisites** (flag B only evaluates if flag A is on) with cycle detection.
- Add **scheduled changes** ("enable at t=T") driven by an injected clock.
- Support **config snapshots/versioning** and evaluate against a pinned version.
- Add **segment definitions** (named, reusable rule groups shared across flags).
- Discuss how clients would receive config updates in a real distributed setup (poll vs stream).
