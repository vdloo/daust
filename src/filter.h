/* filter.h */
#ifndef FILTER_H
#define FILTER_H
char *reconstruct_command(int w, char *rmt, char *command);
char *filter_specified_remote(int ac, char *av[], int o);
int filter_specified_all(int ac, char *av[], int o);
char *filter_command(int ac, char *av[], int o);
int filter_who(int ac, char **av, int o, char *rmt);
char *sanitize_command(int ac, char **av, int o);
#endif
