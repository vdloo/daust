/* serialize.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "serialize.h"
#include "node_data.h"
#include "utils.h"

char st[] = "|start_of_block|";
char ed[] = "|end_of_block|";
char dl[] = "\n";

// serializes from pointer to item, to end of list. 
// returns pointer to new buffer.
char *serialize(struct nli *node)
{
	char *buf;
	buf = NULL;
	if (node) {
		struct nodeinfo *nfo;
		int m_siz = 0;
		int *mp = &m_siz;

		buf = asdtobfp(buf, mp, st, dl);
		do
		{
			nfo = node->info;
			buf = asdtobfp(buf, mp, st, dl);
			buf = asdtobfp(buf, mp, nfo->hostname, dl);
			buf = asdtobfp(buf, mp, nfo->keynode, dl);
			buf = asdtobfp(buf, mp, nfo->internalhost, dl);
			buf = asdtobfp(buf, mp, nfo->externalhost, dl);
			buf = asdtobfp(buf, mp, nfo->identifier, dl);
			buf = asdtobfp(buf, mp, nfo->command, dl);
			buf = asdtobfp(buf, mp, ed, dl);
		} while (node = node->next);
		buf = asdtobfp(buf, mp, ed, '\0');
	}
	return buf;
}

// deserializes buffer to nodeinfo linked list.
// returns pointer to new list.
struct nli *deserialize(char *buf)
{
	if (!buf) return NULL;
	struct nli *np = NULL, *node = NULL;
	struct nodeinfo *nfo;

	int l = strlen(buf);
	int nest = 0, el = 0;
	char *sg = strtok(buf, dl);
	char *hn, *kn, *ih, *eh, *id, *cd;
	do {
		if (strstr(sg, st)) {
			el = 0;
			if (nest < 2) ++nest;	
		}
		if (strstr(sg, ed)) {
			if (nest == 2) {
				node->info = create_node();
				nfo = node->info;
				set_node_element(&nfo->hostname,	hn);
				set_node_element(&nfo->keynode, 	kn);
				set_node_element(&nfo->internalhost, 	ih);
				set_node_element(&nfo->externalhost, 	eh);
				set_node_element(&nfo->identifier, 	id);
				set_node_element(&nfo->command, 	cd);
			}
		       	if (nest > 0) --nest;
		}
		sg = strtok(NULL, dl);
		if (nest == 2) {
			switch(el) {
				case 0: 
					if (node) {
						node = add_node_to_list(node);
					} else {
						np = create_nodelist();
						node = np;
					}
					hn = strdup(sg);
					break;
				case 1: 
					kn = strdup(sg);
					break;
				case 2: 
					ih = strdup(sg);
					break;
				case 3:
					eh = strdup(sg);
					break;
				case 4:
					id = strdup(sg);
					break;
				case 5:
					cd = strdup(sg);
					break;
			}
			el++;
		}
	} while (sg);
	if (hn) free(hn);
	if (kn) free(kn);
	if (ih) free(ih);
	if (eh) free(eh);
	if (id) free(id);
	if (cd) free(cd);
	return np;
}
