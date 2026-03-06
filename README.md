# Limit Order Book simulation for High-Frequency Trading

This repository implements a simplified Limit Order Book (LOB) simulator using Red-Black Trees (RBTs) as the primary data structure. The simulator is written in C++ and demonstrates how a balanced binary search tree can maintain ordered price levels efficiently for matching buy (bid) and sell (ask) orders.

This repository is intended as a case study for understanding data structures (balanced BSTs) and their application in a latency-sensitive domain — financial market order matching.

## Why a Red-Black Tree?

A Limit Order Book must support:
- Fast insertion of price levels and orders.
- Fast lookup of best bid (maximum price) and best ask (minimum price).
- Efficient deletion/rebalancing when price levels are removed.

Red-Black Trees provide O(log n) average and worst-case time complexities for insertions, deletions, and lookups while remaining balanced. This makes RBTs a suitable choice for an educational LOB: they keep the tree height O(log n) and ensure predictable operation times.

> Real HFT systems often use specialised data structures (e.g., skip lists, binary heaps combined with hash maps, or custom flat arrays) and kernel/OS optimisations for performance.

## Repository Contents

- `LimitOrderBook.cpp` — single-file C++ implementation (contains `Node`, `RedBlackTree`, `LimitOrderBook`, and `main`).

## High-level design

There are three main classes in the implementation:

1. **Node** — represents a single tree node (a price level) with fields:
   - `double price` — price level
   - `int quantity` — aggregated quantity at that price level
   - `string color` — "RED" or "BLACK" used by Red-Black Tree algorithms
   - `Node *left, *right, *parent` — pointers for tree structure

2. **RedBlackTree** — balanced BST that stores `Node` objects ordered by `price`.
   - Provides standard RBT operations: `insert`, `deleteNode`, `search`, rotations (`rotateLeft`, `rotateRight`), `fixInsert`, `fixDelete`, `transplant`, and search helpers (`minimum`, `maximum`, `successor`, `predecessor`).
   - Uses a `NIL` sentinel node to simplify boundary logic.

3. **LimitOrderBook** — two RBT instances:
   - `bids` (stores buy orders; highest price = best bid)
   - `asks` (stores sell orders; lowest price = best ask)
   - Methods: `addOrder(price, qty, isBuy)`, `getBestBid()`, `getBestAsk()`, `matchOrders()`, and `displayBook(depth)`.

## Function & Module Documentation

Below are the most important functions and their responsibilities.

- **Node**

  - Signature: Node(`double price`,`int quantity`)
  - Purpose: Create a new node representing a price level. Newly created nodes default to color RED (standard RBT insertion convention) and have null child/parent pointers.
  - Inputs: price (double), quantity (int)
  - Output: new Node pointer

- **RedBlackTree**

  **Note:** The tree stores nodes ordered by price. For bids, higher price is more important (maximum = best bid). For asks, lower price is better (minimum = best ask).

  1. **rotateLeft(`Node *x`) / rotateRight(`Node *x`)**
     - Purpose: Fundamental tree rebalancing primitives used by RBT algorithms.
     - Input: node x around which to rotate.
     - Output: Rewires pointers between x, x->right (or x->left), and their children/parents.
     - **Complexity:** O(1).

  2. **fixInsert(`Node *k`)**
     - Purpose: After inserting a RED node, restores Red-Black properties using recoloring and rotations.
     - Input: the newly inserted node k.
     - **Complexity:** O(log n) in worst case (while loop up the tree).

  3. **transplant(`Node *u`, `Node *v`)**
     - Purpose: Helper used by deletion to replace subtree rooted at u with subtree rooted at v.
     - **Complexity:** O(1).

  4. **fixDelete(`Node *k`)**
     - Purpose: After a physical deletion, restore RBT properties (color/structure) via rotations and recoloring.
     - **Complexity:** O(log n).

  5. **insert(`double price`, `int qty`)**
     - Purpose: Insert a new price level into the tree. If a node with the exact price already exists, the implementation adds the quantity to that node rather than creating a duplicate.
     - Inputs: price, qty
     - Output: modifies the tree (adds or updates a node).
     - **Complexity:** Search + insert is O(log n) to find insertion point; fixInsert may take O(log n).

  6. **Search/Traversal helpers: search(`double price`), minimum(), maximum(), successor(`Node *x`), predecessor(`Node *x`)**
     - Purpose: Standard BST utility functions used by the LOB to find best prices and iterate.
     - **Complexity:** search O(log n), minimum/maximum O(log n) worst-case (down to leaf), successor/predecessor O(log n) worst-case.

  7. **deleteNode(`double price`)**
     - Purpose: Delete `price` level from the tree and restore Red-Black properties with fixDelete.
     - Input: Price level to delete.
     - **Complexity:** O(log n) for deletion and balancing.

