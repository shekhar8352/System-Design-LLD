# LLD Machine Coding – Coupon & Discount Engine (C++)

## Problem Statement

Design and implement a **Coupon & Discount Engine** for an e-commerce checkout that evaluates a cart against a set of configurable discount rules and applies the best valid combination, using Object-Oriented Programming in C++.

This problem is asked at Flipkart, Swiggy, Zepto, and Amazon interviews. It tests **rule-engine design (Strategy + Composite)**, **eligibility predicates**, **discount stacking policies**, and clean separation of rule *definition* from rule *evaluation*.

---

## Functional Requirements

1. Coupon management:
   - `createCoupon(code, discountType, eligibilityRules, validity, usageLimits)`.
   - Discount types: **flat amount** (₹100 off), **percentage** (10% off, with optional cap), **item-level** (cheapest item free / Buy-X-Get-Y).
2. Eligibility rules, composable with AND/OR:
   - Minimum cart value, specific category, first-order-only, user segment (e.g., PREMIUM), payment method, valid time window.
3. Applying coupons:
   - `applyCoupon(cart, code)` — validates eligibility and returns the discounted cart breakdown, or a **specific rejection reason**.
   - `bestCoupon(cart, userId)` — among all auto-applicable coupons, pick the one giving maximum discount.
4. Usage limits:
   - Global cap (first 1000 uses) and per-user cap (once per user), enforced at **order confirmation**, not at preview.
5. Stacking:
   - At most one **cart-level** coupon plus at most one **payment-offer** coupon per order; item-level and cart-level discounts compose in a defined order.
6. Queries:
   - Coupon details, remaining global uses, a user's coupon usage history.

---

## Business Rules

- Percentage discounts apply on the **post-item-level-discount** subtotal; caps are applied after computing the raw percentage.
- A discount can never exceed the payable amount (no negative totals).
- Expired, exhausted, or ineligible coupons are rejected with a machine-readable reason (`EXPIRED`, `MIN_CART_NOT_MET`, `USER_LIMIT_REACHED`, ...).
- Buy-X-Get-Y: the **cheapest** qualifying items become free; document tie-breaking.
- Preview (`applyCoupon`) must be side-effect free; usage counters change only on `confirmOrder(cart, codes)`.
- Rule evaluation must be deterministic — same cart + same coupons ⇒ same result.

---

## Non-Functional Requirements

- In-memory only.
- Standard C++ library only.
- Follow OOP principles; adding a new rule type or discount type must not modify the evaluation engine (Open/Closed Principle).
- No UI/network; demonstrate via `main()` with several carts and coupons.

---

## Design Expectations (What Interviewers Look For)

### 1. Class Modelling

Expected core classes:
- `Cart` / `CartItem` (sku, category, unitPrice, quantity)
- `Coupon` (code, `Discount`, `EligibilityRule`, validity window, usage limits)
- `Discount` (abstract) → `FlatDiscount`, `PercentDiscount(cap)`, `BuyXGetYDiscount`
- `EligibilityRule` (abstract predicate) → `MinCartValueRule`, `CategoryRule`, `FirstOrderRule`, `AndRule`, `OrRule` (Composite)
- `CouponEngine` (applyCoupon, bestCoupon, confirmOrder)
- `UsageTracker` (global + per-user counters)
- `DiscountBreakdown` (line-by-line explanation of the final price)

---

### 2. Composite Rule Evaluation
- Eligibility rules form a tree evaluated against a `CartContext` (cart + user + payment + time).
- Rejection should surface **which** rule failed, not just a boolean.

---

### 3. Stacking & Ordering Policy
- A single, clearly documented pipeline: item-level discounts → cart-level coupon → payment offer.
- The pipeline should be data-driven so the order is testable and changeable.

---

### 4. Preview vs Commit
- Clean separation between pure evaluation and state mutation (usage counters), so `bestCoupon` can evaluate dozens of coupons safely.

---

## Constraints

- Single currency, integer paise/rupee arithmetic (avoid floating-point money).
- No persistence; no concurrency required.
- Coupon codes are unique strings.

---

## Example Scenario

```text
Coupons:
  WELCOME100: flat ₹100 off, rule = FirstOrder AND MinCart(₹500)
  SAVE10:     10% off capped ₹200, rule = MinCart(₹1000)
  B2G1SHOES:  Buy 2 Get 1 free, rule = Category(SHOES)

Cart(user=U1, first order): 3 shoes @ ₹800 + 1 shirt @ ₹500 → subtotal ₹2900

applyCoupon(cart, B2G1SHOES) → cheapest shoe free → −₹800 → ₹2100
bestCoupon(cart, U1)         → SAVE10 on ₹2100 = ₹210 → capped ₹200 → ₹1900
                               WELCOME100 gives only ₹100 → SAVE10 wins

confirmOrder(cart, [B2G1SHOES, SAVE10]) → OK, counters incremented
applyCoupon(cart2, WELCOME100) for U1's 2nd order → REJECTED (FIRST_ORDER_ONLY)
```

---

## Bonus Challenges (If Time Permits)

- Add **coupon priority tiers** and exclusivity flags (a coupon that cannot stack with anything).
- Add **scheduled flash-sale coupons** activated by an injected clock.
- Support **partial refunds**: reverse a used coupon and restore usage counts.
- Add a small **DSL/JSON loader** that builds the rule tree from text.
- Discuss how you'd evaluate `bestCoupon` efficiently with 10,000 active coupons.
