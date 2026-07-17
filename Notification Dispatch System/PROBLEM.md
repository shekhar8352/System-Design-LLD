# LLD Machine Coding – Notification Dispatch System (C++)

## Problem Statement

Design and implement a **Notification Dispatch System** that sends notifications to users over multiple channels (EMAIL, SMS, PUSH) with user preferences, retries, and rate control, using Object-Oriented Programming in C++.

This problem is asked at Amazon, Flipkart, Swiggy, and Razorpay interviews. It tests the **Strategy pattern** (channel senders), **Chain of Responsibility / fallback logic**, **retry handling**, and clean separation between notification composition and delivery.

---

## Functional Requirements

1. Support sending a notification to a user:
   - `send(userId, message, type)` where type is `TRANSACTIONAL` or `PROMOTIONAL`.
2. Support **multiple channels**: `EMAIL`, `SMS`, `PUSH`. Each user registers contact details per channel.
3. Users have **preferences**:
   - Opt-in/opt-out per channel per notification type (e.g., no promotional SMS).
   - A **preferred channel order** (e.g., PUSH → EMAIL → SMS).
4. Delivery uses **fallback**: try the highest-priority allowed channel; if delivery fails, fall back to the next channel in order.
5. Support **retries**: each channel attempt is retried up to `maxRetries` times before falling back.
6. Maintain a **delivery log**: every attempt records channel, timestamp, status (SUCCESS / FAILED), and attempt number.
7. Query APIs:
   - Notification status by notificationId (`DELIVERED`, `FAILED_ALL_CHANNELS`, `SUPPRESSED_BY_PREFERENCE`).
   - Delivery history for a user.

---

## Business Rules

- `TRANSACTIONAL` notifications **ignore promotional opt-outs** but still respect channel availability (user must have contact info for the channel).
- `PROMOTIONAL` notifications must be fully suppressed if the user opted out of all channels — status `SUPPRESSED_BY_PREFERENCE`.
- A notification is `DELIVERED` on the **first successful channel attempt**; remaining channels are not tried.
- If all allowed channels exhaust their retries, status is `FAILED_ALL_CHANNELS`.
- Channel senders are unreliable — simulate failure via an injectable failure policy (e.g., a `FakeSender` that fails the first N attempts) so behaviour is deterministic and testable.
- A user with no registered contact detail for a channel skips that channel silently (it is not a failed attempt).

---

## Non-Functional Requirements

- In-memory only.
- Standard C++ library only.
- Follow OOP principles; adding a new channel (e.g., WHATSAPP) must not modify existing sender classes.
- No real network calls; senders print/simulate delivery. Demonstrate via `main()`.

---

## Design Expectations (What Interviewers Look For)

### 1. Class Modelling

Expected core classes:
- `Notification` (id, userId, message, type, status, createdAt)
- `User` (id, contact details per channel, `NotificationPreferences`)
- `NotificationPreferences` (opt-in matrix, channel priority order)
- `ChannelSender` (abstract: `send(user, message) → bool`) + `EmailSender`, `SmsSender`, `PushSender`
- `DeliveryAttempt` (notificationId, channel, attemptNo, status, timestamp)
- `NotificationService` (orchestrates preference check → channel ordering → retry → fallback)
- Enums: `Channel`, `NotificationType`, `NotificationStatus`, `AttemptStatus`

---

### 2. Strategy + Fallback
- Each channel is a `ChannelSender` strategy behind a common interface.
- Fallback across channels reads like a Chain of Responsibility; retries live per-channel, not in `main`.

---

### 3. Preference Resolution
- A single, well-named method should answer: "which channels, in what order, may this notification use?"
- Transactional vs promotional rules must be in one place, not scattered `if`s.

---

### 4. Testability
- Failure behaviour of senders must be injectable (constructor-injected policy or mock sender).
- Delivery log should make it easy to assert the exact sequence of attempts.

---

## Constraints

- Single-threaded; no async queues or actual scheduling required.
- No persistence; in-memory logs are enough.
- No template rendering / localization required (plain string messages).

---

## Example Scenario

```text
User u1: PUSH + EMAIL registered, prefers [PUSH, EMAIL, SMS], opted out of promotional EMAIL
maxRetries per channel = 2

send(u1, "OTP 1234", TRANSACTIONAL)
  → PUSH attempt 1: FAILED
  → PUSH attempt 2: FAILED
  → EMAIL attempt 1: SUCCESS
  → Notification DELIVERED (3 attempts logged)

send(u1, "50% off!", PROMOTIONAL)
  → PUSH attempt 1: SUCCESS
  → Notification DELIVERED (EMAIL never considered — but would be skipped anyway due to opt-out)

User u2: opted out of ALL promotional channels
send(u2, "Sale!", PROMOTIONAL) → SUPPRESSED_BY_PREFERENCE (zero attempts)
```

---

## Bonus Challenges (If Time Permits)

- Add **rate limiting per user** (max N promotional notifications per day).
- Add **scheduled notifications** (deliver at a future timestamp using an injected clock).
- Add **bulk send** to a user segment with per-user status reporting.
- Add an **Observer** so other components can react to delivery events.
- Discuss how you would make dispatch **asynchronous** with a worker pool.
