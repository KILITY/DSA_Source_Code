# B-Tree Validator

---

## What Is Being Tested

The testing system validates **all core B-Tree properties**, including:

- Each node contains **between `t-1` and `2t-1` keys** (except the root)
- Keys inside each node are **strictly sorted**
- Internal nodes have exactly **`keys + 1` children**
- All child subtrees contain keys within the **correct parent-defined interval**
- **All leaves are at the same depth**
- Root shrink after deletions is handled correctly
- Borrow and merge operations preserve structure
- The tree remains valid after **every insert and every delete**
- The tree can be fully cleared without breaking invariants

If **any property is violated**, the validator immediately stops and reports:
- the operation type (insert / delete / clear)
- the index and key involved
- the random seed used
- the exact invariant that failed

---

## Why a Validator Instead of Classic Unit Tests

Classic unit tests (insert X, expect structure Y) are not practical for B-Trees because:

- valid B-Tree structures are not unique
- delete operations can trigger deep, cascading structural changes
- many bugs only appear after long sequences of mixed operations

Instead, this project uses **property-based testing**:

> The test does not check *what the tree looks like*,  
> it checks *whether the tree is still a valid B-Tree*.

This is the same strategy used in:
- database index testing
- compiler data structures
- file systems
- operating system kernels

---

## How Randomized Testing Works

The test uses **deterministic randomness**.

### Step-by-step process:

1. A pool of integers is created: 1, 2, 3, ..., 4n

2. The pool is **shuffled using a fixed seed** and mersenne_twister_engine.

3. The first `n` values are inserted into the tree, one by one.

4. The inserted values are shuffled again.

5. `⌊n / 2⌋` values are deleted in random order.

6. The remaining values are deleted to fully clear the tree. --Yes this could be one step, but why not?

7. The validator is run **after every single operation**.

Because a fixed seed is used:
- every test run is **reproducible**
- crashes and invariant violations can be replayed exactly
- debugging is deterministic

---

## Why Shuffling Instead of Generating Random Numbers

The tests intentionally use **shuffling instead of raw random number generation** because:

- B-Trees require **unique keys**
- duplicates would hide bugs or reduce coverage
- the *order* of insertion matters, not the numeric magnitude
- shuffling guarantees uniform randomness without duplicates

---

## What Happens on Failure

If the validator detects a problem, the test stops immediately and returns a message such as:

FAIL: validator failed after DELETE | key=836 | step=46 | t=3 | n=100 | seed=123 | reason="internal node children != keys + 1"

This information is sufficient to:
- reproduce the failure
- identify the faulty operation
- debug the exact code path

---

## How to Run the Tests

Example usage in `main`:

```c++
std::cout << runBTreeGeneratedTest(3, 1000, 123) << std::endl;