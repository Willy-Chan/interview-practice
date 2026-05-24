class ArrayHeapAllocator {
private:
    struct Block {
        size_t size;     // Size of usable data area
        bool is_free;    // Availability flag
        Block* next;     // Next block pointer
    };

    static constexpr size_t POOL_SIZE = 1024;
    char memory_pool[POOL_SIZE];
    Block* free_list_head;

    // Rounds up to pointer alignment (typically 8 bytes)
    size_t align(size_t size) {
        return (size + sizeof(void*) - 1) & ~(sizeof(void*) - 1);
    }

    void coalesce() {
        Block* current = free_list_head;

        // Merge consecutive free blocks (list is kept in memory order)
        while (current != nullptr && current->next != nullptr) {
            if (current->is_free && current->next->is_free) {
                current->size += sizeof(Block) + current->next->size;
                current->next = current->next->next;
            } else {
                current = current->next;
            }
        }
    }

public:
    ArrayHeapAllocator() {
        free_list_head = reinterpret_cast<Block*>(memory_pool);
        free_list_head->size = POOL_SIZE - sizeof(Block);
        free_list_head->is_free = true;
        free_list_head->next = nullptr;
    }

    void* alloc(size_t size) {
        if (size == 0) return nullptr;

        size = align(size);
        Block* current = free_list_head;

        while (current != nullptr) {
            if (current->is_free && current->size >= size) {
                // Split if enough room remains for another block header + alignment slack
                if (current->size >= size + sizeof(Block) + sizeof(void*)) {
                    char* current_ptr = reinterpret_cast<char*>(current);
                    Block* new_block = reinterpret_cast<Block*>(
                        current_ptr + sizeof(Block) + size);

                    new_block->size = current->size - size - sizeof(Block);
                    new_block->is_free = true;
                    new_block->next = current->next;

                    current->size = size;
                    current->next = new_block;
                }

                current->is_free = false;
                return reinterpret_cast<void*>(
                    reinterpret_cast<char*>(current) + sizeof(Block));
            }
            current = current->next;
        }

        std::cout << "[ALLOC ERROR] Out of memory!\n";
        return nullptr;
    }

    void free(void* ptr) {
        if (!ptr) return;

        Block* block = reinterpret_cast<Block*>(
            reinterpret_cast<char*>(ptr) - sizeof(Block));
        block->is_free = true;
        coalesce();
    }
};

int main() {
    ArrayHeapAllocator allocator;

    void* a = allocator.alloc(64);
    void* b = allocator.alloc(128);
    void* c = allocator.alloc(32);

    allocator.free(b);
    allocator.free(a);
    allocator.free(c);

    void* d = allocator.alloc(200);
  (void)d;
    return 0;
}
