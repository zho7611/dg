// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

class Chunk;

// Red-black binary search tree
template<class KeyType, class ValueType>
class TreeMap {
private:
  enum NodeColor {
      NodeColor_Black,
      NodeColor_Red,
  };
  struct Node {
    Node* left;
    Node* right;
    Node* parent_;
    NodeColor color_;
    KeyType key_;
    ValueType value_;

    Node() :
      left(NULL), right(NULL), parent_(NULL),
      color_(NodeColor_Red) {}

    ~Node() {}
  };

public:
  class Iterator {
  public:
    friend class TreeMap;
    Iterator() : current_(NULL), Reserve_(false) {}
    ~Iterator() {}
    // Pre-increment
    Iterator& operator++() {
      if (Reserve_) Decrement();
      else Increment();
      return *this;
    }
    // Post-increment
    Iterator operator++(int) {
      Iterator Saved = *this;
      if (Reserve_) Decrement();
      else Increment();
      return Saved;
    }
    // Pre-decrement
    Iterator& operator--() {
      if (Reserve_) Increment();
      else Decrement();
      return *this;
    }
    // Post-decrement
    Iterator operator--(int) {
      Iterator Saved = *this;
      if (Reserve_) Increment();
      else Decrement();
      return Saved;
    }
    bool operator!=(const Iterator& it) {
      return (it.current_ != current_);
    }
    bool operator==(const Iterator& it) {
      return (it.current_ == current_);
    }
    KeyType& Key() {
      Check(current_);
      return current_->key_;
    }
    ValueType& Value() {
      Check(current_);
      return current_->value_;
    }

  private:
    Node* current_;
    bool Reserve_;

    void Increment() {
      if (current_->right) {
        // Has the right node
        current_ = current_->right;
        while (current_->left) {
          current_ = current_->left;
        }
      } else {
        // Doesn't have the right node
        Node* currentParent = current_->parent_;
        while (currentParent && (current_ == currentParent->right)) {
          current_ = currentParent;
          currentParent = currentParent->parent_;
        }
        current_ = currentParent;
      }
    }
    void Decrement() {
      // Empty!
      if (!current_) return;
      if (current_->left) {
        current_ = current_->left;
        while (current_->right) {
          current_ = current_->right;
        }
      } else {
        Node* currentParent = current_->parent_;
        while (currentParent && (current_ == currentParent->left)) {
          current_ = currentParent;
          currentParent = currentParent->parent_;
        }
        current_ = currentParent;
      }
    }
  };

  TreeMap();
  TreeMap(const TreeMap<KeyType, ValueType>& rhs);
  ~TreeMap();
  int Count()const;
  void Clear();
  bool Erase(Iterator it);
  bool Remove(const KeyType& key);
  bool IsEmpty() const;
  ValueType& Get(const KeyType& key);
  const ValueType& GetConst(const KeyType& key) const;
  bool IsFound(const KeyType& key) const;
  Iterator Begin() const;
  Iterator ReverseBegin() const;
  Iterator End() const;
  Iterator ReverseEnd() const;
  Iterator UpperBound(const KeyType& key) const;
  Iterator LowerBound(const KeyType& key) const;
  Iterator Insert(const KeyType& key, const ValueType& value);
  Iterator Find(const KeyType& key) const;
  void CopyFrom(const TreeMap<KeyType, ValueType>& rhs);
  TreeMap<KeyType, ValueType>& operator=(
      const TreeMap<KeyType, ValueType>& rhs) {
    CopyFrom(rhs);
    return *this;
  }

private:
  Node* root_;
  Node* head_;
  Node* tail_;
  int count_;

  // Just for test
  bool IsLeaf(Node* x) const;
  int CountDepth(Node* x) const;
  // Rotate node x to left
  void RotateLeft(Node* x);
  // Rotate node x to right
  void RotateRight(Node* x);
  // Maintain NodeColor_Red-NodeColor_Black tree balance after inserting node x
  void InsertFixup(Node* x);
  // Allocate node for data and Insert in tree
  Node* InsertNode(const KeyType& key, const ValueType& value);
  // Maintain NodeColor_Red-NodeColor_Black tree balance after deleting node x
  void DeleteFixup(Node*x, Node* parentX);
  // Delete the node from tree
  bool DeleteNode(Node* node);
  Iterator FindNode(const KeyType& key) const;
  bool KeyCompare(const KeyType& lhs, const KeyType& rhs) const;
};

