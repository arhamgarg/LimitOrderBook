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

    if (x->parent == NIL) {
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

    if (x->parent == NIL) {
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

  void transplant(Node *u, Node *v) {
    if (u->parent == NIL) {
      root = v;
    } else if (u == u->parent->left) {
      u->parent->left = v;
    } else {
      u->parent->right = v;
    }

    v->parent = u->parent;
  }

  void fixDelete(Node *k) {
    while (k != root && k->color == "BLACK") {
      if (k == k->parent->left) {
        Node *w = k->parent->right;

        if (w->color == "RED") {
          w->color = "BLACK";
          k->parent->color = "RED";
          rotateLeft(k->parent);
          w = k->parent->right;
        }

        if (w->left->color == "BLACK" && w->right->color == "BLACK") {
          w->color = "RED";
          k = k->parent;
        } else {
          if (w->right->color == "BLACK") {
            w->left->color = "BLACK";
            w->color = "RED";
            rotateRight(w);
            w = k->parent->right;
          }

          w->color = k->parent->color;
          k->parent->color = "BLACK";
          w->right->color = "BLACK";
          rotateLeft(k->parent);
          k = root;
        }
      } else {
        Node *w = k->parent->left;

        if (w->color == "RED") {
          w->color = "BLACK";
          k->parent->color = "RED";
          rotateRight(k->parent);
          w = k->parent->left;
        }

        if (w->right->color == "BLACK" && w->left->color == "BLACK") {
          w->color = "RED";
          k = k->parent;
        } else {
          if (w->left->color == "BLACK") {
            w->right->color = "BLACK";
            w->color = "RED";
            rotateLeft(w);
            w = k->parent->left;
          }

          w->color = k->parent->color;
          k->parent->color = "BLACK";
          w->left->color = "BLACK";
          rotateRight(k->parent);
          k = root;
        }
      }
    }

    k->color = "BLACK";
  }

public:
  RedBlackTree() {
    NIL = new Node(0, 0);
    NIL->color = "BLACK";
    NIL->left = NIL->right = NIL->parent = NIL;
    root = NIL;
    root->parent = NIL;
  }

  void insert(double price, int quantity) {
    Node *newNode = new Node(price, quantity);

    newNode->left = NIL;
    newNode->right = NIL;

    Node *parent = NIL;
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

    if (parent == NIL) {
      root = newNode;
      newNode->parent = NIL;
    } else if (newNode->price < parent->price) {
      parent->left = newNode;
    } else {
      parent->right = newNode;
    }

    if (newNode->parent == NIL) {
      newNode->color = "BLACK";
      return;
    }

    if (newNode->parent->parent == NIL) {
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

  void deleteNode(double price) {
    Node *z = search(price);

    if (z == NIL) {
      return;
    }

    Node *y = z;
    Node *x;
    string yOriginalColor = y->color;

    if (z->left == NIL) {
      x = z->right;
      transplant(z, z->right);
    } else if (z->right == NIL) {
      x = z->left;
      transplant(z, z->left);
    } else {
      y = minimum(z->right);
      yOriginalColor = y->color;
      x = y->right;

      if (y->parent == z) {
        x->parent = y;
      } else {
        transplant(y, y->right);
        y->right = z->right;
        y->right->parent = y;
      }

      transplant(z, y);
      y->left = z->left;
      y->left->parent = y;
      y->color = z->color;
    }

    delete z;

    if (yOriginalColor == "BLACK") {
      fixDelete(x);
    }
  }

  Node *getNil() { return NIL; }
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

    return (n == bids.getNil()) ? -1 : n->price;
  }

  double getBestAsk() {
    Node *n = asks.minimum();

    return (n == asks.getNil()) ? -1 : n->price;
  }

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

  void displayBook(int depth = 5) {
    cout << "---- BIDS ----\n";

    Node *b = bids.maximum();

    for (int i = 0; b != bids.getNil() && i < depth; i++) {
      cout << b->price << " : " << b->quantity << "\n";
      b = bids.predecessor(b);
    }

    cout << "---- ASKS ----\n";

    Node *a = asks.minimum();

    for (int i = 0; a != asks.getNil() && i < depth; i++) {
      cout << a->price << " : " << a->quantity << "\n";
      a = asks.successor(a);
    }
  }
};

int main() {
  LimitOrderBook lob;

  lob.addOrder(100.5, 50, true);
  lob.addOrder(101.0, 30, true);
  lob.addOrder(99.5, 40, true);

  lob.addOrder(102.0, 20, false);
  lob.addOrder(103.5, 10, false);
  lob.addOrder(100.0, 25, false);

  lob.displayBook(3);

  cout << "Best Bid: " << lob.getBestBid() << "\n";
  cout << "Best Ask: " << lob.getBestAsk() << "\n";

  cout << "\n--- Matching Orders ---\n";
  lob.matchOrders();

  cout << "\n--- After Matching ---\n";
  lob.displayBook(3);
}
