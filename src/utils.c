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

// copy the haystack into a new block of memory and 
// replace the needle with the replace value and return
// the pointer
char *str_replace(char *needle, char *replace, char *haystack)
{
	char *r = NULL;
	char *p = NULL;
	if (p = strstr(haystack, needle)) {
		int strlenneedle = strlen(needle);
		int strlenreplace = strlen(replace);

		int neelen 	= strlen(needle);
		int prelen 	= p - haystack; // characters until needle
		int replen 	= strlen(replace);
		// characters from needle until nullterm of haystack
		int poslen 	= haystack + strlen(haystack) - p - neelen;

		int hl  	= prelen + replen + poslen + 1;

		r = malloc(hl * sizeof(char));
		int strlenhay = strlen(haystack);
		memcpy(r, haystack, prelen);
		memcpy(r + prelen, replace, replen);
		memcpy(r + prelen + replen, haystack + prelen + neelen, poslen);
		r[hl - 1] = '\0'; 
	}
	return r;
}

char *str_replace_all(char *needle, char *replace, char *haystack)
{
	char *buf = NULL;
	char *str = strdup(haystack);
	while (buf = str_replace(needle, replace, str)){
		if (str) free(str);
		str = buf;
	}
	return str;
}
