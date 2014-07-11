/* node_data.h */
#ifndef NODE_DATA_H
#define NODE_DATA_H

// structure that holds the local info on a node
struct nodeinfo {
	char *hostname; 	// hostname of the machine
	char *keynode;		// node that introduced this node to the cluster
	char *internalhost;	// local ip of node
	char *externalhost;	// public facing ip of the node
	char *identifier;	// unique identifier
	char *neighbour[2];	// nodes that this node talks to
	char *command;		// daust option request for other machine
	time_t timestamp;
};

// node list item struct
struct nli {
	struct nodeinfo *info;
	struct nli *prev;
	struct nli *next;
};

extern char na[];
struct nli *add_node_to_list(struct nli *node);
struct nli *create_nodelist();
struct nodeinfo *create_node();
char *set_node_element(char **el, char *buf);
void log_nodelist(struct nli *node);

#endif
