/* node_data.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "node_data.h"
#include "utils.h"
#include "config.h"

// populates nodeinfo element and returns pointer to that element
char na[] = "N/A";
char *set_node_element(char **el, char *buf)
{
	if (*el) free(*el);
	*el = buf ? strdup(buf) : strdup(na);
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
	node->hostname		= strdup(na);
	node->internalhost	= strdup(na);
	node->keynode		= strdup(na);
	node->externalhost	= strdup(na);
	node->identifier	= strdup(na);
	//node->neighbour[0] 	= strdup(na);
	//node->neighbour[1] 	= strdup(na);
	node->command		= strdup(na);
	node->unique		= gen_uuid();
	update_node_timestamp(node);
	return node;
}

// copy nodeinfo struct into a new block of memory
// and return a pointer to that block
struct nodeinfo *dup_nodeinfo(struct nodeinfo *nfo)
{
	struct nodeinfo *dnfo = NULL;
	if (nfo) {
		dnfo = malloc(sizeof(struct nodeinfo));
		dnfo->hostname		= strdup(nfo->hostname);
		dnfo->internalhost	= strdup(nfo->internalhost);
		dnfo->keynode		= strdup(nfo->keynode);
		dnfo->externalhost	= strdup(nfo->externalhost);
		dnfo->identifier	= strdup(nfo->identifier);
		dnfo->command		= strdup(nfo->command);
		dnfo->unique		= strdup(nfo->unique);
		dnfo->timestamp		= nfo->timestamp;
	}
	return dnfo;
}


// clears memory of nodeinfo struct pointed to by argument
void destroy_node(struct nodeinfo *nfo)
{
	if (nfo) {
		if (nfo->hostname) free(nfo->hostname);
		if (nfo->internalhost) free(nfo->internalhost);
		if (nfo->keynode) free(nfo->keynode);
		if (nfo->identifier) free(nfo->identifier);
		if (nfo->command) free(nfo->command);
		if (nfo->externalhost) free(nfo->externalhost);
		if (nfo->unique) free(nfo->unique);
		free(nfo);
	}
}

// add/insert node list item into list
struct nli *add_node_to_list(struct nli *node)
{
	struct nli *np 	= malloc(sizeof(struct nli));
	np->next 	= NULL;
	np->prev	= NULL;
	if (node) {
		if (node->next) {
			node->next->prev 	= np;
			np->next 		= node->next;
		}
		node->next 			= np;
	}
	np->prev 			= node;
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
		while (node->next) {
			node = node->next;
		}
		// remove node list items until there are no more
		// previous node list items left in the list
		do {
			node = remove_node_from_list(node);
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

char *nodelist_list(struct nli *node)
{
	char *buf = NULL;
	if (node) {
		struct nodeinfo *nfo;
		int ms = 0;
		int *mp = &ms;
		do
		{
			if (buf) buf = asdtobfp(buf, mp, " ", "\n");
			nfo = node->info;
			buf = astobfp(buf, mp, nfo->hostname);
			buf = astobfp(buf, mp, ": internal ip ");
			buf = astobfp(buf, mp, nfo->internalhost);
//			buf = astobfp(buf, mp, ", public ip ");
//			buf = astobfp(buf, mp, nfo->externalhost);
//			buf = astobfp(buf, mp, ", keynode ip ");
//			buf = astobfp(buf, mp, nfo->keynode);
//			buf = astobfp(buf, mp, ", command ");
//			buf = astobfp(buf, mp, nfo->command);
			buf = astobfp(buf, mp, ", unique ");
			buf = astobfp(buf, mp, nfo->unique);
		} while (node = node->next);
		buf = astobfp(buf, mp, NULL);
	}
	return buf;
}

// tries to find node based on identifier to the end of
// the list, then returns pointer to that node.
struct nli *node_by_identifier(char *ident, struct nli *node)
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

struct nli *node_by_hostname(char *hostname, struct nli *node)
{
	struct nli *match = NULL;
	if (node) {
		struct nodeinfo *nfo;
		do
		{
			nfo = node->info;
			if (strstr(node->info->hostname, hostname)) {
				match = node;	
			}
		} while (node = node->next);
	}
	return match;
}

char *externalhost_by_hostname(char *hostname, struct nli *node)
{
	struct nli *nl = node_by_hostname(hostname, node);
	return nl ? nl->info->externalhost: NULL;
}

char *internalhost_by_hostname(char *hostname, struct nli *node) 
{
	struct nli *nl = node_by_hostname(hostname, node);
	return nl ? nl->info->internalhost: NULL;
}

// check if identifier and command are the same, if so return pointer
// to n2, else NULL
struct nli *compare_nodes(struct nli *n1, struct nli *n2)
{
	struct nli *match = NULL;
	if (strcmp( n1->info->unique, n2->info->unique) == 0) {
		match = n2;
	}

	return match;
}

// find nli needle in nodelist haystack and return pointer
struct nli *find_node(struct nli *needle, struct nli *haystack)
{
	struct nli *match = NULL;
	if (needle) {
		do
		{
			if (!match) match = compare_nodes(needle, haystack);
		} while (haystack = haystack->next);
	}
	return match;
}

struct nli *join_lists(struct nli *local, struct nli *foreign)
{
	if (foreign) {
		struct nli *match = NULL;
		struct nodeinfo *nfo;
		char *hn, *kn, *ih, *eh, *id, *uq;
		int i = 0;
		do
		{
			hn = foreign->info->hostname;
			printf("trying to join %s\n", hn);
			kn = foreign->info->keynode;
			ih = foreign->info->internalhost;
			eh = foreign->info->externalhost;
			id = foreign->info->identifier;
			uq = foreign->info->unique;

			// update nodeinfo from broadcasting node,
			// add other nodes if it they don't locally
			// exist yet
			match 	= node_by_identifier(id, local);
			if (match) {
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
			set_node_element(&nfo->unique, 		uq);
			update_node_timestamp(nfo);

			i++;

		} while (foreign = foreign->next);
	}
	return local;
}
