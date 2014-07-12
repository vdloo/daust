/* utils.c */
#include <stdlib.h>
#include <string.h>

// adds string to buffer from position m_sp separated by delimiter dl
char *append_to_buf(char *buf, int *m_sp, char *str, char *dl)
{
	int p_siz 	= *m_sp;
	int strl 	= strlen(str); 
	int dll		= strlen(dl);
	*m_sp = *m_sp + (strl + dll) * sizeof(char);
	buf = realloc(buf, *m_sp);
	memcpy(buf + p_siz, str, strl * sizeof(char));
	if (dl) {
		memcpy(buf + p_siz + strl, dl, dll * sizeof(char));
	}
	return buf;
}
