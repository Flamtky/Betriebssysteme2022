#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char *fmtname(char *path) {
  static char buf[DIRSIZ + 1];
  char *p;

  // Find first character after last slash.
  for (p = path + strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if (strlen(p) >= DIRSIZ) return p;
  memmove(buf, p, strlen(p));
  memset(buf + strlen(p), ' ', DIRSIZ - strlen(p));
  return buf;
}

void printPermission(struct stat *s) {
  if (s->type == T_DIR)
    printf("d");
  else if (s->type == T_SYMLINK)
    printf("l");
  else
    printf("-");
  if (s->mode & P_RUSR)
    printf("r");
  else
    printf("-");
  if (s->mode & P_WUSR)
    printf("w");
  else
    printf("-");
  if (s->mode & P_XUSR)
    printf("x");
  else
    printf("-");
  if (s->mode & P_RGRP)
    printf("r");
  else
    printf("-");
  if (s->mode & P_WGRP)
    printf("w");
  else
    printf("-");
  if (s->mode & P_XGRP)
    printf("x");
  else
    printf("-");
  if (s->mode & P_ROTH)
    printf("r");
  else
    printf("-");
  if (s->mode & P_WOTH)
    printf("w");
  else
    printf("-");
  if (s->mode & P_XOTH)
    printf("x");
  else
    printf("-");
  printf(" ");
  // owner and group
  printf("%d %d ", s->uid, s->gid);
}

void ls(char *path) {
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if ((fd = open(path, 0)) < 0) {
    fprintf(2, "ls: cannot open %s\n", path);
    return;
  }

  if (fstat(fd, &st) < 0) {
    fprintf(2, "ls: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch (st.type) {
    case T_FILE:
      printPermission(&st);
      printf("%s %d %d %l\n", fmtname(path), st.type, st.ino, st.size);
      break;

    case T_DIR:
      if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) {
        printf("ls: path too long\n");
        break;
      }
      strcpy(buf, path);
      p = buf + strlen(buf);
      *p++ = '/';
      while (read(fd, &de, sizeof(de)) == sizeof(de)) {
        if (de.inum == 0) continue;
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;
        if (stat(buf, &st) < 0) {
          printf("ls: cannot stat %s\n", buf);
          continue;
        }
        printPermission(&st);
        printf("%s %d %d %d\n", fmtname(buf), st.type, st.ino, st.size);
      }
      break;
  }
  close(fd);
}

int main(int argc, char *argv[]) {
  int i;

  if (argc < 2) {
    ls(".");
    exit(0);
  }
  for (i = 1; i < argc; i++) ls(argv[i]);
  exit(0);
}
