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
