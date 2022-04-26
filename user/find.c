#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void find(char *path, char *term)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  // Open the directory for reading.
  if ((fd = open(path, 0)) < 0)
  {
    fprintf(2, "find: cannot open current path %s\n", path);
    return;
  }

  // Get stats on the directory.
  if (fstat(fd, &st) < 0)
  {
    fprintf(2, "find: cannot get stats on current path %s\n", path);
    close(fd);
    return;
  }

  // Check if path is too long
  if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf)
  {
    fprintf(2, "find: path too long\n");
    close(fd);
    return;
  }

  strcpy(buf, path);
  p = buf + strlen(buf);
  *p++ = '/';
  while (read(fd, &de, sizeof(de)) == sizeof(de))
  {
    memmove(p, de.name, DIRSIZ);
    if (stat(buf, &st) < 0)
    {
      fprintf(2, "find: cannot stat %s\n", buf);
      continue;
    }
    // if de.name is empty skip
    if (de.name[0] == 0)
      continue;
    if (st.type == T_DIR)
    {
      // Found a directory, recurse.
      if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
        // Ignore . and .. to avoid infinite loops.
        continue;
      find(buf, term);
    }
    // Check if the file name contains the search term.
    if (strstr(de.name, term) != 0)
    {
      printf("%d %s\n", st.type, buf);
    }
  }

  // Close the directory.
  close(fd);
}

int main(int argc, char *argv[])
{
  int i;

  if (argc < 2)
  {
    fprintf(2, "usage: find path\n");
    exit(0);
  }

  for (i = 1; i < argc; i++)
  {
    find(".", argv[i]);
  }
  exit(0);
}
