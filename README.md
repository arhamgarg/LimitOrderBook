# Case Study: Limit Order Book Simulation using Red-Black Tree

**Course:** Data Structures and Algorithms (23CSE203)

**Project:** A Red-Black Tree based Limit Order Book simulation for High-Frequency Trading

---

## Project Overview

This repository implements a simplified Limit Order Book (LOB) simulator using Red-Black Trees (RBTs) as the primary data structure. The simulator is written in C++ and demonstrates how a balanced binary search tree can maintain ordered price levels efficiently for matching buy (bid) and sell (ask) orders.

This project is intended as a case study for understanding data structures (balanced BSTs) and their application in a latency-sensitive domain — financial market order matching.

---

## Why a Red-Black Tree?

A Limit Order Book must support:
- Fast insertion of price levels and orders.
- Fast lookup of best bid (maximum price) and best ask (minimum price).
- Efficient deletion/rebalancing when price levels are removed.

Red-Black Trees provide O(log n) average- and worst-case time for insertions, deletions, and lookups while remaining balanced. This makes RBTs a suitable choice for an educational LOB: they keep the tree height O(log n) and ensure predictable operation times.

> Real HFT systems often use specialized data structures (e.g., skip lists, binary heaps combined with hash maps, or custom flat arrays) and kernel/OS optimizations for performance. This project focuses on correctness and clarity.

---

## Repository Contents

- `LimitOrderBook.cpp` — single-file C++ implementation (contains `Node`, `RedBlackTree`, `LimitOrderBook`, and `main`).

---

## How to build and run

Requirements:
- A C++ compiler (e.g. `g++`) supporting C++11 or newer.

Build and run (Unix-like systems):

```bash
# Compile
g++ LimitOrderBook.cpp -o lob -std=c++11

# Run
./lob
```

This will run the sample `main()` included in the file and print the initial book, the best bid/ask, any trades produced during matching, and the book after matching.

---

## High-level design

There are three main classes in the implementation:

1. **Node** — represents a single tree node (a price level) with fields:
   - `double price` — price level
   - `int quantity` — aggregated quantity at that price level
   - `string color` — "RED" or "BLACK" used by Red-Black Tree algorithms
   - `Node *left, *right, *parent` — pointers for tree structure

2. **RedBlackTree** — balanced BST that stores `Node` objects ordered by `price`.
   - Provides standard RBT operations: `insert`, `deleteNode`/`remove`, rotations, `fixInsert`, `fixDelete`, and search helpers (`minimum`, `maximum`, `successor`, `predecessor`).
   - Uses a `NIL` sentinel node to simplify boundary logic.

3. **LimitOrderBook** — two RBT instances:
   - `bids` (stores buy orders; highest price = best bid)
   - `asks` (stores sell orders; lowest price = best ask)
   - Methods: `addOrder(price, qty, isBuy)`, `getBestBid()`, `getBestAsk()`, `matchOrders()`, and `displayBook(depth)`.

---

## Function & Module Documentation

Below are the most important functions and their responsibilities. (Names refer to those in LimitOrderBook.cpp.)

- **Node** (constructor)

     - Signature: Node(`double price`,`int quantity`)
     - Purpose: Create a new node representing a price level. Newly created nodes default to color RED (standard RBT insertion convention) and have null child/parent pointers.
     - Inputs: price (double), quantity (int)
     - Outputs: new Node pointer

- **RedBlackTree** — key methods

    **Note**: The tree stores nodes ordered by price. For bids, higher price is more important (maximum = best bid). For asks, lower price is better (minimum = best ask).

    1. **rotateLeft(`Node *x`) / rotateRight(`Node *x`)**

        - Purpose: Fundamental tree rebalancing primitives used by RBT algorithms.
        - Inputs: node x around which to rotate.
        - Effects: Rewires pointers between x, x->right (or x->left), and their children/parents.
        - **Complexity**: O(1).

    2. **fixInsert(`Node *z`)**

        - Purpose: After inserting a RED node, restores Red-Black properties using recoloring and rotations.
        - Inputs: the newly inserted node z.
        - **Complexity**: O(log n) in worst case (while loop up the tree).

    3. **insert(`double price`, `int qty`)**

        - Purpose: Insert a new price level into the tree. If a node with the exact price already exists, the implementation adds the quantity to that node rather than creating a duplicate.
        - Inputs: price, qty
        - Outputs: modifies the tree (adds or updates a node).
        - **Complexity**: Search + insert is O(log n) to find insertion point; fixInsert may take O(log n).

    4. **transplant(`Node *u`, `Node *v`)**

        - Purpose: Helper used by deletion to replace subtree rooted at u with subtree rooted at v.
        - **Complexity**: O(1).

    5. **deleteNode(`Node *z`) / remove (depending on exact symbol name used in code)**

        - Purpose: Remove node z (price level) from the tree and restore Red-Black properties with fixDelete.
        - Inputs: Node pointer z to remove.
        - **Complexity**: O(log n) for deletion and balancing.

    6. **fixDelete(`Node *x`)**

        - Purpose: After a physical deletion, restore RBT properties (color/structure) via rotations and recoloring.
        - **Complexity**: O(log n).

    7. **Search/Traversal helpers: search(`double price`), minimum(), maximum(), successor(`Node*`), predecessor(`Node*`), inorderHelper(`Node*`)**

        - Purpose: Standard BST utility functions used by the LOB to find best prices and iterate.
        - **Complexity**: search O(log n), minimum/maximum O(log n) worst-case (down to leaf), successor/predecessor O(log n) worst-case.