- **LimitOrderBook**

  1. **addOrder(double price, int qty, bool isBuy)**
     - Purpose: Add a new order to the book. If isBuy == true, it inserts into the bids tree; otherwise into asks.
     - Behaviour: Uses the tree's insert() which accumulates quantity at an existing price level if present.
     - **Complexity:** O(log n).

  2. **getBestBid() / getBestAsk()**
     - Purpose: Return the best bid (maximum price) and best ask (minimum price) respectively.
     - Return: Price or -1 if the tree is empty.
     - **Complexity:** O(log n) to find minimum/maximum.

  3. **matchOrders()**
     - Purpose: Execute naive crossing of orders while best bid price >= best ask price.

## Implemented Algorithm

The core matching process in the **Limit Order Book** works as follows:

```cpp
void matchOrders() {
  Node *bestBid = bids.maximum();
  Node *bestAsk = asks.minimum();

  while (bestBid != bids.getNil() && bestAsk != asks.getNil() &&
         bestBid->price >= bestAsk->price) {
    int tradedQty = min(bestBid->quantity, bestAsk->quantity);

    cout << "TRADE: " << tradedQty << " @ " << bestAsk->price << "\n";

    bestBid->quantity -= tradedQty;
    bestAsk->quantity -= tradedQty;

    double bidPrice = bestBid->price;
    double askPrice = bestAsk->price;

    if (bestBid->quantity == 0) {
      bids.deleteNode(bidPrice);
    }

    if (bestAsk->quantity == 0) {
      asks.deleteNode(askPrice);
    }

    bestBid = bids.maximum();
    bestAsk = asks.minimum();
  }
}
```

### Explanation:

1. **Initialisation:**
   - Best bid = maximum price in the bids tree.
   - Best ask = minimum price in the asks tree.

2. **Matching condition:**
   Orders match while `bestBid->price >= bestAsk->price`.

3. **Trade execution:**
   - `tradedQty = min(bestBid->quantity, bestAsk->quantity)`
   - Output trade at the ask price.

4. **Quantity update:**
   Subtract traded quantity from both price levels.

5. **Node Deletion and Re-query:**
   If a node's quantity becomes 0, that price level is deleted from the respective tree. The `bestBid` and `bestAsk` are then re-queried from the updated trees.

### Important implementation notes / limitations:

- The matching is price-time naive: this model aggregates at price levels and does not simulate order-by-order FIFO within the same price level.

- **Complexity:** Each loop iteration touches the best bid/ask. Number of iterations ≤ number of price levels consumed; each pointer movement or tree removal (if added) would be O(log n) per removal. Overall worst-case O(k log n) where k is number of matched price levels.

- **displayBook(int depth = 5)**
  - Purpose: Print top depth bids (descending) and asks (ascending) to the console.
  - **Complexity:** O(depth * log n) to find next predecessor/successor repeatedly.

## **Complexity** Summary

- **Insert (price level):** O(log n)
- **Search (by price):** O(log n)
- **Find best (minimum/maximum):** O(log n)
- **Delete (price level):** O(log n)
- **Match step (single trade between top-of-book levels):** O(1) for quantity arithmetic; moving to next level or deleting a level is O(log n).

**Space complexity:** O(n) nodes where n is number of distinct price levels stored.

## Example

**Input:**
- Bids: (100.5, 50), (101.0, 30), (99.5, 40)
- Asks: (102.0, 20), (103.5, 10), (100.0, 25)

**Output:**
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
102 : 20
103.5 : 10
```