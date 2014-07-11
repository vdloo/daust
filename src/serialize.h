/* serialize.h */
#ifndef SERIALIZE_H
#define SERIALIZE_H
#include "node_data.h"

char *serialize(struct nli *node);
struct nli *deserialize(char *buf);

#endif
