#include <iostream>
#include <stdexcept>

template <typename T>
class LinkedList {
public:
    // 定义链表节点
    struct Node {
        T data;
        Node* next;

        Node(const T& data, Node* next = nullptr) : data(data), next(next) {}
    };

    // 定义迭代器
    class Iterator {
    private:
        Node* current;

    public:
        Iterator(Node* node) : current(node) {}

        T& operator*() const {
            return current->data;
        }

        T* operator->() const {
            return &current->data;
        }

        Iterator& operator++() {
            current = current->next;
            return *this;
        }

        Iterator operator++(int) {
            Iterator temp = *this;
            ++(*this);
            return temp;
        }

        bool operator==(const Iterator& other) const {
            return current == other.current;
        }

        bool operator!=(const Iterator& other) const {
            return current != other.current;
        }

        // 提供一个公共方法来获取当前节点的指针
        Node* getNode() const {
            return current;
        }
    };

private:
    Node* head; // 链表的头节点

public:
    // 构造函数
    LinkedList() : head(nullptr) {}

    // 析构函数
    ~LinkedList() {
        clear();
    }

    // 禁止拷贝构造和赋值
    LinkedList(const LinkedList&) = delete;
    LinkedList& operator=(const LinkedList&) = delete;

    // 向链表中插入数据
    void push_back(const T& value) {
        head = new Node(value, head);
    }

    // 查找数据
    Iterator find(const T& value) {
        Node* current = head;
        while (current != nullptr) {
            if (current->data == value) {
                return Iterator(current);
            }
            current = current->next;
        }
        return Iterator(nullptr);
    }

    // 删除指定迭代器指向的节点
    void erase(Iterator it) {
        if (it == Iterator(nullptr)) {
            throw std::invalid_argument("Iterator is invalid or out of range");
        }

        Node* to_delete = it.getNode();
        if (to_delete == head) {
            head = head->next;
        }
        else {
            Node* current = head;
            while (current != nullptr && current->next != to_delete) {
                current = current->next;
            }
            if (current == nullptr) {
                throw std::invalid_argument("Iterator does not point to a valid node");
            }
            current->next = to_delete->next;
        }
        delete to_delete;
    }

    // 清空链表
    void clear() {
        Node* current = head;
        while (current != nullptr) {
            Node* next = current->next;
            delete current;
            current = next;
        }
        head = nullptr;
    }

    // 获取迭代器
    Iterator begin() const {
        return Iterator(head);
    }

    Iterator end() const {
        return Iterator(nullptr);
    }

    // 遍历链表
    void print() const {
        Node* current = head;
        while (current != nullptr) {
            std::cout << current->data << " -> ";
            current = current->next;
        }
        std::cout << "nullptr" << std::endl;
    }

    bool empty() const {
        return head == nullptr;
    }
};