template <class KeyType, class ValueType>
bool TreeMap<KeyType, ValueType>::IsLeaf(Node* x) const {
  if (!x) {
    return false;
  }
  if (!x->left && !x->right) {
    return true;
  }
  return false;
}

template <class KeyType, class ValueType>
int TreeMap<KeyType, ValueType>::CountDepth(Node* x) const {
  if (!x) {
    return 0;
  }
  int count = CountDepth(x->parent_);
  return (x->color_ == NodeColor_Black) ? ++count : count;
}

template <class KeyType, class ValueType>
void TreeMap<KeyType, ValueType>::RotateLeft(Node* x) {
  Node* y = x->right;
  Check(y);
  // Establish x->right link
  x->right = y->left;
  if (y->left) {
    y->left->parent_ = x;
  }
  // Y's new Parent was x's Parent
  y->parent_ = x->parent_;
  // Set the Parent to point to y instead of x
  // First see whether we're at the root
  if (x->parent_ == NULL) {
    root_ = y;
  } else if (x == (x->parent_)->left) {
    // X was on the left of its Parent
    x->parent_->left = y;
  } else {
    // X must have been on the right
    x->parent_->right = y;
  }
  // Finally, put x on y's left
  y->left = x;
  x->parent_ = y;
}

template <class KeyType, class ValueType>
void TreeMap<KeyType, ValueType>::RotateRight(Node* x) {
  Node* y = x->left;
  Check(y);
  // Establish x->left link
  x->left = y->right;
  if (y->right) {
    y->right->parent_ = x;
  }
  // Y's new Parent was x's Parent
  y->parent_ = x->parent_;
  // Set the Parent to point to y instead of x
  // First see whether we're at the root
  if (x->parent_ == NULL) {
    root_ = y;
  } else if ((x == (x->parent_)->right)) {
    // X was on the right of its Parent
    x->parent_->right = y;
  } else {
    // X must have been on the left
    x->parent_->left = y;
  }
  // Finally, put x on y's left
  y->right = x;
  x->parent_ = y;
}

template <class KeyType, class ValueType>
void TreeMap<KeyType, ValueType>::InsertFixup(Node* x) {
  // Check NodeColor_Red-NodeColor_Black properties
  while (x->parent_ && x->parent_->color_ == NodeColor_Red) {
    // We have a violation
    if (x->parent_ == x->parent_->parent_->left) {
      Node* y = x->parent_->parent_->right;
      if (y && (y->color_ == NodeColor_Red)) {
        // Uncle is RED
        x->parent_->color_ = NodeColor_Black;
        y->color_ = NodeColor_Black;
        x->parent_->parent_->color_ = NodeColor_Red;
        x = x->parent_->parent_;
      } else {
        // Uncle is BLACK
        if (x == x->parent_->right) {
          // Make x a left child
          x = x->parent_;
          RotateLeft(x);
        }
        // Recolor and rotate
        x->parent_->color_ = NodeColor_Black;
        x->parent_->parent_->color_ = NodeColor_Red;
        RotateRight(x->parent_->parent_);
      }
    } else {
      // Mirror image of above code
      Node* y = x->parent_->parent_->left;
      if (y && (y->color_ == NodeColor_Red)) {
        // Uncle is RED
        x->parent_->color_ = NodeColor_Black;
        y->color_ = NodeColor_Black;
        x->parent_->parent_->color_ = NodeColor_Red;
        x = x->parent_->parent_;
      } else {
        // Uncle is BLACK
        if (x == x->parent_->left) {
          x = x->parent_;
          RotateRight(x);
        }
        x->parent_->color_ = NodeColor_Black;
        x->parent_->parent_->color_ = NodeColor_Red;
        RotateLeft(x->parent_->parent_);
      }
    }
  }
  root_->color_ = NodeColor_Black;
}

