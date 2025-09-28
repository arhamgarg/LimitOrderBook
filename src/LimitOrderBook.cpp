enum Color { RED, BLACK };

class Node {
public:
  double price;
  int quantity;
  Color color;
  Node *left, *right, *parent;

  Node(double price, int quantity) {
    this->price = price;
    this->quantity = quantity;
    color = RED;
    left = right = parent = nullptr;
  }
};

class RedBlackTree {
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
};