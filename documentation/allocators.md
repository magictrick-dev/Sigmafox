*[Return to Index](../README.md)*

# Allocator API

Sigmafox implements a handful of allocators to help with managing datastructures. For the
most part, using the malloc/free wrapper is the most ideal allocator since other allocators
are potentially more wasteful outside of very specific contexts.

### Malloc/Free Wrapper Documentation

Both `malloc()` and `free()` are wrapped to help track memory allocations for diagnostic
purposes. Using this diagnostic information, it is possible to trivially detect memory
leaks by inspecting the `memory_alloc_stats` structured returned by `memory_statistics()`
function. When debug build is turned off, these wrapper functions call `malloc` and `free`
explicitly and the `memory_statistics` function always returns true.

<p align="center"><img src="./images/allocator_wrapper.svg" /></p>

When the user allocates using the wrapper, the region is extended to accomodate an
internal structure that describes the allocation size. This is referenced by `memory_free`
to accurately determine how many bytes are about to freed from the call. Additionally,
the internal structure adds a check-pointer to ensure that the "head" is valid.

-   ```C
    typedef struct
    memory_alloc_stats
    {
        size_t memory_allocated;
        size_t memory_freed;
        size_t alloc_calls;
        size_t free_calls;
    } memory_alloc_stats;
    ```

    The `memory_alloc_stats` structure is filled out by the `memory_statistics` function
    with details about the malloc/free wrapper. If `memory_allocated` and `memory_freed`
    are the same value, then there is potentially no memory leaks. Likewise, `alloc_calls`
    and `free_calls` should also be the same values.

-   ```C++
    void* memory_alloc(size_t size);
    ```

    The wrapper for `malloc()`; calls `malloc()` and performs the necessary work to write
    the allocation header.

    `memory_alloc_type(type)` is a macro that expands to: `(type*)memory_alloc(sizeof(type))`.
    `memory_alloc_array(type, count)` expands to: `(type*)memory_alloc(sizeof(type)*count)`.

    You can expect both of these to be used in preference to the actual function call.

-   ```C++
    void memory_free(void *ptr);
    ```

    The wrapper for `free()`; calls `free()` after performing the necessary checks and work
    to update diagonistics information. This function doesn't check for double frees.

-   ```C++
    bool memory_statistics(memory_alloc_stats *stats);
    ```

    If `memory_alloc_stats` is provided and not-null, fills out the structure with
    diagnostic information regarding the calls to `malloc()` and `free()`. The return
    value of this function determines if there is a mismatch or potential memory leak
    in the application.

Below is an example demonstrating the API:

```C++
int* number_a = memory_alloc_type(int);
int* number_b = memory_alloc_type(int);

*number_a = 12;
*number_b = 14;
int sum = *number_a + *number_b;

memory_free(number_a);
memory_free(number_b);

memory_alloc_stats memory_stats = {};
if (!memory_statistics(&memory_stats))
{
    std::cout << "Memory allocation/free mismatch: " << std::endl;
    std::cout << "    Total Size      : " << memory_stats.memory_allocated << " bytes" << std::endl;
    std::cout << "    Free Size       : " << memory_stats.memory_freed << " bytes" << std::endl;
    std::cout << "    Calls to Malloc : " << memory_stats.alloc_calls << std::endl;
    std::cout << "    Calls to Free   : " << memory_stats.free_calls << std::endl;
}
```