- **LimitOrderBook — key methods**

  1. addOrder(double price, int qty, bool isBuy)

      - Purpose: Add a new order to the book. If isBuy == true, it inserts into the bids tree; otherwise into asks.
      - Behavior: Uses the tree's insert() which accumulates quantity at an existing price level if present.
      - **Complexity**: O(log n).

  2. getBestBid() / getBestAsk()

      - Purpose: Return the best bid (maximum price) and best ask (minimum price) respectively.
      - Return: double price. If tree is empty, behavior depends on code (e.g., may return sentinel 0.0 — see source comments).
      - **Complexity**: O(log n) to find min/max.

  3. matchOrders()

      - Purpose: Execute naive crossing of orders while best bid price >= best ask price.

## Algorithm (implemented):

The core matching process in the **Limit Order Book** works as follows:

```cpp
Find bestBid = bids.maximum()
Find bestAsk = asks.minimum()

while (both exist && bestBid->price >= bestAsk->price) {
    tradedQty = min(bestBid->quantity, bestAsk->quantity)
    Print: "TRADE: " << tradedQty << " @ " << bestAsk->price

    // Subtract tradedQty from each node's quantity
    bestBid->quantity -= tradedQty
    bestAsk->quantity -= tradedQty

    // If a node's quantity reaches 0, move the pointer
    if (bestBid->quantity == 0) bestBid = predecessor(bestBid)
    if (bestAsk->quantity == 0) bestAsk = successor(bestAsk)
}
```

### Explanation

1. **Initialization**:
   - Best bid = maximum price in the bids tree.
   - Best ask = minimum price in the asks tree.

2. **Matching condition**:
   Orders match while `bestBid->price >= bestAsk->price`.

3. **Trade execution**:
   - `tradedQty = min(bestBid->quantity, bestAsk->quantity)`
   - Output trade at the ask price.

4. **Quantity update**:
   Subtract traded quantity from both price levels.

5. **Pointer update**:
   If any level’s quantity becomes 0, shift pointer to its predecessor (bid) or successor (ask).


### Important implementation notes / limitations:

- The current implementation does not remove nodes whose quantity becomes zero; it only advances the local pointer used during matching. That leaves zero-quantity price nodes in the tree. In practice you would call the tree's deletion routine to clean up empty price levels.

- The matching is price-time naive: this model aggregates at price levels and doesn't simulate order-by-order FIFO within the same price level.

- **Complexity**: Each loop iteration touches the best bid/ask. Number of iterations ≤ number of price levels consumed; each pointer movement or tree removal (if added) would be O(log n) per removal. Overall worst-case O(k log n) where k is number of matched price levels.

- **displayBook(int depth = 5)**

  - Purpose: Print top depth bids (descending) and asks (ascending) to the console.
  - **Complexity**: O(depth * log n) to find next predecessor/successor repeatedly.

---

## **Complexity** Summary

- **Insert (price level):** O(log n)
- **Search (by price):** O(log n)
- **Find best (min/max):** O(log n)
- **Delete (price level):** O(log n)
- **Match step (single trade between top-of-book levels):** O(1) for quantity arithmetic; moving to next level or deleting a level is O(log n).

Space **complexity**: O(n) nodes where n is number of distinct price levels stored.

---

## Example run (from provided `main()`)

Input (orders added in `main()`):
- Bids: (100.5, 50), (101.0, 30), (99.5, 40)
- Asks: (102.0, 20), (103.5, 10), (100.0, 25)

Output (representative):
```
---- BIDS ----
101 : 30
100.5 : 50
99.5 : 40
---- ASKS ----
100 : 25
102 : 20
103.5 : 10
Best Bid: 101
Best Ask: 100

--- Matching Orders ---
TRADE: 25 @ 100

--- After Matching ---
---- BIDS ----
101 : 5
100.5 : 50
99.5 : 40
---- ASKS ----
100 : 0
102 : 20
103.5 : 10
```

---

## Applications & Limitations

**Applications**
- Demonstration of balanced BSTs and LOBs.
- Reference implementation for coursework.

**Limitations**
- No per-order FIFO at price levels (only aggregates).
- Zero-quantity price nodes are not deleted.
- Simplified logic compared to real HFT engines.

---

## Contributors :

- **[Arham Garg](https://github.com/arhamgarg)**
- **[S S Naveen](https://github.com/Naveen77qwerty)**
- **[A Adithyan](https://github.com/Cirutuu)**
- **[H Dharshan](https://github.com/Dharshan2208)**
