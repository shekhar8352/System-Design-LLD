# OOP Design Exercise – Library Book Management System (C++)

## Problem Statement

You are asked to design and implement a **Library Book Management System** using **Object-Oriented Programming principles in C++**.

The objective of this exercise is to evaluate your ability to:
- Model real-world entities using classes
- Define clear responsibilities
- Enable interaction between objects
- Write clean, maintainable OOP-based code

This is a **design-focused problem**, not a database or UI-heavy task.

---

## Functional Requirements

1. The system should manage:
   - Books
   - Library users
2. Each **Book** must have:
   - A unique book ID
   - Title
   - Author
   - Availability status
3. Each **User** must have:
   - A unique user ID
   - Name
   - A list of borrowed books
4. The system should support:
   - Borrowing a book
   - Returning a book
   - Viewing available books

---

## Business Rules

- A book can be borrowed by **only one user at a time**.
- A user can borrow **at most N books** (assume N = 3).
- A book that is already borrowed cannot be borrowed again.
- Only borrowed books can be returned.

---

## Non-Functional Requirements

- Use **C++** only (standard library).
- Follow **OOP principles strictly**.
- Avoid global variables.
- Keep class responsibilities well-defined.
- Console-based interaction is sufficient.

---

## Design Expectations (What Interviewers Look For)

### 1. Class Modeling
Expected core classes:
- `Book`
- `User`
- (Optional) `Library`

Each class should represent **one real-world concept**.

---

### 2. Encapsulation
- Internal state (e.g., book availability, user’s borrowed list) must not be directly modifiable.
- All validations should occur inside class methods.

---

### 3. Object Interaction
- Objects should collaborate via methods, not by exposing internal data.
- Avoid procedural-style logic in `main()`.

---

### 4. Responsibility Separation
Examples:
- `Book` should not know *who* is borrowing it.
- `User` should not directly modify book availability.
- `Library` (if used) should coordinate operations.

---

## Constraints

- Data is stored in memory only.
- No file system or database usage.
- Concurrency is not required.
- Do NOT use inheritance for this problem.

---

## Example Scenario

### Input / Actions
```text
Add Book:
ID: 1
Title: Clean Code
Author: Robert C. Martin

Add Book:
ID: 2
Title: Effective C++
Author: Scott Meyers

Add User:
ID: 101
Name: Sudhanshu

User 101 borrows Book 1
User 101 borrows Book 2
User 101 borrows Book 3
