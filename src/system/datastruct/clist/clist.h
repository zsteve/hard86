/**
 * @file C linked list
 * Stephen Zhang, 2014
 */

#ifndef CLIST_H
#define CLIST_H

#ifdef __cplusplus
extern "C"{
#endif

#ifdef __cplusplus
typedef struct clist_node {
	void* data;
	clist_node *prev, *next;
}clist_node;
#else
typedef struct {
	void* data;
	struct clist_node *prev, *next;
}clist_node;
#endif

typedef struct {
	clist_node *begin, *curr, *end;
}clist;

clist clist_create();
void clist_push_back(clist* c, void* data);
void clist_push_front(clist* c, void* data);
void clist_pop_back(clist* c);
void clist_pop_front(clist* c);
void* clist_getcurr(clist* c);
void clist_begin(clist* c);
void clist_end(clist* c);
int clist_next(clist* c);
void clist_prev(clist* c);
void clist_destroy(clist* c);

#ifdef __cplusplus
}
#endif

#endif
