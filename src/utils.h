/* utils.h */
#ifndef UTILS_H
#define UTILS_H
char *astobfp(char *buf, int *mp, char *str);
char *asdtobfp(char *buf, int *mp, char *str1, char *str2);
char **explode(char *buf, char *dl, int *acp);
void destroy_array(char **av, int ac);
#endif
