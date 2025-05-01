#include <iostream>
#include <stdexcept>

template <typename T>
class LinkedList {
public:
    // ��������ڵ�
    struct Node {
        T data;
        Node* next;

        Node(const T& data, Node* next = nullptr) : data(data), next(next) {}
    };

    // ���������
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

        // �ṩһ��������������ȡ��ǰ�ڵ��ָ��
        Node* getNode() const {
            return current;
        }
    };

private:
    Node* head; // �����ͷ�ڵ�

public:
    // ���캯��
    LinkedList() : head(nullptr) {}

    // ��������
    ~LinkedList() {
        clear();
    }

    // ��ֹ��������͸�ֵ
    LinkedList(const LinkedList&) = delete;
    LinkedList& operator=(const LinkedList&) = delete;

    // �������в�������
    void push_back(const T& value) {
        head = new Node(value, head);
    }

    // ��������
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

    // ɾ��ָ��������ָ��Ľڵ�
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

    // �������
    void clear() {
        Node* current = head;
        while (current != nullptr) {
            Node* next = current->next;
            delete current;
            current = next;
        }
        head = nullptr;
    }

    // ��ȡ������
    Iterator begin() const {
        return Iterator(head);
    }

    Iterator end() const {
        return Iterator(nullptr);
    }

    // ��������
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