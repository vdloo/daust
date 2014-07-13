/* utils.c */
#include <stdlib.h>
#include <string.h>

// append string to buffer from position mp
char *astobfp(char *buf, int *mp, char *str)
{
	int p_siz 	= *mp;
	if (str) {
		int strl 	= strlen(str); 
		*mp = *mp + (strl * sizeof(char));
		buf = realloc(buf, *mp); 
		memcpy(buf + p_siz, str, strl * sizeof(char));
	} else {
		*mp = *mp + sizeof(char);
		buf[p_siz] = '\0'; // p_siz == *mp - 1
	}
	return buf;
}

// append string and delimiter to buffer from position mp
char *asdtobfp(char *buf, int *mp, char *str1, char *str2)
{
	buf = astobfp(buf, mp, str1);
	buf = astobfp(buf, mp, str2);
	return buf;
}

int count_delim(char *buf, char *dl)
{
	int i 		= 0;
	if (buf && dl != NULL) {
		char *sg = buf;
		while ((sg = strpbrk(sg, " ")) != NULL){
			++sg;
			++i;
		}
	}
	return i;
}

char **array_of_segments(char *buf, char *dl, int i)
{
	char *sg 	= NULL;
	char **a;
	int j = 0;
	a		= malloc(i * sizeof(char *));
	sg		= strtok(buf, dl);
	do {
		if (j < i) {
			a[j] = strdup(sg);
		}
		++j;
	} while (sg = strtok(NULL, dl));
	return a;
}

// returns pointer to string array containing pieces
char **explode(char *buf, char *dl, int *acp)
{
	int i = count_delim(buf, dl);
	*acp = i;
	return array_of_segments(buf, dl, i);
}
