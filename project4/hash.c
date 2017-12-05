#include "list.c"

typedef struct _hash_t
{
	list_t *list_head;
	int size;
}hash_t;

int hash_code(int key, int size)
{//get the hash code
	return key % size;
}

void hash_init(hash_t *hash, int size)
{
	hash -> list_head = (list_t *)malloc(sizeof(list_t));
	hash -> size = size;
	int i = 0;
	while(i != size)
	{
		list_init(hash -> list_head + i);
		i++;
	}
}
void hash_insert(hash_t *hash, unsigned int key)
{
	int bucket = hash_code(key, hash -> size);
	return list_insert(hash -> list_head + bucket, key);
}

void hash_delete(hash_t *hash, unsigned int key)
{
	int bucket = hash_code(key, hash -> size);
	return list_delete(hash -> list_head + bucket, key);
}

void *hash_lookup(hash_t *hash, unsigned int key)
{
	int bucket = hash_code(key, hash -> size);
	return list_lookup(hash -> list_head + bucket, key);
}

