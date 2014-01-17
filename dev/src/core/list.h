// Copyright (c) 2014 Jiho Choi. All rights reserved.
// To use this source, see LICENSE file.

#pragma once

namespace dg {

template<class T>
class List {
private:
  struct ListNode {
    ListNode () : prev_(NULL), next_(NULL) {}
    ~ListNode () {
      if (prev_) {
        prev_->next_ = next_;
      }
      if (next_) {
        next_->prev_ = prev_;
      }
    }
    // Insert back
    // Next_ => new_node->next_
    void Insert(ListNode * new_node) {
      Check(new_node);
      if (next_) {
        next_->prev_ = new_node;
        new_node->next_ = next_;
      }
      next_ = new_node;
      new_node->prev_ = this;
    }
    ListNode* prev_;
    ListNode* next_;
    T data_;
  };

public:
  class Iterator {
    friend class List;
  public:
    Iterator() : current_(NULL) {}
    Iterator(ListNode * current) : current_(current) {}
    Iterator(const Iterator& rhs) : current_(rhs.current_) {}
    ~Iterator() {}
    Iterator& operator=(const Iterator& rhs) {
      if (this == &rhs) {
        return *this;
      }
      current_ = rhs.current_;
      return *this;
    }
    // Pre-increment
    Iterator& operator++() {
      if (current_) current_ = current_->next_;
      return *this;
    }
    // Post-increment
    Iterator operator++(int) {
      Iterator saved = *this;
      if (current_) current_ = current_->next_;
      return saved;
    }
    // Pre-decrement
    Iterator& operator--() {
      if (current_) current_ = current_->prev_;
      return *this;
    }
    // Post-decrement
    Iterator operator--(int) {
      Iterator saved = *this;
      if (current_) current_ = current_->prev_;
      return saved;
    }
    bool operator!=(Iterator& it) {
      return (it.current_ != current_);
    }
    bool operator==(Iterator& it) {
      return (it.current_ == current_);
    }
    T& operator*() {
      Check(current_);
      return current_->data_;
    }

  private:
    ListNode* current_;
  };

private:
  // From_node->next_ will be from_node->new_node->next_
  ListNode* Insert(ListNode* from_node, const T& data) {
    // Builds content
    ListNode* new_node = new ListNode;
    new_node->data_ = data;
    ++count_;
    // If it's the first
    if (from_node == NULL) {
      // It becomes the head_ node
      head_ = tail_ = new_node;
      return new_node;
    }
    from_node->Insert(new_node);
    if (tail_ == from_node) {
      tail_ = new_node;
    }
    return new_node;
  }
  void Init() {
    head_ = tail_ = NULL;
    count_ = 0;
  }

public:
  List() {
    Init();
  }
  List(const List<T>& rhs) {
    Init();
    CopyFrom(rhs);
  }
  ~List() {
    Clear();
  }
  List<T>& operator =(const List<T>& rhs) {
    return CopyFrom(rhs);
  }
  List<T>& CopyFrom(const List<T>& rhs) {
    if (&rhs == this) {
      return *this;
    }
    Clear();
    Iterator it = rhs.Begin();
    Iterator ie = rhs.End();
    while (it != ie) {
      PushBack(*it);
      it++;
    }
    return *this;
  }
  void Erase(Iterator it) {
    ListNode* removed = it.current_;
    if (!removed) return;
    if (head_ == removed) head_ = removed->next_;
    if (tail_ == removed) tail_ = removed->prev_;
    delete (removed);
    --count_;
  }
  void Clear() {
    ListNode* current = head_;
    while (current) {
      ListNode* next = current->next_;
      delete (current);
      --count_;
      current = next;
    }
    Init();
  }
  int Count() const {
    return count_;
  }
  Iterator PushBack(const T& data) {
    return Iterator(Insert(tail_, data));
  }
  T Get(int index) {
    Check(index < Count());
    return *GetIterator(index);
  }
  Iterator GetIterator(int index) {
    Iterator it = Begin();
    Iterator ie = End();
    while (index && (it != ie)) {
      --index;
      ++it;
    }
    if (index == 0) return it;
    return ie;
  }
  Iterator Begin() {
    return Iterator(head_);
  }
  Iterator Begin() const {
    return Iterator(head_);
  }
  Iterator End() {
    return Iterator(NULL);
  }
  Iterator End() const {
    return Iterator(NULL);
  }
  Iterator Find(T data) {
    Iterator it = Begin(), it_end = End();
    for (; it != it_end; ++it) {
      if (data == *it) return it;
    }
    return End();
  }
  bool IsEmpty() const {
    return (count_ == 0);
  }
  T PopFront() {
    Iterator it = Begin();
    T Item = *it;
    Erase(it);
    return Item;
  }

private:
  ListNode* head_;
  ListNode* tail_;
  int count_;
};

} // namespace dg
