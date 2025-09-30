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

This project was developed collaboratively as part of the Data Structures and Algorithms (23CSE203) course. Each team member contributed significantly to different aspects of the implementation, bringing together expertise in algorithms, data structures, and financial systems.

🟣 Arham Garg

<p align="center">
  <a href="https://github.com/arhamgarg">
    <img src="https://github.com/arhamgarg.png" width="150px" style="border-radius: 50%;" alt="Arham Garg"/>
  </a>
</p>
<p align="center">
  <a href="https://github.com/arhamgarg">@arhamgarg</a>
</p>
Arham served as the project coordinator, managing the repository and ensuring smooth integration of all features. He was responsible for merging pull requests and maintaining code quality throughout development.
Key Contributions:

* Initial project setup and repository structure ([commit 3c2bcfc](https://github.com/arhamgarg/LimitOrderBook/commit/3c2bcfc9841dfb94b173e76668823383c9d17fe3))
* Implemented the foundational Node class for Red-Black Tree structure([commit c371c7d](https://github.com/arhamgarg/LimitOrderBook/commit/c371c7d70ff7c6d06d6d04d366c865a5effca0ac))
* Developed search and retrieval operations including minimum(), maximum(), predecessor(), and successor() methods([commit 96b2551](https://github.com/arhamgarg/LimitOrderBook/commit/96b2551aaf34c755b0866f38078ecf3366f9e131))
* Integrated all feature branches through PR reviews and merges
* Managed version control and ensured code consistency across the project

Merged Pull Requests:

* [PR #10 : Contributions with detailed history and profiles](https://github.com/arhamgarg/LimitOrderBook/pull/10)
* [PR #9: Comprehensive README and algorithm documentation](https://github.com/arhamgarg/LimitOrderBook/pull/9)
* [PR #8: Example LOB driver integration](https://github.com/arhamgarg/LimitOrderBook/pull/8)
* [PR #7: Order matching and display features](https://github.com/arhamgarg/LimitOrderBook/pull/7)
* [PR #6: Core LOB implementation](https://github.com/arhamgarg/LimitOrderBook/pull/6)
* [PR #5: Search and retrieval methods](https://github.com/arhamgarg/LimitOrderBook/pull/5)
* [PR #4: Insert functionality](https://github.com/arhamgarg/LimitOrderBook/pull/4)
* [PR #3: Fix insertion logic](https://github.com/arhamgarg/LimitOrderBook/pull/3)
* [PR #2: Rotation operations](https://github.com/arhamgarg/LimitOrderBook/pull/2)
* [PR #1: Initial node structure](https://github.com/arhamgarg/LimitOrderBook/pull/1)

Technical Focus: Project Management • Tree Traversal • Search Algorithms • Version Control

🔵 S S Naveen

<p align="center">
  <a href="https://github.com/Naveen77qwerty">
    <img src="https://github.com/Naveen77qwerty.png" width="150px" style="border-radius: 50%;" alt="S S Naveen"/>
  </a>
</p>
<p align="center">
  <a href="https://github.com/Naveen77qwerty">@Naveen77qwerty</a>
</p>
Naveen focused on implementing the fundamental Red-Black Tree operations and the core Limit Order Book infrastructure that forms the backbone of this trading system. Enhanced the Project contributions section of README file
Key Contributions:

* Implemented rotation operations (rotateLeft() and rotateRight()) essential for tree rebalancing ([commit 0fc5b92](https://github.com/arhamgarg/LimitOrderBook/commit/0fc5b92a814e43a1bb200a658f0efb3b7625487d))
* Designed and built the core LimitOrderBook class with basic order management functionality ([commit 5e5b62d](https://github.com/arhamgarg/LimitOrderBook/commit/5e5b62df887f7fd94def670a79d11558533f94e9))
* Enhanced Contributors section with detailed profiles and contribution history ([commit 4d372e1](https://github.com/Naveen77qwerty/LimitOrderBook/commit/4d372e18817f40fcb1a7ecc61cd8a2ccdce8d8fb))

Technical Focus: Red-Black Trees • Rotation Algorithms • Tree Rebalancing • System Architecture

🟢 A Adithyan

<p align="center">
  <a href="https://github.com/Cirutuu">
    <img src="https://github.com/Cirutuu.png" width="150px" style="border-radius: 50%;" alt="A Adithyan"/>
  </a>
</p>
<p align="center">
  <a href="https://github.com/Cirutuu">@Cirutuu</a>
</p>
Adithyan specialized in the critical tree balancing algorithms and the order matching engine, implementing the complex logic that ensures data structure integrity and efficient trade execution.
Key Contributions:

* Implemented the Red-Black Tree fixInsert() method for maintaining tree balance after insertions ([commit 09a1f7b](https://github.com/arhamgarg/LimitOrderBook/commit/09a1f7b1b8c65ad7813e077e65a9c3a5268095e0))
* Developed the order matching algorithm that executes trades between bids and asks ([commit 975f12f](https://github.com/arhamgarg/LimitOrderBook/commit/975f12fae5eb6175450a81c7f83ac5ca480001fa))

Technical Focus: Fix-up Operations • Order Matching • Trade Execution • Data Visualization

🔴 H Dharshan

<p align="center">
  <a href="https://github.com/Dharshan2208">
    <img src="https://github.com/Dharshan2208.png" width="150px" style="border-radius: 50%;" alt="H Dharshan"/>
  </a>
</p>
<p align="center">
  <a href="https://github.com/Dharshan2208">@Dharshan2208</a>
</p>
Dharshan handled critical implementation details and created comprehensive documentation that makes this complex project accessible and understandable.
Key Contributions:

* Implemented the tree constructor and insert method with duplicate price handling ([commit ed4d26d](https://github.com/arhamgarg/LimitOrderBook/commit/ed4d26df1686437fee813a10805069730d12ea35))
* Developed the main driver program demonstrating complete LOB functionality
* Authored comprehensive README documentation with detailed algorithm explanations ([commit 0aa24d4](https://github.com/arhamgarg/LimitOrderBook/commit/0aa24d4c3a3496e5981539d2c457e11701f8eb71))
* Comprehensive README and algorithm documentation ([commit cd60a28](https://github.com/arhamgarg/LimitOrderBook/commit/cd60a288966b2b32d0b4349b8d1eaf2db82a3de6))

Technical Focus: Tree Insertion • Technical Writing • Code Examples • Algorithm Analysis
