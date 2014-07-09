/* init_node.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "config.h"
#include "init_node.h"
#include "socket.h"

struct nodeinfo {
	char *hostname; 	// hostname of the machine
	char *keynode;		// node that introduced this node to the cluster
	char *internalhost;	// local ip of node
	char *externalhost;	// public facing ip of the node
	char *neighbour[2];	// nodes that this node talks to
};

char na[] = "N/A";
// populates new nodeinfo struct and returns pointer
struct nodeinfo *create_node(char *hn, char *kn, char *ih, char *eh)
{
	struct nodeinfo *node 	= malloc(sizeof(struct nodeinfo));
	node->hostname 		= hn ? strdup(hn) : na; 
	node->internalhost 	= ih ? strdup(ih) : na;
	node->keynode		= kn ? strdup(kn) : na;
	node->externalhost 	= eh ? strdup(eh) : na;
	node->neighbour[0] 	= na;
	node->neighbour[1] 	= na;
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
		free(node);
	}
}

// node list item struct
struct nli {
	struct nodeinfo *info;
	struct nli *prev;
	struct nli *next;
};

// add/insert node in list
struct nli *add_node_to_list(struct nli *cur)
{
	struct nli *p 	= malloc(sizeof(struct nli));
	p->next 	= NULL;
	if (cur) {
		if (cur->next) {
			cur->next->prev 	= p;
			p->next 		= cur->next;
		}
		cur->next 			= p;
	}
	p->prev 				= cur;
	return p;
}

// create new node list, returns pointer to first item in new list
struct nli *create_node_list()
{
	return add_node_to_list(NULL);
}

// removes item from list and returns pointer to item before deleted
struct nli *remove_node_from_list(struct nli *node)
{
	struct nli *n = node->prev;
	if (node->prev) node->prev->next = node->next;
	if (node->next) node->next->prev = node->prev;
	destroy_node(node->info);
	free(node);
	return n;
}

// destroys from pointer to item, to end of list
void destroy_node_list(struct nli *node)
{
	if (node) {
		do {
			destroy_node(node->info);
		} while (node = node->next);
	}
}


// count items left in list
int count_node_list(struct nli *node)
{
	int i = 0;
	if (node) {
		do {
			i++;
		} while (node = node->next);
	}
	return i;
}

char st[] = "|start_of_block|";
char ed[] = "|end_of_block|";
char dl[] = "\n";

char *append_to_buf(char *buf, int *m_sp, char *str)
{
	int p_siz = *m_sp;
	*m_sp = *m_sp + (strlen(str) + 1) * sizeof(char);
	buf = realloc(buf, *m_sp);
	memcpy(buf + p_siz, str, strlen(str) * sizeof(char));
	memcpy(buf + p_siz + strlen(str), dl, strlen(dl) * sizeof(char));
	return buf;
}

void log_nodelist(struct nli *node)
{
	if (node) {
		struct nodeinfo *nfo;
		do
		{
			nfo = node->info;
			printf("%s: ",			nfo->hostname);
			printf("internal ip %s, ", 	nfo->internalhost);
			printf("public ip %s, ", 	nfo->externalhost);
			printf("keynode %s.\n",		nfo->keynode);
		} while (node = node->next);
	}
}

// serializes from pointer to item, to end of list. 
// returns pointer to new buffer.
char *serialize(struct nli *node)
{
	char *buf;
	if (node) {
		struct nodeinfo *nfo;
		int m_siz = 0;
		int *m_sp = &m_siz;

		buf = NULL;
		buf = append_to_buf(buf, m_sp, st);
		do
		{
			nfo = node->info;
			buf = append_to_buf(buf, m_sp, st);
			buf = append_to_buf(buf, m_sp, nfo->hostname);
			buf = append_to_buf(buf, m_sp, nfo->keynode);
			buf = append_to_buf(buf, m_sp, nfo->internalhost);
			buf = append_to_buf(buf, m_sp, nfo->externalhost);
			buf = append_to_buf(buf, m_sp, ed);
		} while (node = node->next);
		buf = append_to_buf(buf, m_sp, ed);
		buf[m_siz - 1] = '\0'; //turn last delimiter into null terminator
	}
	return buf;
}

// deserializes buffer to nodeinfo linked list.
// returns pointer to new list.
struct nli *deserialize(char *buf)
{
	struct nli *head = NULL, *np = NULL;

	int l = strlen(buf);
	int nest = 0, el = 0;
	char *sg = strtok(buf, dl);
	char *hn, *kn, *ih, *pf;
	do {
		if (strstr(sg, st)) {
			el = 0;
			if (nest < 2) ++nest;	
		}
		if (strstr(sg, ed)) {
			if (nest == 2) {
				np->info 	= create_node(hn, kn, ih, pf);
			}
		       	if (nest > 0) --nest;
		}
		sg = strtok(NULL, dl);
		if (nest == 2) {
			switch(el) {
				case 0: 
					if (np) {
						np = add_node_to_list(np);
					} else {
						head = create_node_list();
						np = head;
					}
					hn 	= strdup(sg);
					break;
				case 1: 
					kn 	= strdup(sg);
					break;
				case 2: 
					ih	= strdup(sg);
					break;
				case 3:
					pf	= strdup(sg);
					break;
			}
			el++;
		}
	} while (sg);
	return head;
}

void buf_callback(char *buf)
{
	struct nli *nl = deserialize(buf);
	int nodesleft = count_node_list(nl);
	if (config->verbosity) {
		log_nodelist(nl);
	}
}

int init_node()
{
	char *hn, *kn, *ih, *pf;
	struct nli *np, *head;
	head = create_node_list();

	hn = hostname();
	kn = config->keynode;
	ih = internalhost();
	pf = config->publicface;
	np = head;
	head->info 	= create_node(hn, kn, ih, pf);

	hn = strdup("neigh1");
	kn = strdup("keynode1");
	ih = strdup("192.168.1.2");
	pf = strdup("n1.rickvandeloo.com");
	np = add_node_to_list(np);
	np->info 	= create_node(hn, kn, ih, pf);

	hn = strdup("neigh2");
	kn = strdup("keynode2");
	ih = strdup("192.168.2.3");
	pf = strdup("n2.rickvandeloo.com");
	np = add_node_to_list(np);
	np->info 	= create_node(hn, kn, ih, pf);

	if (config->server) {
		receive_packets(4040, buf_callback);
	} else {
		// serializing for transmission
		char *buf;
		buf = serialize(head);
		send_packets("test", 4040, buf);
		free(buf);
	}


	destroy_node_list(head);
	printf("now exiting init_node()\n");
}
