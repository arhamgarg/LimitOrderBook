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
