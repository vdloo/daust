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
	time_t timestamp;
};

// node list item struct
struct nli {
	struct nodeinfo *info;
	struct nli *prev;
	struct nli *next;
};

struct nli *create_nodelist();
struct nodeinfo *create_node();
char *set_node_element(char **el, char *buf);
void log_nodelist(struct nli *node);
char *serialize(struct nli *node);
struct nli *deserialize(char *buf);

#endif
