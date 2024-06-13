


typedef struct head_list{
    head_list* prev, next;
}head_list;

typedef struct hash_entry {
    int key;
    void *p;
    head_list list;
}hash_entry;

typedef struct hasv_v1 {
    int ele_size;
    hash_entry** table;
}hasv_v1;


/******************API*********************/
hasv_v1 * create_hash_table_v1(void);
