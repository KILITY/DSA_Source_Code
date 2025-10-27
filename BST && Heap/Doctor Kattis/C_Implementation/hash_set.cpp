/* =====================================================================
 * HASH MAP / HASH SET IMPLEMENTATION
 * =====================================================================
 *
 * A Hash Map (also called Hash Table or Hash Set) is a data structure that
 * stores key-value pairs with extremely fast lookup, insertion, and deletion.
 *
 * KEY CONCEPT - HASHING:
 * Instead of searching through a list, we use a hash function to convert
 * a key (like a string) into an array index. This lets us jump directly
 * to where the data is stored, giving us O(1) average time complexity.
 *
 * HASH COLLISION:
 * The problem: Two different keys might hash to the same index.
 * Solution (the one I use): Linear Probing - if a spot is taken, try the next one.
 *
 * LOAD FACTOR:
 * The ratio of items stored to total capacity (50 items in 100 slots = 0.5).
 * When load factor gets too high, we resize the table to more slots.
 * ===================================================================== */

#include "hash_set.h"
#include <stdlib.h>
#include <string.h>

/* =====================================================================
 * hash_str: Converts a string into a hash value (index number)
 * =====================================================================
 * This is the "hashing" step - converting a string key into a number
 * that represents where it should go in our array.
 *
 * ALGORITHM USED: Polynomial rolling hash
 *
 * PARAMETERS:
 *   a = running sum of character values
 *   b = running sum of the running sums (adds positional weight)
 *   MOD = modulo value (65521) to keep numbers manageable
 *
 * WHY TWO VALUES (a and b)?
 *   Using both components gives us a better distribution - reduces collisions
 *   where two different strings would hash to the same number.
 *
 * FINAL STEP:
 *   ((long long)b << 16) | a
 *   This combines 'a' and 'b' into a single 64-bit number:
 *   - Shifts 'b' left 16 bits (makes room for 'a')
 *   - Uses bitwise OR to merge them
 *
 * EXAMPLE: "Hi"
 *   'H' = 72, 'i' = 105
 *   After processing, might produce something like: 12345678
 * ===================================================================== */
long long hash_str(const char* str) {
    int a = 1, b = 0;  // Initialize a to 1 (multiplicative identity)
    const int MOD = 65521;  // Modulo prevents integer overflow
    for (int i = 0; str[i]; ++i) {
        a = (a + (unsigned char)str[i]) % MOD;
        // Add the updated 'a' to 'b' (gives positional information)
        b = (b + a) % MOD;
    }
    // Combine both hash values into one 64-bit number and return
    return ((long long)b << 16) | a;
}

/* =====================================================================
 * index_from_hash: Converts a hash value to an actual array index
 * =====================================================================
 * Takes the large hash number and converts it to a valid array position
 * (0 to capacity-1).
 *
 * BITWISE OPERATION BREAKDOWN:
 *   (cap - 1) & (h ^ (h >> 12))
 *
 *   h >> 12:       Right shift h by 12 bits (divide by 4096 approximately)
 *   h ^ (h >> 12): XOR operation - mixes high and low bits together
 *                  (helps distribute collisions more evenly)
 *   (cap - 1) &:   Bitwise AND with (capacity - 1)
 *                  This is a fast way to do modulo when capacity is a power of 2
 *
 * WHY POWER OF 2?
 *   If capacity = 16 (binary: 10000), then capacity - 1 = 15 (binary: 01111)
 *   ANDing with 01111 gives us any value from 0 to 15
 *
 * Parameters:
 *   h   - The hash value from hash_str()
 *   cap - Current capacity of the hash table
 * ===================================================================== */
static unsigned index_from_hash(long long h, unsigned cap) {
    return (cap - 1) & (h ^ (h >> 12));
}

hash_map_t* init_hash_map() {
    hash_map_t* map = (hash_map_t*)malloc(sizeof(hash_map_t));
    map->capacity = DEFAULT_HASH_SET_CAPACITY;  // Initial size (usually 16)
    map->length = 0;  // Currently storing 0 items
    // calloc allocates memory AND zeros it out (all entries start empty)
    map->table = (hash_entry_t*)calloc(map->capacity, sizeof(hash_entry_t));
    return map;
}

static void resize(hash_map_t* map) {
    unsigned oldcap = map->capacity;  // Save the old capacity
    hash_entry_t* oldtab = map->table;  // Save the old table

    map->capacity <<= 1;
    map->table = (hash_entry_t*)calloc(map->capacity, sizeof(hash_entry_t));
    map->length = 0;

    for (unsigned i = 0; i < oldcap; ++i) {
        if (oldtab[i].key) {
            // Re-hash this entry in the new, larger table
            long long h = hash_str(oldtab[i].key);
            unsigned idx = index_from_hash(h, map->capacity);

            while (map->table[idx].key)
                idx = (idx + 1) % map->capacity;

            map->table[idx].key = oldtab[i].key;
            map->table[idx].value = oldtab[i].value;
            map->length++;
        }
    }

    free(oldtab);
}

void hash_map_put(hash_map_t* map, const char* key, void* value) {
    // Check if load factor is too high (≥ 50%)
    if (map->length * 2 >= map->capacity)
        resize(map);  // Double

    long long h = hash_str(key);
    unsigned idx = index_from_hash(h, map->capacity);

    while (map->table[idx].key) {
        if (strcmp(map->table[idx].key, key) == 0) {
            map->table[idx].value = value;
            return;
        }
        idx = (idx + 1) % map->capacity;
    }

    map->table[idx].key = strdup(key);  // strdup makes a copy of the key string - string duplicate
    map->table[idx].value = value;
    map->length++;
}

void* hash_map_get(hash_map_t* map, const char* key) {
    long long h = hash_str(key);
    unsigned idx = index_from_hash(h, map->capacity);

    while (1) { //looks scary, but it is safe because we will always at least find an empty slot
        if (!map->table[idx].key)
            return NULL;
        if (strcmp(map->table[idx].key, key) == 0)
            return map->table[idx].value;
        idx = (idx + 1) % map->capacity;
    }
}

void hash_map_delete(hash_map_t* map, const char* key) {
    long long h = hash_str(key);
    unsigned idx = index_from_hash(h, map->capacity);

    while (1) {
        if (!map->table[idx].key)
            return;
        if (strcmp(map->table[idx].key, key) == 0) {
            free(map->table[idx].key);
            map->table[idx].key = NULL;
            map->table[idx].value = NULL;
            map->length--;
            return;
        }
        // This slot is taken but not our key, try next
        idx = (idx + 1) % map->capacity;
    }
}

void free_hash_map(hash_map_t* map) {
    for (unsigned i = 0; i < map->capacity; ++i)
        if (map->table[i].key)
            free(map->table[i].key);

    free(map->table);
    free(map);
}