/**
 * @file C linked list
 * Stephen Zhang, 2014
 */

#include <stdlib.h>
#include "clist.h"

clist clist_create(){
	clist c;
	c.begin=c.curr=c.end=0;	/* initialize to NULL */
	return c;
}

static void join(clist_node* a, clist_node* b){
	/* joins two nodes */
	if(a){
		a->next=b;
	}
	if(b){
		b->prev=a;
	}
}

void clist_push_back(clist* c, void* data){
	clist_node* e=(clist_node*)malloc(sizeof(clist_node));
	e->data=data;
	e->prev=0;
	e->next=0;
	if(c->end==0 && c->begin==0){
		/* special case - empty list */
		c->begin=c->end=e;
		return;
	}
	join(c->end, e);
	c->end=e;
}

void clist_push_front(clist* c, void* data){
	clist_node* e=(clist_node*)malloc(sizeof(clist_node));
	e->data=data;
	e->prev=0;
	e->next=0;
	if(c->end==0 && c->begin==0){
		/* special case - empty list */
		c->begin=c->end=e;
		return;
	}
	join(e, c->begin);
	c->begin=e;
}

void clist_pop_back(clist* c){
	clist_node* e=c->end->prev;
	free(c->end->data);
	free(c->end);
	c->end=e;
}

void clist_pop_front(clist* c){
	clist_node* e=c->begin->next;
	free(c->begin->data);
	free(c->begin);
	c->begin=e;
}

void* clist_getcurr(clist* c){
	return c->curr->data;
}

void clist_begin(clist* c){
	c->curr=c->begin;
}

void clist_end(clist* c){
	c->curr=c->end;
}

void clist_next(clist* c){
	if(c->curr->next){
		c->curr=c->curr->next;
	}
}

void clist_prev(clist* c){
	if(c->curr->prev){
		c->curr=c->curr->prev;
	}
}

void clist_destroy(clist* c){
	clist_node* e=c->begin;
	while(e!=0){
		clist_node* n;
		free(e->data);
		n=e->next;
		free(e);
		e=n;
	}
}
