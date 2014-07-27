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
		buf = realloc(buf, *mp); 
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
	int i 		= 1;
	int lb = strlen(buf);
	buf[lb - 1] = (buf[lb - 1] == ' ') ? '\0' : buf[lb - 1];
	if (buf && dl != NULL) {
		char *cbuf = strdup(buf);
		char *sg   = cbuf;
		while ((sg = strpbrk(sg, " ")) != NULL){
			++sg;
			++i;
		}
		if (cbuf) free(cbuf);
	}
	return i;
}

char **array_of_segments(char *buf, char *dl, int i)
{
	char *sg 	= NULL;
	char **a;
	int j = 0;
	char *cbuf	= strdup(buf);
	a		= malloc(i * sizeof(char *));
	sg		= strtok(cbuf, dl);
	do {
		if (j < i) {
			a[j] = strdup(sg);
		}
		++j;
	} while (sg = strtok(NULL, dl));
	if (cbuf) free(cbuf);
	return a;
}

// returns pointer to string array containing pieces
char **explode(char *buf, char *dl, int *acp)
{
	int i = count_delim(buf, dl);
	*acp = i;
	return array_of_segments(buf, dl, i);
}

void destroy_array(char **av, int ac)
{
	int i;
	char *el;
	if (av) {
		for (i = 0; i < ac; i++) {
			el = av[i];
			if (el) free(el);
		}
		free(av);
	}
}

int count_occurence(char *needle, char *haystack)
{
	int nlen = strlen(needle);
	int i = 0;
	int offset = 0;
	if (nlen) {
		// count the occurences of the needle by using strstr, 
		// skip ahead the length of the needle every match
		while (haystack = strstr(haystack + offset, needle)) {
			++i;
			if (i) offset = nlen;
		}
	}
	return i;
}

char *str_replace(char *needle, char *replace, char *haystack)
{
	if (!haystack) return NULL;
	char *match 	= NULL;
	char *r 	= NULL;

	int dif = 0;
	int talloc = 1;
	int nlen = strlen(needle);
	int rlen = strlen(replace);
	char *ohp = haystack + strlen(haystack);
	int prev = 0;
	while (haystack && (match = strstr(haystack, needle))) {
		dif = match - haystack;
		talloc = dif + rlen + talloc;
		r = realloc(r, talloc);	
		memcpy(r + prev, haystack, dif);
		memcpy(r + dif + prev, replace, rlen);
		prev = rlen + dif + prev;
		haystack = match + nlen;
		haystack = haystack < ohp ? haystack : NULL;
	}
	if (haystack) {
		int res = strlen(haystack);
		talloc = res + talloc;
		r = realloc(r, talloc);
		memcpy(r + prev, haystack, res);
	}
	r[talloc - 1] 	= '\0';
	return r;
	
}




























