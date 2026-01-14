# OOP Design Exercise – Bank Account Management System (C++)

## Problem Statement

You are asked to design and implement a **Bank Account Management System** using **Object-Oriented Programming principles in C++**.

The goal of this exercise is **not** to build a complete banking application, but to demonstrate your understanding of:
- Class design
- Encapsulation
- Object interaction
- Clean and maintainable code

You should focus on **design quality** as much as correctness.

---

## Functional Requirements

1. The system should allow creation of bank accounts.
2. Each bank account must have:
   - A unique account number
   - Account holder name
   - Current balance
3. The system should support:
   - Depositing money
   - Withdrawing money
   - Checking account balance
4. The system must prevent:
   - Withdrawal of more money than the available balance
   - Deposits of negative amounts

---

## Non-Functional Requirements

- Use **C++** (standard library only).
- Follow **OOP principles** strictly.
- Keep data members **private** and expose behavior through public methods.
- Avoid global variables.
- The design should be easy to extend later (e.g., different account types).

---

## Design Expectations (What Interviewers Look For)

### 1. Class Design
- A well-defined `BankAccount` class.
- Clear separation between **data** and **behavior**.
- Logical grouping of responsibilities.

### 2. Encapsulation
- Balance must not be directly modifiable from outside the class.
- All validations must happen inside class methods.

### 3. Constructors & Initialization
- Proper constructor to initialize account details.
- Avoid partially initialized objects.

### 4. Error Handling
- Graceful handling of invalid operations (e.g., insufficient balance).
- Clear method return values or messages.

---

## Constraints

- Assume all data is in-memory (no database).
- Concurrency is NOT required.
- UI is NOT required (console interaction is sufficient).
- Do NOT use inheritance for this problem.

---

## Example Scenario

### Input / Actions
```text
Create Account:
Account Number: 1001
Account Holder: Sudhanshu Shekhar
Initial Balance: 5000

Deposit: 2000
Withdraw: 1000
Withdraw: 7000
