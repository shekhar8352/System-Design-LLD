# LLD Machine Coding – Task Management System (C++)

## Problem Statement

Design and implement a **Task Management System** (similar to Trello or Jira's core) using Object-Oriented Programming in C++.

This problem is commonly asked at Atlassian, Linear, Notion, and Freshworks. It tests your understanding of **hierarchical data modelling**, **role-based access**, **filtering**, and **notification systems**.

---

## Functional Requirements

1. The system must manage:
   - **Users** (admins and regular members)
   - **Projects** (containers for tasks)
   - **Tasks** (work items within a project)
   - **Comments** on tasks

2. Users should be able to:
   - Create a project.
   - Add members to a project.
   - Create tasks within a project.
   - Assign tasks to members.
   - Update task status.
   - Add comments to tasks.
   - Set due dates and priority on tasks.

3. The system should support:
   - **Filtering tasks** by: status, priority, assignee, due date.
   - **Searching tasks** by title keyword.
   - **Task history log** — every status change is recorded with timestamp and actor.
   - **Notifications** (in-memory): notify a user when a task is assigned to them or its due date changes.

---

## Business Rules

- Task statuses (in order): `TODO → IN_PROGRESS → IN_REVIEW → DONE`.
- Status can only move **forward** (no going back from DONE).
- Only a **project member** can create or update tasks in that project.
- Only the **task assignee or project admin** can move a task to `DONE`.
- Priority levels: `LOW`, `MEDIUM`, `HIGH`, `CRITICAL`.
- A task must belong to exactly one project.
- A user can be a member of multiple projects.
- Task due dates must be in the future when set.
- Comments are append-only (no editing or deletion).

---

## Non-Functional Requirements

- In-memory only.
- Standard C++ library only.
- Follow OOP principles (single responsibility, encapsulation).
- No UI needed.
- Notifications are in-memory (a simple notification queue per user).

---

## Design Expectations (What Interviewers Look For)

### 1. Class Modelling

Expected core classes:
- `User` (ID, name, role: `ADMIN` / `MEMBER`, notification inbox)
- `Project` (ID, name, member list, task list)
- `Task` (ID, title, description, assignee, status, priority, due date, comment list, history)
- `Comment` (author, text, timestamp)
- `TaskHistory` (actor, old status, new status, timestamp)
- `Notification` (message, timestamp, target user)
- `TaskManager` (orchestrates project and task operations)
- `NotificationService` (delivers notifications to user inboxes)

---

### 2. Role-Based Access Control
- `ADMIN`: can add/remove members, delete tasks, access all projects.
- `MEMBER`: can only act within projects they belong to.
- Access checks must be encapsulated (not scattered across all methods).

---

### 3. Filtering & Search
- `TaskManager::filterTasks(projectId, filterCriteria)` returns matching tasks.
- `FilterCriteria` should be composable (status AND priority AND assignee).
- Use a predicate-based approach if possible.

---

### 4. Audit Trail
- Every `Task` maintains a `vector<TaskHistory>` of all state changes.
- Each change records: who changed it, from what status, to what status, and when.

---

### 5. Notification System
- `NotificationService` maintains a `map<userId, queue<Notification>>`.
- Push notification when: task assigned, task due date changed, task status changes.
- Users can call `getNotifications()` to read their inbox.

---

## Constraints

- Data stored in memory only.
- No file system, database, or UI.
- Concurrency is NOT required.
- Timestamps can be simulated (incrementing integer or `std::time_t`).

---

## Example Scenario

```text
Setup:
  Users: Alice (ADMIN), Bob (MEMBER), Carol (MEMBER)
  Project: "Apollo Launch" | Members: Alice, Bob, Carol

Action: Alice creates task
  Title: "Design API schema"
  Priority: HIGH | Due: 2026-08-01 | Assignee: Bob
Output:
  Task T-001 created.
  Bob receives notification: "You were assigned task T-001"

Action: Bob starts task T-001
  Status: TODO → IN_PROGRESS
Output:
  History recorded: Bob changed status TODO → IN_PROGRESS at 2026-07-05 10:00

Action: Carol adds comment on T-001
  Comment: "Have you considered REST vs gRPC?"
Output: Comment added.

Action: Bob moves T-001 to IN_REVIEW
Action: Alice moves T-001 to DONE
Output: Task T-001 completed.

Action: Alice filters tasks in "Apollo Launch" by status=DONE
Output: [T-001 – "Design API schema" – DONE]

Action: Bob reads notifications
Output:
  [1] "You were assigned task T-001" @ 2026-07-01 09:00
```

---

## Bonus Challenges (If Time Permits)

- Add **subtasks** (tasks within tasks, one level deep).
- Add **labels/tags** for tasks (e.g., "backend", "urgent").
- Implement **task dependencies** (Task B cannot start until Task A is DONE).
- Add a **Sprint** concept: tasks grouped into time-boxed sprints with a start/end date.
