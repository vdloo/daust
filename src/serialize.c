/* serialize.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "serialize.h"
#include "node_data.h"
#include "utils.h"

char st[] 	= "|_start_of_block_|";
char ed[] 	= "|_end_of_block_|";
char dl[] 	= "\n";
char dlr[] 	= "|_NEWLINE_|";

// serializes from pointer to item, to end of list. 
// returns pointer to new buffer.
char *serialize(struct nli *node)
{
	if (!node) return NULL;
	char *z = nodelist_list(node);
	free(z);
	char *buf;
	buf = NULL;
	if (node) {
		struct nodeinfo *nfo;
		int m_siz = 0;
		int *mp = &m_siz;

		char *hn, *kn, *ih, *eh, *cm;
		buf = asdtobfp(buf, mp, st, dl);
		do
		{
			nfo = node->info;
			buf = asdtobfp(buf, mp, st, dl);

			hn = str_replace_all(dl, dlr, nfo->hostname);
			buf = asdtobfp(buf, mp, hn, dl);
			free(hn);

			kn = str_replace_all(dl, dlr, nfo->keynode);
			buf = asdtobfp(buf, mp, kn, dl);
			free(kn);

			ih = str_replace_all(dl, dlr, nfo->internalhost);
			buf = asdtobfp(buf, mp, ih, dl);
			free(ih);

			eh = str_replace_all(dl, dlr, nfo->externalhost);
			buf = asdtobfp(buf, mp, eh, dl);
			free(eh);

			buf = asdtobfp(buf, mp, nfo->identifier, dl);

			cm = str_replace_all(dl, dlr, nfo->command);
			buf = asdtobfp(buf, mp, cm, dl);
			free(cm);

			buf = asdtobfp(buf, mp, nfo->unique, dl);

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
	char *hn, *kn, *ih, *eh, *id, *cd, *uq;
	hn = kn = ih = eh = id = cd = uq = NULL;

	do {
		if (strstr(sg, st)) {
			el = 0;
			if (nest < 2) ++nest;	
		}
		if (strstr(sg, ed)) {
			if (nest == 2) {
				nfo = node->info;
				set_node_element(&nfo->hostname,	hn);
				set_node_element(&nfo->keynode, 	kn);
				set_node_element(&nfo->internalhost, 	ih);
				set_node_element(&nfo->externalhost, 	eh);
				set_node_element(&nfo->identifier, 	id);
				set_node_element(&nfo->command, 	cd);
				set_node_element(&nfo->unique, 		uq);
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
					node->info = create_node();
					if (hn) free(hn);
					hn = str_replace_all(dlr, dl, sg);
					break;
				case 1: 
					if (kn) free(kn);
					kn = str_replace_all(dlr, dl, sg);
					break;
				case 2: 
					if (ih) free(ih);
					ih = str_replace_all(dlr, dl, sg);
					break;
				case 3:
					if (eh) free(eh);
					eh = str_replace_all(dlr, dl, sg);
					break;
				case 4:
					if (id) free(id);
					id = sg ? strdup(sg) : sg;
					break;
				case 5:
					if (cd) free(cd);
					cd = str_replace_all(dlr, dl, sg);
					break;
				case 6:
					if (uq) free(uq);
					uq = sg ? strdup(sg) : sg;
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
	if (uq) free(uq);
	return np;
}
