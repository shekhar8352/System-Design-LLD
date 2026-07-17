# LLD Machine Coding – Online Auction System (C++)

## Problem Statement

Design and implement an **Online Auction System** (like eBay) where sellers list items, buyers place bids under auction rules, and the system determines winners at close, using Object-Oriented Programming in C++.

This problem is asked at Amazon, eBay, Flipkart, and Meesho interviews. It tests **auction state machines**, **bid validation rules**, the **Observer pattern** (outbid notifications), and time-driven behaviour with an injected clock.

---

## Functional Requirements

1. Auction lifecycle:
   - `createAuction(sellerId, item, startPrice, minIncrement, startTime, endTime)`.
   - Auctions transition `SCHEDULED → ACTIVE → CLOSED` based on time; sellers may `cancel` before any bid is placed.
2. Bidding:
   - `placeBid(auctionId, bidderId, amount)` — valid only while `ACTIVE`.
   - A valid bid must be ≥ `currentHighest + minIncrement` (or ≥ `startPrice` for the first bid).
3. **Automatic (proxy) bidding**: a bidder may set a `maxAutoBid`; the system bids on their behalf just enough to stay highest, up to their maximum.
4. Closing:
   - At `endTime`, the highest bidder wins at their **standing bid amount**; the seller and winner are recorded.
   - An auction with no bids closes as `UNSOLD`.
5. Notifications (Observer): a bidder who gets **outbid** receives a notification; all participants are notified on close.
6. Queries:
   - Current highest bid and bidder (highest bidder's identity may be shown, but their `maxAutoBid` must never be exposed).
   - Bid history for an auction; auctions won by a user.

---

## Business Rules

- Sellers cannot bid on their **own auctions**.
- Bids below the required minimum are rejected without recording.
- Proxy-bid resolution: when a new bid arrives against a proxy holder, the system auto-raises the proxy holder to `min(newBid + minIncrement, maxAutoBid)`. If the new bid exceeds `maxAutoBid`, the new bidder becomes highest.
- Two proxy bidders resolve to: higher max wins, standing at `min(lowerMax + minIncrement, higherMax)`.
- Bids on `SCHEDULED`/`CLOSED` auctions are rejected; time comes from an injected clock (no sleeping).
- A bidder cannot lower their own existing proxy max; raising is allowed.
- Cancel is rejected once any bid exists.

---

## Non-Functional Requirements

- In-memory only.
- Standard C++ library only.
- Follow OOP principles; bid-validation rules should be readable and centralized, not scattered.
- No UI/network; demonstrate via `main()` with a scripted timeline.

---

## Design Expectations (What Interviewers Look For)

### 1. Class Modelling

Expected core classes:
- `Auction` (item, seller, times, state, minIncrement, bid history, highest bid, proxy registry)
- `Bid` (bidder, amount, timestamp, isAutoBid flag)
- `User` (id, name)
- `AuctionService` (createAuction, placeBid, setProxyBid, closeDueAuctions(now), queries)
- `AuctionObserver` (interface) + `NotificationService` implementation
- Enums: `AuctionState` (SCHEDULED, ACTIVE, CLOSED, CANCELLED), `AuctionResult` (SOLD, UNSOLD)

---

### 2. Proxy Bid Algorithm
- The heart of the problem: a single well-tested function resolving a new bid against the current proxy state.
- Bid history should show the **auto-generated counter-bids**, flagged as automatic.

---

### 3. State Machine + Time
- State transitions driven by `closeDueAuctions(now)` / lazy checks on access — no background threads.
- All time-dependent rules testable with a `FakeClock`.

---

### 4. Observer Usage
- Outbid and auction-closed events flow through an observer interface; the auction core must not depend on any concrete notifier.

---

## Constraints

- English (ascending price) auctions only; single item per auction, single winner.
- Payment, escrow, and shipping are out of scope.
- Single-threaded; concurrency is NOT required.

---

## Example Scenario

```text
createAuction(A1, seller=S, start=₹100, minInc=₹10, end=t100)

t10: bid(B1, ₹100)              → highest: B1 @ 100
t20: setProxyBid(B2, max=₹200)  → auto-bid: B2 @ 110 | B1 notified (outbid)
t30: bid(B1, ₹150)              → auto-raise: B2 @ 160 | B1 notified (outbid)
t40: bid(B1, ₹250)              → exceeds B2's max → highest: B1 @ 250? 
                                   NO — B1 must only need ₹210 (200+10): highest B1 @ 210
                                   B2 notified (outbid, max exhausted)
t100: close → SOLD to B1 @ ₹210 | all participants notified

Auction with no bids at end → CLOSED, UNSOLD
bid on closed auction → REJECTED
```

---

## Bonus Challenges (If Time Permits)

- Add **anti-sniping**: a bid in the last N seconds extends `endTime` by M seconds.
- Add **reserve price**: highest bid below reserve closes as UNSOLD (reserve never revealed).
- Add **Buy-It-Now** that ends the auction instantly if no bids exceed a threshold.
- Support **Dutch auctions** (descending price) behind a common `Auction` interface.
- Discuss concurrency: two bids arriving "simultaneously" and how you'd serialize per-auction.
