#include <iostream>
using namespace std;

class Node {
public:
  double price;
  int quantity;
  string color;
  Node *left, *right, *parent;

  Node(double price, int quantity) {
    this->price = price;
    this->quantity = quantity;
    color = "RED";
    left = right = parent = nullptr;
  }
};

class RedBlackTree {
private:
  Node *root;
  Node *NIL;

  void rotateLeft(Node *x) {
    Node *y = x->right;

    x->right = y->left;

    if (y->left != NIL) {
      y->left->parent = x;
    }

    y->parent = x->parent;

    if (x->parent == nullptr) {
      root = y;
    } else if (x == x->parent->left) {
      x->parent->left = y;
    } else {
      x->parent->right = y;
    }

    y->left = x;
    x->parent = y;
  }

  void rotateRight(Node *x) {
    Node *y = x->left;

    x->left = y->right;

    if (y->right != NIL) {
      y->right->parent = x;
    }

    y->parent = x->parent;

    if (x->parent == nullptr) {
      root = y;
    } else if (x == x->parent->right) {
      x->parent->right = y;
    } else {
      x->parent->left = y;
    }

    y->right = x;
    x->parent = y;
  }

  void fixInsert(Node *k) {
    while (k != root && k->parent->color == "RED") {
      if (k->parent == k->parent->parent->left) {
        Node *u = k->parent->parent->right;

        if (u->color == "RED") {
          k->parent->color = "BLACK";
          u->color = "BLACK";
          k->parent->parent->color = "RED";
          k = k->parent->parent;
        } else {
          if (k == k->parent->right) {
            k = k->parent;
            rotateLeft(k);
          }

          k->parent->color = "BLACK";
          k->parent->parent->color = "RED";
          rotateRight(k->parent->parent);
        }
      } else {
        Node *u = k->parent->parent->left;

        if (u->color == "RED") {
          k->parent->color = "BLACK";
          u->color = "BLACK";
          k->parent->parent->color = "RED";
          k = k->parent->parent;
        } else {
          if (k == k->parent->left) {
            k = k->parent;
            rotateRight(k);
          }

          k->parent->color = "BLACK";
          k->parent->parent->color = "RED";
          rotateLeft(k->parent->parent);
        }
      }
    }

    root->color = "BLACK";
  }

public:
  RedBlackTree() {
    NIL = new Node(0, 0);
    NIL->color = "BLACK";
    NIL->left = NIL->right = NIL;
    root = NIL;
  }

  void insert(double price, int quantity) {
    Node *newNode = new Node(price, quantity);

    newNode->left = NIL;
    newNode->right = NIL;

    Node *parent = nullptr;
    Node *current = root;

    while (current != NIL) {
      parent = current;

      if (newNode->price < current->price) {
        current = current->left;
      } else if (newNode->price > current->price) {
        current = current->right;
      } else {
        current->quantity += quantity;
        delete newNode;
        return;
      }
    }

    newNode->parent = parent;

    if (parent == nullptr) {
      root = newNode;
    } else if (newNode->price < parent->price) {
      parent->left = newNode;
    } else {
      parent->right = newNode;
    }

    if (newNode->parent == nullptr) {
      newNode->color = "BLACK";
      return;
    }

    if (newNode->parent->parent == nullptr) {
      return;
    }

    fixInsert(newNode);
  }

  Node *search(double price) {
    Node *temp = root;

    while (temp != NIL) {
      if (price == temp->price) {
        return temp;
      } else if (price < temp->price) {
        temp = temp->left;
      } else {
        temp = temp->right;
      }
    }

    return NIL;
  }

  Node *minimum(Node *node = nullptr) {
    Node *temp = (node == nullptr) ? root : node;

    if (temp == NIL) {
      return NIL;
    }

    while (temp->left != NIL) {
      temp = temp->left;
    }

    return temp;
  }

  Node *maximum(Node *node = nullptr) {
    Node *temp = (node == nullptr) ? root : node;

    if (temp == NIL) {
      return NIL;
    }

    while (temp->right != NIL) {
      temp = temp->right;
    }

    return temp;
  }

  Node *predecessor(Node *x) {
    if (x == NIL) {
      return NIL;
    }

    if (x->left != NIL) {
      return maximum(x->left);
    }

    Node *p = x->parent;

    while (p != NIL && x == p->left) {
      x = p;
      p = p->parent;
    }

    return p;
  }

  Node *successor(Node *x) {
    if (x == NIL) {
      return NIL;
    }

    if (x->right != NIL) {
      return minimum(x->right);
    }

    Node *p = x->parent;

    while (p != NIL && x == p->right) {
      x = p;
      p = p->parent;
    }

    return p;
  }
};

class LimitOrderBook {
private:
  RedBlackTree bids;
  RedBlackTree asks;

public:
  void addOrder(double price, int qty, bool isBuy) {
    if (isBuy) {
      bids.insert(price, qty);
    } else {
      asks.insert(price, qty);
    }
  }

  double getBestBid() {
    Node *n = bids.maximum();

    return (n == nullptr || n->price == 0) ? -1 : n->price;
  }

  double getBestAsk() {
    Node *n = asks.minimum();

    return (n == nullptr || n->price == 0) ? -1 : n->price;
  }

  void matchOrders() {
    Node *bestBid = bids.maximum();
    Node *bestAsk = asks.minimum();

    while (bestBid && bestAsk && bestBid->price >= bestAsk->price) {
      int tradedQty = min(bestBid->quantity, bestAsk->quantity);

      cout << "TRADE: " << tradedQty << " @ " << bestAsk->price << "\n";

      bestBid->quantity -= tradedQty;
      bestAsk->quantity -= tradedQty;

      if (bestBid->quantity == 0) {
        bestBid = bids.predecessor(bestBid);
      }

      if (bestAsk->quantity == 0) {
        bestAsk = asks.successor(bestAsk);
      }
    }
  }

  void displayBook(int depth = 5) {
    cout << "---- BIDS ----\n";

    Node *b = bids.maximum();

    for (int i = 0; b && i < depth; i++) {
      cout << b->price << " : " << b->quantity << "\n";
      b = bids.predecessor(b);
    }

    cout << "---- ASKS ----\n";

    Node *a = asks.minimum();

    for (int i = 0; a && i < depth; i++) {
      cout << a->price << " : " << a->quantity << "\n";
      a = asks.successor(a);
    }
  }
};