template <class KeyType, class ValueType>
typename TreeMap<KeyType, ValueType>::Node* TreeMap<KeyType, ValueType>::InsertNode(
    const KeyType& key, const ValueType& value) {
  Check(Find(key) == End());
  if (Find(key) != End()) {
    return NULL;
  }
  // Find where node belongs
  Node* current = root_;
  Node* parent = NULL;
  while (current != NULL) {
    parent = current;
    current = KeyCompare(key, current->key_) ?
        current->left : current->right;
  }
  // Setup new node
  Node* new_node = new Node;
  new_node->key_ = key;
  new_node->value_ = value;
  new_node->parent_ = parent;
  new_node->left = new_node->right = NULL;
  new_node->color_ = NodeColor_Red;
  // Insert node in tree
  if (parent) {
    if (KeyCompare(key, parent->key_)) {
      parent->left = new_node;
    } else {
      parent->right = new_node;
    }
  } else {
    root_ = new_node;
  }
  // Head/Tail setting
  // If ( new_node < Head )
  if (!head_ || KeyCompare(key, head_->key_)) {
    head_ = new_node;
  }
  // If ( new_node > Tail )
  if (!tail_ || KeyCompare(tail_->key_, key)) {
    tail_ = new_node;
  }
  InsertFixup(new_node);
  ++count_;
  return(new_node);
}

template <class KeyType, class ValueType>
void TreeMap<KeyType, ValueType>::DeleteFixup(Node*x, Node* parentX) {
  if (!x) {
    return;
  }
  while (x != root_ && (!x || x->color_ == NodeColor_Black)) {
    if (x == parentX->left) {
      Node* parentRight = parentX->right;
      if (parentRight && parentRight->color_ == NodeColor_Red) {
        parentRight->color_ = NodeColor_Black;
        parentX->color_ = NodeColor_Red;
        RotateLeft(parentX);
        parentRight = parentX->right;
      }
      if (parentRight == NULL) {
        x = parentX;
        parentX = parentX->parent_;
      } else if (((!parentRight->left || parentRight->left->color_ == NodeColor_Black)
              && (!parentRight->right || parentRight->right->color_ == NodeColor_Black))) {
        parentRight->color_ = NodeColor_Red;
        x = parentX;
        parentX = parentX->parent_;
      } else {
        if (!parentRight->right || parentRight->right->color_ == NodeColor_Black) {
          if (parentRight->left) {
            parentRight->left->color_ = NodeColor_Black;
          }
          parentRight->color_ = NodeColor_Red;
          RotateRight(parentRight);
          parentRight = parentX->right;
        }
        parentRight->color_ = parentX->color_;
        parentX->color_ = NodeColor_Black;
        if (parentRight->right) {
          parentRight->right->color_ = NodeColor_Black;
        }
        RotateLeft(parentX);
        break; // X = root_;
      }
    } else {
      Node* parentLeft = parentX->left;
      if (parentLeft && parentLeft->color_ == NodeColor_Red) {
        parentLeft->color_ = NodeColor_Black;
        parentX->color_ = NodeColor_Red;
        RotateRight(parentX);
        parentLeft = parentX->left;
      }
      if (parentLeft == NULL) {
        x = parentX;
        parentX = parentX->parent_;
      } else if ((!parentLeft->right || parentLeft->right->color_ == NodeColor_Black)
          && (!parentLeft->left || parentLeft->left->color_ == NodeColor_Black)) {
        parentLeft->color_ = NodeColor_Red;
        x = parentX;
        parentX = parentX->parent_;
      } else {
        if (!parentLeft->left || parentLeft->left->color_ == NodeColor_Black) {
          if (parentLeft->right) {
            parentLeft->right->color_ = NodeColor_Black;
          }
          parentLeft->color_ = NodeColor_Red;
          RotateLeft(parentLeft);
          parentLeft = parentX->left;
        }
        parentLeft->color_ = parentX->color_;
        parentX->color_ = NodeColor_Black;
        if (parentLeft->left) {
          parentLeft->left->color_ = NodeColor_Black;
        }
        RotateRight(parentX);
        break; // X = root_;
      }
    }
  }
  if (x) {
    x->color_ = NodeColor_Black;
  }
}

