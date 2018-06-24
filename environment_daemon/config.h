#ifndef __INCLUDE_CONFIG_H__
#define __INCLUDE_CONFIG_H__

typedef struct daemon_config {
  const char *logdir;
  int  span;
} daemon_config_t;

int get_option(int argc, char *argv[], daemon_config_t *conf, int *extopt);

#endif
