/* serialize.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "serialize.h"
#include "node_data.h"

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
			buf = append_to_buf(buf, m_sp, nfo->identifier);
			buf = append_to_buf(buf, m_sp, nfo->command);
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
					set_node_element(&hn, sg);
					break;
				case 1: 
					set_node_element(&kn, sg);
					break;
				case 2: 
					set_node_element(&ih, sg);
					break;
				case 3:
					set_node_element(&eh, sg);
					break;
				case 4:
					set_node_element(&id, sg);
					break;
				case 5:
					set_node_element(&cd, sg);
					break;
			}
			el++;
		}
	} while (sg);
	return np;
}