template <class KeyType, class ValueType>
bool TreeMap<KeyType, ValueType>::DeleteNode(Node* node) {
  Node* y = node;
  Node* x = 0;
  Node* parent_x = 0;
  if (!node) {
    return false;
  }
  // Set y
  if (node->left == NULL || node->right == NULL) {
    // Y has a NULL node as a child
    y = node;
  } else {
    // Find tree successor with a NULL node as a child
    Check((node != head_) && (node != tail_));
    y = node->right;
    while (y->left != NULL) {
      y = y->left;
    }
  }
  // Set x
  // X is y's only child
  if (y->left != NULL) {
    x = y->left;
  } else {
    x = y->right;
  }
  if (y == node) {
    // Head/Tail setting
    if (head_ == node) {
      Check(!node->left);
      if (node->right) {
        Node* LeftMost = node->right;
        while (LeftMost->left) {
          LeftMost = LeftMost->left;
        }
        head_ = LeftMost;
      } else {
        head_ = node->parent_;
        Check(!node->parent_ || (node->parent_->left == node));
      }
    }
    if (tail_ == node) {
      Check(!node->right);
      if (node->left) {
        Node* RightMost = node->left;
        while (RightMost->right) {
          RightMost = RightMost->right;
        }
        tail_ = RightMost;
      } else {
        tail_ = node->parent_;
        Check(!node->parent_ || (node->parent_->right == node));
      }
    }
  }
  // Remove y from the Parent chain
  parent_x = y->parent_; // Saving for the case [x == NULL]
  if ((x)) {
    x->parent_ = y->parent_;
  }
  if (y->parent_) {
    if (y == y->parent_->left) {
      y->parent_->left = x;
    } else {
      y->parent_->right = x;
    }
  } else {
    root_ = x;
  }
  if (y != node) {
    node->key_ = y->key_;
    node->value_ = y->value_;
    if (y == tail_) {
      tail_ = node;
    }
    Check(y != head_);
  }
  if (y->color_ != NodeColor_Red) {
    DeleteFixup(x, parent_x);
  }
  --count_;
  Check(y != tail_);
  delete y;
  return true;
}

template <class KeyType, class ValueType>
bool TreeMap<KeyType, ValueType>::Erase(Iterator it) {
  return DeleteNode(it.current_);
}

template <class KeyType, class ValueType>
typename TreeMap<KeyType, ValueType>::Iterator TreeMap<KeyType, ValueType>::Insert(
    const KeyType& key, const ValueType& value) {
  Iterator it;
  it.current_ = InsertNode(key, value);
  return it;
}

template <class KeyType, class ValueType>
typename TreeMap<KeyType, ValueType>::Iterator TreeMap<KeyType, ValueType>::Find(
    const KeyType& key) const {
  return FindNode(key);
}

template <class KeyType, class ValueType>
bool TreeMap<KeyType, ValueType>::IsFound(const KeyType& key) const {
  return (FindNode(key) != End());
}

template <class KeyType, class ValueType>
bool TreeMap<KeyType, ValueType>::Remove(const KeyType& key) {
  Iterator it = Find(key);
  if (it == End()) {
    return false;
  }
  Erase(it);
  return true;
}

template <class KeyType, class ValueType>
int TreeMap<KeyType, ValueType>::Count() const {
  return count_;
}

template <class KeyType, class ValueType>
bool TreeMap<KeyType, ValueType>::IsEmpty() const {
  return (count_ == 0);
}

template <class KeyType, class ValueType>
typename TreeMap<KeyType, ValueType>::Iterator
TreeMap<KeyType, ValueType>::Begin() const {
  Iterator it;
  it.current_ = head_;
  it.Reserve_ = false;
  return it;
}

