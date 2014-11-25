#ifndef _HASH_H
#define _HASH_H 1

typedef enum {CHAR, INT} keytype;

typedef struct {
		void *key;
		void *val;
} bucket;

typedef struct {
		int size;
		int numkeys;
		unsigned (*hashfunc)(void *key, int M);
		int (*compfunc)(void *c1, void *c2);
		void * (*dupefunc)(void *key);
		bucket **table;
} hashtable;

void hashcreate(hashtable *table, keytype typ, int size);
void hashinsert(hashtable *table, void *key, void *val);
void * hashfind(hashtable *table, void *key);
void hashforeach(hashtable *table, void (*func)(void *, void *));
void hashempty(hashtable *table);
void hashdelete(hashtable *table, void *key);

#endif
