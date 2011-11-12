/* Copyright (c) 2011 the authors listed at the following URL, and/or
the authors of referenced articles or incorporated external code:
http://en.literateprograms.org/Hash_table_(C)?action=history&offset=20100620072342

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Retrieved from: http://en.literateprograms.org/Hash_table_(C)?oldid=16749
*/

#include "hashtbl.h"

#include <string.h>
#include <stdio.h>

static char *mystrdup(const char *s)
{
	char *b;
	if(!(b=malloc(strlen(s)+1))) return NULL;
	strcpy(b, s);
	return b;
}


static hash_size def_hashfunc(const char *key)
{
	hash_size hash=0;
	
	while(*key) hash+=(unsigned char)*key++;

	return hash;
}



HASHTBL *hashtbl_create(hash_size size, hash_size (*hashfunc)(const char *))
{
	HASHTBL *hashtbl;

	if(!(hashtbl=malloc(sizeof(HASHTBL)))) return NULL;

	if(!(hashtbl->nodes=calloc(size, sizeof(struct hashnode_s*)))) {
		free(hashtbl);
		return NULL;
	}

	hashtbl->size=size;

	if(hashfunc) hashtbl->hashfunc=hashfunc;
	else hashtbl->hashfunc=def_hashfunc;

	return hashtbl;
}


void hashtbl_destroy(HASHTBL *hashtbl)
{
	hash_size n;
	struct hashnode_s *node, *oldnode;
	
	for(n=0; n<hashtbl->size; ++n) {
		node=hashtbl->nodes[n];
		while(node) {
			free(node->key);
			free(node->data);  // added 
			oldnode=node;
			node=node->next;
			free(oldnode);
		}
	}
	free(hashtbl->nodes);
	free(hashtbl);
}


int hashtbl_insert(HASHTBL *hashtbl, const char *key, void *data)
{
	struct hashnode_s *node;
	hash_size hash=hashtbl->hashfunc(key)%hashtbl->size;


/*	fprintf(stderr, "hashtbl_insert() key=%s, hash=%d, data=%s\n", key, hash, (char*)data);*/

	node=hashtbl->nodes[hash];
	while(node) {
		if(!strcmp(node->key, key)) {
			//node->data=data; removed old

			// this new code change the existing data with the new one
			// free(node->data=data);
       			//node->data = (char *) malloc (strlen ((char*) data) + 1); //added
			//sprintf(node->data,"%s",data); // added 
			
			// this new code does nothing is the key already exists
			fprintf(stderr,"Key already existing: [key=%s] [data=%s] [skip add element]",node->key, node->data); // added 
			return 0;
		}
		node=node->next;
	}


	if(!(node=malloc(sizeof(struct hashnode_s)))) return -1;
	if(!(node->key=mystrdup(key))) {
		free(node);
		return -1;
	}
	//node->data=data;
       	node->data = (char *) malloc (strlen ((char*) data) + 1); //added
	sprintf(node->data,"%s",data); // added 

	node->next=hashtbl->nodes[hash];
	hashtbl->nodes[hash]=node;


	return 0;
}


int hashtbl_remove(HASHTBL *hashtbl, const char *key)
{
	struct hashnode_s *node, *prevnode=NULL;
	hash_size hash=hashtbl->hashfunc(key)%hashtbl->size;

	node=hashtbl->nodes[hash];
	while(node) {
		if(!strcmp(node->key, key)) {
			free(node->key);
			free(node->data); // added
			if(prevnode) prevnode->next=node->next;
			else hashtbl->nodes[hash]=node->next;
			free(node);
			return 0;
		}
		prevnode=node;
		node=node->next;
	}

	return -1;
}


void *hashtbl_get(HASHTBL *hashtbl, const char *key)
{
	struct hashnode_s *node;
	hash_size hash=hashtbl->hashfunc(key)%hashtbl->size;

/*	fprintf(stderr, "hashtbl_get() key=%s, hash=%d\n", key, hash);*/

	node=hashtbl->nodes[hash];
	while(node) {
		if(!strcmp(node->key, key)) return node->data;
		node=node->next;
	}

	return NULL;
}

int hashtbl_resize(HASHTBL *hashtbl, hash_size size)
{
	HASHTBL newtbl;
	hash_size n;
	struct hashnode_s *node,*next;

	newtbl.size=size;
	newtbl.hashfunc=hashtbl->hashfunc;

	if(!(newtbl.nodes=calloc(size, sizeof(struct hashnode_s*)))) return -1;

	for(n=0; n<hashtbl->size; ++n) {
		for(node=hashtbl->nodes[n]; node; node=next) {
			next = node->next;
			hashtbl_insert(&newtbl, node->key, node->data);
			hashtbl_remove(hashtbl, node->key);
			
		}
	}

	free(hashtbl->nodes);
	hashtbl->size=newtbl.size;
	hashtbl->nodes=newtbl.nodes;

	return 0;
}

