#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "hash.h"

static int isprime(int number);
static int nextprime(int number);

static void hashexpand(hashtable *table);

static unsigned chrhash(void *s, int M);
static int chrcomp(void *s1, void *s2);
static void * chrdupe(void *key);

static unsigned inthash(void *s, int M);
static int intcomp(void *s1, void *s2);
static void * intdupe(void *i);


void
hashcreate(hashtable *table, keytype typ, int size)
{
	table->size=nextprime(size);
	table->numkeys=0;

	if (typ == CHAR) {
		table->hashfunc=chrhash;
		table->compfunc=chrcomp;
		table->dupefunc=chrdupe;
	}
	else { /* INT */
		table->hashfunc=inthash;
		table->compfunc=intcomp;
		table->dupefunc=intdupe;
	}

	assert((table->table=(calloc(table->size, sizeof(bucket *)))) != NULL);
}

void
hashinsert(hashtable *table, void *key, void *val)
{
	unsigned h=table->hashfunc(key, table->size);
	bucket *p;

	assert((p=(bucket *)malloc(sizeof(bucket))) != NULL);
	assert((p->key=table->dupefunc(key)) != NULL);
	p->val=val;

	for (; (table->table)[h] != NULL; h=(h+1)%table->size)
		;
	
	(table->table)[h]=p;

	if (table->numkeys++ > table->size/2)
		hashexpand(table);
}

void *
hashfind(hashtable *table, void *key)
{
	register unsigned h=table->hashfunc(key, table->size);

	for (; (table->table)[h] != NULL; h=(h+1)%table->size)
		if (table->compfunc((table->table)[h]->key, key) == 0)
			return (table->table)[h]->val;

	return NULL;
}

void
hashdelete(hashtable *table, void *key)
{
	register unsigned h=table->hashfunc(key, table->size);

	for (; (table->table)[h] != NULL; h=(h+1)%table->size) {
		if (table->compfunc((table->table)[h]->key, key) == 0) {
			free((table->table)[h]->key);
			free((table->table)[h]->val);
			(table->table)[h]->key=(table->table)[h]->val=NULL;
			free((table->table)[h]);
			(table->table)[h]=NULL;
		}
	}
}

void
hashforeach(hashtable *table, void (*func)(void *, void *))
{
	int l;

	for (l=0; l<table->size; l++)
		if ((table->table)[l] != NULL)
			func((table->table)[l]->key, (table->table)[l]->val);
}

void
hashempty(hashtable *table)
{
	int l;

	for (l=0; l<table->size; l++) {
		if ((table->table)[l] != NULL) {
			free((table->table)[l]->key);
			free((table->table)[l]->val);
			free((table->table)[l]);
			(table->table)[l]=NULL;
		}
	}

	table->numkeys=0;
}

static void
hashexpand(hashtable *table)
{
	int size, l;
	bucket **t;
	unsigned h;

	size=nextprime(table->size*2);

	assert((t=(calloc(size, sizeof(bucket *)))) != NULL);

	for (l=0; l<table->size; l++) {
		if ((table->table)[l] != NULL) {
			h=table->hashfunc((table->table)[l]->key, size);

			for (; t[h] != NULL; h=(h+1)%size)
				;

			t[h]=(table->table)[l];
		}
	}

	free(table->table);

	table->size=size;
	table->table=t;
}

static int
nextprime(int number)
{
	for (;!isprime(number);number++)
		;

	return number;
}

static int
isprime(int number)
{
	int sqr, l;

	sqr=sqrt(number);

	for (l=2;l<=sqr;l++)
		if (number%l == 0)
			return 0;

	return 1;
}

static unsigned
chrhash(void *p, int M)
{
	register unsigned hashval;
	register char *s=p;

	for (hashval=0; *s!='\0'; s++)
		hashval=*s+31*hashval;

	return hashval%M;
}

static int
chrcomp(void *s1, void *s2)
{
	return strcmp((char *)s1, (char *)s2);
}

static void *
chrdupe(void *key)
{
	return (void *)strdup((char *)key);
}

static unsigned
inthash(void *i, int M)
{
	return *(int *)i%M;
}

static int
intcomp(void *i1, void *i2)
{
	return *(int *)i1-*(int *)i2;
}

static void *
intdupe(void *i)
{
	int *p;


	assert((p=(int *)malloc(sizeof(int))) != NULL);

	*p=*(int *)i;

	return p;
}