template <class KeyType, class ValueType>
typename TreeMap<KeyType, ValueType>::Iterator
TreeMap<KeyType, ValueType>::ReverseBegin() const {
  Iterator it;
  it.current_ = tail_;
  it.Reserve_ = true;
  return it;
}

template <class KeyType, class ValueType>
typename TreeMap<KeyType, ValueType>::Iterator
TreeMap<KeyType, ValueType>::End() const {
  Iterator it;
  it.current_ = NULL;
  it.Reserve_ = false;
  return it;
}

template <class KeyType, class ValueType>
typename TreeMap<KeyType, ValueType>::Iterator
TreeMap<KeyType, ValueType>::ReverseEnd() const {
  Iterator it;
  it.current_ = NULL;
  it.Reserve_ = true;
  return it;
}

template <class KeyType, class ValueType>
void TreeMap<KeyType, ValueType>::Clear() {
  Iterator it = Begin();
  while (it != End()) {
    Erase(it);
    it = Begin();
  }
  Check(count_ == 0);
  Check(!tail_ && !head_);
}

template <class KeyType, class ValueType>
typename TreeMap<KeyType, ValueType>::Iterator
TreeMap<KeyType, ValueType>::UpperBound(const KeyType& key) const {
  Node* y = NULL; // The last node which is greater than the key
  Node* x = root_; // Current node
  while (x) {
    if (KeyCompare(key, x->key_)) { // X->Key > key
      y = x, x = x->left;
    } else {
      x = x->right;
    }
  }
  Iterator it;
  it.current_ = y;
  return it;
}

template <class KeyType, class ValueType>
typename TreeMap<KeyType, ValueType>::Iterator
TreeMap<KeyType, ValueType>::LowerBound(const KeyType& key) const {
  Node* y = NULL; // The last node which is not less than the key
  Node* x = root_; // Current node
  while (x) {
    // !(x->Key < k) == x->Key >= k
    if (!KeyCompare(x->key_, key)) {
      y = x, x = x->left;
    } else {
      x = x->right;
    }
  }
  Iterator it;
  it.current_ = y;
  return it;
}

template <class KeyType, class ValueType>
TreeMap<KeyType, ValueType>::TreeMap() :
  root_(NULL), head_(NULL), tail_(NULL), count_(0) {
}

template <class KeyType, class ValueType>
TreeMap<KeyType, ValueType>::TreeMap(const TreeMap<KeyType, ValueType>& rhs) :
  root_(NULL), head_(NULL), tail_(NULL), count_(0) {
  CopyFrom(rhs);
}

template <class KeyType, class ValueType>
TreeMap<KeyType, ValueType>::~TreeMap() {
  Clear();
}

template <class KeyType, class ValueType>
typename TreeMap<KeyType, ValueType>::Iterator
TreeMap<KeyType, ValueType>::FindNode(const KeyType& key) const {
  Iterator it = LowerBound(key);
  return ((it == End()) || KeyCompare(key, it.current_->key_)) ? End() : it;
}

template <class KeyType, class ValueType>
bool TreeMap<KeyType, ValueType>::KeyCompare(
    const KeyType& lhs, const KeyType& rhs) const {
  return lhs < rhs;
}

template <class KeyType, class ValueType>
ValueType& TreeMap<KeyType, ValueType>::Get(const KeyType& key) {
  Iterator it = FindNode(key);
  if (it == End()) {
    ValueType temp;
    return Insert(key, temp).Value();
  }
  return it.Value();
}

template <class KeyType, class ValueType>
const ValueType& TreeMap<KeyType, ValueType>::GetConst(const KeyType& key) const {
  Iterator it = FindNode(key);
  Check(it != End());
  return it.Value();
}

template<class KeyType, class ValueType>
void TreeMap<KeyType, ValueType>::CopyFrom(const TreeMap<KeyType, ValueType>& rhs) {
  Check(this->IsEmpty());
  Check(this != &rhs);
  if (this == &rhs) {
    return;
  }
  Iterator it = rhs.Begin();
  Iterator end = rhs.End();
  for (; it != end; ++it) {
    Insert(it.Key(), it.Value());
  }
}

} // namespace dg
