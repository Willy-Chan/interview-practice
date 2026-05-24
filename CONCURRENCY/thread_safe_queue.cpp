#include <memory>
#include <mutex>

class TwoLockQueue {
private:

    // Node == element we are storing. We keep a linked list of nodes. Point to the net node.
    struct Node {
        int value;
        Node* next;
        Node(int val) : value(val), next(nullptr) {}
    };

    Node* head;                 // Managed by head_mtx
    Node* tail;                 // Managed by tail_mtx
    
    std::mutex head_mtx;
    std::mutex tail_mtx;

public:
    TwoLockQueue() {
        // The Trick: Initialize with a dummy node so head and tail are separated
        Node* dummy = new Node(0);
        head = dummy;
        tail = dummy;
    }
    ~TwoLockQueue() {
        // walk list and delete everything
    }

    // --- PUSH (Only grabs tail_mtx) ---
    void push(int new_value) {
        Node* new_node = new Node(new_value);

        {
            std::unique_lock<std::mutex> lock(tail_mtx);
            tail->next = new_node;
            tail = new_node;
        } 
    }

    // --- POP (Only grabs head_mtx) ---
    bool pop(int& value) {
        std::unique_lock<std::mutex> lock(head_mtx);
        if (head->next == nullptr)
            return false;

        Node* old_head = head;
        Node* real_front = head->next;
        value = real_front->value;
        head = real_front;
        delete old_head;   // free the old dummy / old front node
        return true;
    }
};