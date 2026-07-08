# LLD Machine Coding – In-Memory Pub-Sub Message Queue (C++)

## Problem Statement

Design and implement an **in-memory Publish-Subscribe messaging system** (a mini Kafka) where publishers send messages to topics and multiple subscribers consume them independently, using Object-Oriented Programming in C++.

This problem is asked at Flipkart, Uber, LinkedIn, and Confluent interviews. It tests the **Observer pattern vs offset-based consumption**, **queue modelling**, **interface design**, and reasoning about **delivery semantics**.

---

## Functional Requirements

1. The system must support **topics**:
   - `createTopic(topicName)`
   - Publishers send messages to a topic: `publish(topicName, message)`.
2. **Subscribers** can subscribe to one or more topics:
   - `subscribe(topicName, subscriber)`
   - `unsubscribe(topicName, subscriber)`
3. Every subscriber of a topic receives **every message** published to it (fan-out) — one subscriber consuming a message does not remove it for others.
4. Each subscriber consumes **independently at its own pace**:
   - Each subscriber has its own **offset** per topic.
   - `poll(topicName, subscriber)` returns the next unread message and advances that subscriber's offset.
5. Messages within a topic are delivered to each subscriber **in publish order**.
6. The system must support **resetting a subscriber's offset** to re-consume from a given position: `resetOffset(topicName, subscriber, offset)`.
7. Support querying: message count in a topic and a subscriber's current offset (lag).

---

## Business Rules

- A subscriber that subscribes **after** messages were published starts from offset 0 by default (receives the backlog).
- Polling when there are no new messages returns "no message" (empty optional) — it must not block or crash.
- Unsubscribing discards the subscriber's offset; re-subscribing starts fresh.
- Messages are **immutable** once published.
- Topics are independent — ordering guarantees exist only **within** a topic.
- Publishing to a non-existent topic is an error (or auto-creates it — pick one and be consistent).

---

## Non-Functional Requirements

- In-memory only; messages live in a per-topic append-only log (vector).
- Standard C++ library only.
- Follow OOP principles with clear separation between broker, topic, and subscriber.
- No real threading required — a `poll()`-based pull model keeps it single-threaded.

---

## Design Expectations (What Interviewers Look For)

### 1. Class Modelling

Expected core classes:
- `Message` (id, payload, timestamp)
- `Topic` (name, append-only `vector<Message>` log)
- `Subscriber` (abstract interface: `onMessage(Message)` for push mode, or just an id for pull mode)
- `Subscription` (subscriber + topic + current offset)
- `Broker` / `PubSubService` (facade: manages topics, subscriptions, publish, poll)

---

### 2. Log + Offset Model (not a shared queue)
- Store messages in an append-only log per topic.
- Each subscription keeps its own offset into that log — this is what allows independent consumption and offset reset.
- Do NOT model this as one queue where consuming pops the message for everyone.

---

### 3. Push vs Pull
- Implement **pull** (`poll`) as the core.
- Discuss (or implement as bonus) **push** delivery: broker calls `subscriber.onMessage()` on publish — classic Observer pattern.

---

### 4. Delivery Semantics Discussion
- Be ready to explain what your design gives: at-most-once vs at-least-once per poll, and how offset advancement timing affects it.

---

## Constraints

- In-memory only; no persistence.
- Single-threaded; concurrency is NOT required (thread-safety discussion is a bonus).
- No network layer; everything is direct method calls.
- Message retention is unbounded (retention policy is a bonus).

---

## Example Scenario

```text
Setup: createTopic("orders")
       subscribe("orders", S1)
       subscribe("orders", S2)

Action: publish("orders", "order-1 placed")
Action: publish("orders", "order-2 placed")

Action: poll("orders", S1) → "order-1 placed"  (S1 offset: 0 → 1)
Action: poll("orders", S1) → "order-2 placed"  (S1 offset: 1 → 2)
Action: poll("orders", S1) → <no message>      (S1 caught up)

Action: poll("orders", S2) → "order-1 placed"  (S2 consumes independently)

Action: resetOffset("orders", S1, 0)
Action: poll("orders", S1) → "order-1 placed"  (re-consumed)

Query: topic "orders" size → 2 | S2 lag → 1
```

---

## Bonus Challenges (If Time Permits)

- Add **push-based delivery** with an Observer-style `onMessage` callback and a per-subscriber retry count on failure.
- Implement **retention policy**: keep only the last N messages and handle offsets pointing at evicted messages.
- Add **consumer groups**: subscribers in a group share one offset (each message consumed by exactly one member).
- Add **partitions** per topic with ordering guaranteed only within a partition.
- Make the broker **thread-safe** and discuss lock granularity.
