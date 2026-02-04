#pragma once

#include <cassert>
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <utility>
#include <algorithm>
#include <stdexcept>

template <typename Type>
class SingleLinkedList {
    struct Node {
        Node() : value(), next_node(nullptr) {}
        Node(const Type& val, Node* next)
            : value(val), next_node(next) {}

        Type value;
        Node* next_node = nullptr;
    };

public:
    class Iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = Type;
        using difference_type = std::ptrdiff_t;
        using pointer = Type*;
        using reference = Type&;

        Iterator() = default;
        explicit Iterator(Node* node) : node_(node) {}

        reference operator*() const {
			assert(node_ != nullptr && "Dereferencing end() iterator");
            return node_->value;
        }

        pointer operator->() const {
			assert(node_ != nullptr && "Dereferencing end() iterator");
            return &node_->value;
        }

        Iterator& operator++() {
			assert(node_ != nullptr && "Incrementing end() iterator");
            node_ = node_->next_node;
            return *this;
        }

        Iterator operator++(int) {
            Iterator tmp(*this);
            ++(*this);
            return tmp;
        }

        friend bool operator==(const Iterator& lhs, const Iterator& rhs) {
            return lhs.node_ == rhs.node_;
        }

        friend bool operator!=(const Iterator& lhs, const Iterator& rhs) {
            return !(lhs == rhs);
        }

        Node* GetNode() const {
            return node_;
        }

    private:
        Node* node_ = nullptr;
    };

    class ConstIterator {
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = const Type;
    using difference_type = std::ptrdiff_t;
    using pointer = const Type*;
    using reference = const Type&;

    ConstIterator() = default;
	
    explicit ConstIterator(const Node* node) : node_(node) {}
	
    ConstIterator(const Iterator& it) : node_(it.GetNode()) {}

    reference operator*() const {
		assert(node_ != nullptr && "Dereferencing end() iterator");
        return node_->value;
    }

    pointer operator->() const {
		assert(node_ != nullptr && "Dereferencing end() iterator");
        return &node_->value;
    }

    ConstIterator& operator++() {
		assert(node_ != nullptr && "Dereferencing end() iterator");
        node_ = node_->next_node;
        return *this;
    }

    ConstIterator operator++(int) {
        ConstIterator tmp(*this);
        ++(*this);
        return tmp;
    }

    friend bool operator==(const ConstIterator& lhs, const ConstIterator& rhs) {
        return lhs.node_ == rhs.node_;
    }

    friend bool operator!=(const ConstIterator& lhs, const ConstIterator& rhs) {
        return !(lhs == rhs);
    }

    const Node* GetNode() const {
        return node_;
    }

private:
    const Node* node_ = nullptr;
};

public:
    SingleLinkedList() noexcept = default;

    SingleLinkedList(std::initializer_list<Type> values) {
        Assign(std::begin(values), std::end(values));
    }

    SingleLinkedList(const SingleLinkedList& other) {
        Assign(other.begin(), other.end());
    }

    SingleLinkedList& operator=(const SingleLinkedList& rhs) {
        if (this != &rhs) {
            SingleLinkedList tmp(rhs);
            swap(tmp);
        }
        return *this;
    }

    ~SingleLinkedList() noexcept {
        Clear();
    }

    void PushFront(const Type& value) {
        head_.next_node = new Node(value, head_.next_node);
        ++size_;
    }

    void Clear() noexcept {
        Node* cur = head_.next_node;
		
        while (cur) {
            Node* next = cur->next_node;
            delete cur;
            cur = next;
        }
		
        head_.next_node = nullptr;
        size_ = 0;
    }

    void swap(SingleLinkedList& other) noexcept {
        std::swap(head_.next_node, other.head_.next_node);
        std::swap(size_, other.size_);
    }

    size_t GetSize() const noexcept {
        return size_;
    }

    bool IsEmpty() const noexcept {
        return size_ == 0;
    }

    Iterator begin() noexcept {
        return Iterator(head_.next_node);
    }

    Iterator end() noexcept {
        return Iterator(nullptr);
    }

    ConstIterator begin() const noexcept {
        return ConstIterator(head_.next_node);
    }

    ConstIterator end() const noexcept {
        return ConstIterator(nullptr);
    }

    ConstIterator cbegin() const noexcept {
        return begin();
    }

    ConstIterator cend() const noexcept {
        return end();
    }

    // Метод PopFront: удаляет первый элемент
    void PopFront() noexcept {
		assert(size_ > 0 && "PopFront on empty list");
        Node* temp = head_.next_node;
        head_.next_node = head_.next_node->next_node;
        delete temp;
        --size_;
    }

    // Метод InsertAfter: вставляет элемент после указанного итератора
	Iterator InsertAfter(ConstIterator pos, const Type& value) {
		Node* pos_node = const_cast<Node*>(pos.GetNode());
		assert(pos_node != nullptr && "InsertAfter at nullptr");
		Node* new_node = new Node(value, pos_node->next_node); // может бросить
		pos_node->next_node = new_node;
		++size_;
		return Iterator(new_node);
	}

	Iterator EraseAfter(ConstIterator pos) noexcept {
		Node* pos_node = const_cast<Node*>(pos.GetNode());
		assert(pos_node != nullptr && "EraseAfter at nullptr");
		Node* to_delete = pos_node->next_node;
		assert(to_delete != nullptr && "EraseAfter after end() iterator");
		pos_node->next_node = to_delete->next_node;
		delete to_delete;
		--size_;
		return Iterator(pos_node->next_node);
	}

    // Метод before_begin: возвращает итератор на фиктивную позицию перед первым элементом
    [[nodiscard]] Iterator before_begin() noexcept {
        return Iterator(&head_);
    }

    // Метод cbefore_begin: константный итератор на фиктивную позицию
    [[nodiscard]] ConstIterator cbefore_begin() const noexcept {
        return ConstIterator(&head_);
    }

    // Метод before_begin: константный итератор на фиктивную позицию
    [[nodiscard]] ConstIterator before_begin() const noexcept {
        return ConstIterator(&head_);
    }

private:
	template <typename It>
	void Assign(It first, It last) {
		Clear();
		Node* tail = &head_;
		try {
			for (auto it = first; it != last; ++it) {
				tail->next_node = new Node(*it, nullptr); // вставка в конец
				tail = tail->next_node;
				++size_;
			}
		} catch (...) {
			Clear();
			throw;
		}
	}

private:
    Node head_;
    size_t size_ = 0;
};

template <typename Type>
void swap(SingleLinkedList<Type>& lhs, SingleLinkedList<Type>& rhs) noexcept {
    lhs.swap(rhs);
}

template <typename Type>
bool operator==(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    if (lhs.GetSize() != rhs.GetSize()) {
        return false;
    }
    return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename Type>
bool operator!=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
bool operator<(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return std::lexicographical_compare(
        lhs.begin(), 
		lhs.end(),
        rhs.begin(), 
		rhs.end()
    );
}

template <typename Type>
bool operator<=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(rhs < lhs);
}

template <typename Type>
bool operator>(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return rhs < lhs;
}

template <typename Type>
bool operator>=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs < rhs);
}