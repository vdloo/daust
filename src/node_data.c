/* node_data.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "node_data.h"

// populates nodeinfo element and returns pointer to that element
char na[] = "N/A";
char *set_node_element(char **el, char *buf)
{
	*el = buf ? strdup(buf) : na;
	return *el;
}

// updates nodeinfo timestamp and returns pointer to the timestamp
time_t update_node_timestamp(struct nodeinfo *node)
{
	node->timestamp = time(NULL);
	return node->timestamp;
}

// creates new empty nodeinfo struct
struct nodeinfo *create_node()
{
	struct nodeinfo *node 	= malloc(sizeof(struct nodeinfo));
	node->neighbour[0] 	= na;
	node->neighbour[1] 	= na;
	update_node_timestamp(node);
	return node;
}

// clears memory of nodeinfo struct pointed to by argument
void destroy_node(struct nodeinfo *node)
{
	if (node) {
		if (node->hostname 	!= na) free(node->hostname);
		if (node->internalhost 	!= na) free(node->internalhost);
		if (node->keynode 	!= na) free(node->keynode);
		if (node->externalhost 	!= na) free(node->externalhost);
		if (node->identifier 	!= na) free(node->identifier);
		free(node);
	}
}

// add/insert node list item into list
struct nli *add_node_to_list(struct nli *node)
{
	struct nli *np 	= malloc(sizeof(struct nli));
	np->next 	= NULL;
	if (node) {
		if (node->next) {
			node->next->prev 	= np;
			np->next 		= node->next;
		}
		node->next 			= np;
	}
	np->prev 				= node;
	return np;
}

// create new node list, returns pointer to first item in new list
struct nli *create_nodelist()
{
	return add_node_to_list(NULL);
}

// removes node list item from list and clears its allocated memory,
// destroy_node continues to clear the memory of the associated nodeinfo
// struct. returns pointer to the item before the one deleted
struct nli *remove_node_from_list(struct nli *node)
{
	struct nli *np = NULL;
	if (node) {
		np = node->prev;
		if (node->prev) node->prev->next = node->next;
		if (node->next) node->next->prev = node->prev;
		destroy_node(node->info);
		free(node);
	}
	return np;
}

// removes all node list items from list and clears their allocated
// memory. pass a pointer to any node list item in list to clear the
// entire list
void destroy_nodelist(struct nli *node)
{
	if (node) {
		// start with the last node list item in list
		while (node) {
			node = node->next;
		}
		// remove node list items until there are no more
		// previous node list items left in the list
		do {
			remove_node_from_list(node);
			if (node) {
				node = node->prev;
			}
		} while (node);
	}
}


// count node list items from pointer to node list item to end of list
int count_nodelist(struct nli *node)
{
	int i = 0;
	if (node) {
		do {
			i++;
		} while (node = node->next);
	}
	return i;
}

void log_nodelist(struct nli *node)
{
	if (node) {
		struct nodeinfo *nfo;
		do
		{
			nfo = node->info;
			// change this to writing printf into a buffer
			// and then printing it AND writing it to a
			// log file if it is specified as an option
			printf("%s: ",			nfo->hostname);
			printf("internal ip %s, ", 	nfo->internalhost);
			printf("public ip %s, ", 	nfo->externalhost);
			printf("keynode %s.\n",		nfo->keynode);
		} while (node = node->next);
	}
}
// tries to find node based on identifier to the end of
// the list, then returns pointer to that node.
struct nli *node_by_identifier(struct nli *node, char *ident)
{
	struct nli *match = NULL;
	if (node) {
		struct nodeinfo *nfo;
		do
		{
			nfo = node->info;
			if (strstr(node->info->identifier, ident)) {
				match = node;	
			}
		} while (node = node->next);
	}
	return match;
}

struct nli *join_lists(struct nli *local, struct nli *foreign)
{
	if (foreign) {
		struct nli *match = NULL;
		struct nodeinfo *nfo;
		char *hn, *kn, *ih, *eh, *id;
		int i = 0;
		do
		{
			hn = foreign->info->hostname;
			kn = foreign->info->keynode;
			ih = foreign->info->internalhost;
			eh = foreign->info->externalhost;
			id = foreign->info->identifier;

			// update nodeinfo from broadcasting node,
			// add other nodes if it they don't locally
			// exist yet
			match 	= node_by_identifier(local, id);
			if (i < 1 && match) {
				nfo 	= match->info;	
			} else if (!match){
				local = add_node_to_list(local);
				nfo = local->info = create_node();
			}
			set_node_element(&nfo->hostname,	hn);
			set_node_element(&nfo->keynode, 	kn);
			set_node_element(&nfo->internalhost, 	ih);
			set_node_element(&nfo->externalhost, 	eh);
			set_node_element(&nfo->identifier, 	id);
			update_node_timestamp(nfo);

			i++;

		} while (foreign = foreign->next);
	}
	return local;
}
