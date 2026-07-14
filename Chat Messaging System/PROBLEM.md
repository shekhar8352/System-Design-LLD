# LLD Machine Coding – Chat & Group Messaging System (C++)

## Problem Statement

Design and implement an **in-memory Chat System** supporting one-to-one and group conversations, message delivery/read receipts, unread counts, and message operations (edit, delete, reply), using Object-Oriented Programming in C++.

This problem is asked at WhatsApp/Meta, Slack, Microsoft (Teams), and Zoho interviews. It tests **conversation/message modelling**, **per-participant state tracking (receipts, unread)**, **Observer pattern for delivery**, and pagination-style history queries.

---

## Functional Requirements

1. Conversations:
   - `createDirectChat(userA, userB)` — at most one direct chat per user pair (idempotent).
   - `createGroup(name, ownerId, members)`; owner/admins can `addMember`, `removeMember`, `promoteAdmin`.
2. Messaging:
   - `sendMessage(conversationId, senderId, text)` → messageId, timestamped via injected clock.
   - `replyTo(conversationId, senderId, text, parentMessageId)` — threaded reply referencing an existing message.
   - `editMessage(messageId, senderId, newText)` — sender-only, within an edit window (e.g., 15 min).
   - `deleteMessage(messageId, senderId)` — sender deletes for everyone; the message shows as a tombstone ("message deleted"), replies to it remain.
3. Receipts & unread:
   - Per recipient, a message is `SENT → DELIVERED → READ`; `markDelivered(userId, conversationId)` and `markRead(userId, conversationId, uptoMessageId)`.
   - `unreadCount(userId, conversationId)` and total unread across conversations.
   - Group receipts: message is "read by all" only when every current member has read it.
4. History:
   - `getMessages(conversationId, beforeMessageId, limit)` — newest-first pagination.
   - `search(conversationId, keyword)` — simple substring match.
5. Notifications:
   - Online users (per a presence flag) receive new-message events via an **Observer**; offline users' messages await `markDelivered`.

---

## Business Rules

- Only current members can send to or read a conversation; removed members keep history up to removal but receive nothing after.
- Read implies delivered; receipts are **monotonic** (never downgrade READ → DELIVERED).
- `markRead(..., uptoMessageId)` reads everything at or before that message — O(1)-ish bookkeeping via a per-user "last read" watermark, not per-message flags.
- Editing is blocked after the window or after deletion; edited messages carry an `(edited)` marker and edit history.
- Deleting is sender-only (admins may also delete in groups — document your choice).
- A user's own messages never count as unread.
- Group must always have ≥ 1 admin: last admin leaving auto-promotes the oldest member (document this).

---

## Non-Functional Requirements

- In-memory only.
- Standard C++ library only.
- Follow OOP principles; receipt bookkeeping must scale as O(members), not O(members × messages).
- No real networking/threads; presence is a simple flag. Demonstrate via `main()`.

---

## Design Expectations (What Interviewers Look For)

### 1. Class Modelling

Expected core classes:
- `User` (id, name, online flag)
- `Conversation` (abstract) → `DirectChat`, `GroupChat` (members, admins)
- `Message` (id, sender, text, timestamp, state, parentId, edit history, deleted flag)
- `ReadWatermark` (per user per conversation: lastDeliveredMsgId, lastReadMsgId)
- `ChatService` (public API)
- `MessageObserver` (interface) + `NotificationService`
- Enums: `ReceiptState`, `MemberRole`

---

### 2. Watermark-Based Receipts
- The key insight: store one watermark per (user, conversation) instead of a receipt per (user, message). Unread count = messages after the watermark.
- "Read by all" in a group = min over members' watermarks.

---

### 3. Membership Changes vs History
- Removed members' watermark freezes; joining members' unread starts from join time. This interaction is a classic correctness trap.

---

### 4. Observer Boundaries
- The conversation core emits events; notification/presence logic stays outside it.

---

## Constraints

- Text messages only (no media); single device per user.
- Single-threaded; no persistence; no encryption.
- Message ids are globally unique and monotonically increasing (sortable by time).

---

## Example Scenario

```text
createGroup("proj-x", owner=A, members=[A,B,C])

A sends m1 "kickoff at 5"      → B (online) notified; C offline
markDelivered(C)               → m1 DELIVERED for C
unreadCount(B) = 1, unreadCount(C) = 1

markRead(B, upto=m1)           → readByAll? No (C hasn't read)
markRead(C, upto=m1)           → m1 read-by-all ✓

B replies m2 (parent=m1) "works for me"
A edits m1 → "kickoff at 5:30 (edited)"
A deletes m1 → tombstone; m2 still shows, parent renders as deleted

addMember(D) → D's unread starts empty (no m1/m2 backlog counted)
removeMember(C) → C can read history ≤ removal, gets nothing new
```

---

## Bonus Challenges (If Time Permits)

- Add **reactions** (emoji per user per message) with add/remove semantics.
- Add **mentions** (`@user`) generating a separate mention-badge count.
- Add **mute** per conversation (suppresses notifications, unread still counts).
- Add **disappearing messages** (TTL per conversation) swept via the injected clock.
- Discuss multi-device sync: what changes when a user has 2 devices with separate delivery states?
