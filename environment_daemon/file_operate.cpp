#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "file_operate.h"

path_stat
is_path_exists(const char *path)
{
  path_stat result = PATH_NOTFOUND;
  int rc;
  struct stat st;

  rc = stat(path, &st);
  if (rc == 0) {
    if (S_ISREG(st.st_mode)) {
      result = PATH_FILE;
    } else if (S_ISDIR(st.st_mode)) {
      result = PATH_DIRECTORY;
    } else if (S_ISCHR(st.st_mode)) {
      result = PATH_CHARDEV;
    } else if (S_ISBLK(st.st_mode)) {
      result = PATH_BLKDEV;
    } else if (S_ISFIFO(st.st_mode)) {
      result = PATH_FIFO;
    }
  }

  return result;
}

int
mkdirp(const char *path)
{
  int ret = -1;
  char buf_path[MAX_PATH_LEN] = {0};
  char work_str[MAX_PATH_LEN] = {0};
  char *saveptr, *str1, *token;
  mode_t dirperm = S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH;  /* permission 0755 */

  path_stat ps;

  strncpy(work_str, path, MAX_PATH_LEN);

  for (str1 = work_str; ; str1 = NULL) {
    token = strtok_r(str1, "/", &saveptr);
    if (token == NULL) {
      break;
    }

    size_t remain = MAX_PATH_LEN - strlen(buf_path) - 1;
    if (strlen(buf_path) > 0 || *path == '/') {
      strncat(buf_path, "/", remain);
      remain--;  /* length of "/" */
    }
    strncat(buf_path, token, remain);
    ps = is_path_exists(buf_path);

    if (ps == PATH_DIRECTORY) {
      ret = 0;  /* success */
    } else if (ps == PATH_NOTFOUND) {
      ret = mkdir (buf_path, dirperm);
      if (ret < 0) {
        fprintf(stderr, "make directory %s failed - %s(%d)\n", buf_path, strerror(errno), errno);
        break;
      }
    }
  }

  return ret;
}

int
copy(const char *src, const char *dst)
{
  FILE *fin, *fout;
  char buf[4096] = {0};
  int ret = -1;
  size_t read_len, write_len;

  if ((fin = fopen(src, "r")) == NULL) {
    fprintf(stderr, "failed copy %s read open failed - %s(%d)\n", src, strerror(errno), errno);
    return ret;
  }
  if ((fout = fopen(dst, "w")) == NULL) {
    fclose(fin);
    fprintf(stderr, "failed copy %s write open failed - %s(%d)\n", dst, strerror(errno), errno);
    return ret;
  }

  while ((read_len = fread(buf, sizeof(char), sizeof(buf), fin)) > 0) {
    write_len = 0;
    do {
      write_len += fwrite(buf + write_len, sizeof(char), read_len - write_len, fout);
      if (ferror(fout) != 0) {
        fprintf(stderr, "failed copy %s write data failed - %s(%d)\n", dst, strerror(errno), errno);
        goto end;
      } 
    } while (read_len - write_len > 0);
    memset(buf, '\0', sizeof(buf));
  }
  if (ferror(fin) != 0) {
    fprintf(stderr, "failed copy %s read data failed - %s(%d)\n", src, strerror(errno), errno);
    goto end;
  } 

  ret = 0;

end:
  if (fin) {
    fclose(fin);
  }
  if (fout) {
    fclose(fout);
  }

  return ret;
}

void
get_dirname(char **dirname, const char *path)
{
  const char *s;
  *dirname = NULL;

  s = path + strlen(path);
  while(path < s && *s != '/') {
    s--;
  }

  if (path < s) {
    *dirname = strndup(path, (size_t)(s - path));
  }
}
