#ifndef C_IMPLEMENTATION_HASH_SET_H
#define C_IMPLEMENTATION_HASH_SET_H

#ifdef __cplusplus
extern "C" {
#endif

#define DEFAULT_HASH_SET_CAPACITY (1 << 10)

    typedef struct {
        char* key;
        void* value;
    } hash_entry_t;

    typedef struct {
        unsigned capacity;
        unsigned length;
        hash_entry_t* table;
    } hash_map_t;

    hash_map_t* init_hash_map();
    void* hash_map_get(hash_map_t* map, const char* key);
    void hash_map_put(hash_map_t* map, const char* key, void* value);
    void hash_map_delete(hash_map_t* map, const char* key);
    long long hash_str(const char* str);
    void free_hash_map(hash_map_t* map);

#ifdef __cplusplus
}
#endif


#endif