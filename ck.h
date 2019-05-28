#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <dirent.h>
#include <libgen.h>
 
/* checks if the string is purely an integer
 * we can do it with `strtol' also
 */
int check_if_number (char *str)
{
  int i;
  for (i=0; str[i] != '\0'; i++)
  {
    if (!isdigit (str[i]))
    {
      return 0;
    }
  }
  return 1;
}
 
#define MAX_BUF 1024
#define PID_LIST_BLOCK 32
 
int *pidof (char *pname)
{
  DIR *dirp;
  FILE *fp;
  struct dirent *entry;
  int *pidlist, pidlist_index = 0, pidlist_realloc_count = 1;
  char path1[MAX_BUF], read_buf[MAX_BUF];
 
  dirp = opendir ("/proc/");
  if (dirp == NULL)
  {
    perror ("Fail");
    return NULL;
  }
 
  pidlist = malloc (sizeof (int) * PID_LIST_BLOCK);
  if (pidlist == NULL)
  {
    return NULL;
  }
 
  while ((entry = readdir (dirp)) != NULL)
  {
    if (check_if_number (entry->d_name))
    {
      strcpy (path1, "/proc/");
      strcat (path1, entry->d_name);
      strcat (path1, "/comm");
 
      /* A file may not exist, it may have been removed.
       * dut to termination of the process. Actually we need to
       * make sure the error is actually file does not exist to
       * be accurate.
       */
      fp = fopen (path1, "r");
      if (fp != NULL)
      {
        fscanf (fp, "%s", read_buf);
        if (strcmp (read_buf, pname) == 0)
        {
          /* add to list and expand list if needed */
          pidlist[pidlist_index++] = atoi (entry->d_name);
          if (pidlist_index == PID_LIST_BLOCK * pidlist_realloc_count)
          {
            pidlist_realloc_count++;
            pidlist = realloc (pidlist, sizeof (int) * PID_LIST_BLOCK * pidlist_realloc_count); //Error check todo
            if (pidlist == NULL)
            {
              return NULL;
            }
          }
        }
        fclose (fp);
      }
    }
  }
 
 
  closedir (dirp);
  pidlist[pidlist_index] = -1; /* indicates end of list */
  return pidlist;
}

/*int function (char *path)
{
  int *list, i;
 
  if (argc != 2)
  {
    printf ("Usage: %s proc_name\n", path);
    return 0;
  }
  list = pidof (path);
  for (i=0; list[i] != -1; i++)
  {
    printf ("%d ", list[i]);
  }
  free (list);
  if (list[0] != -1)
  {
    printf ("\n");
  }
  return 0;
}*/
 
/*int main (int argc, char *argv[])
{
  int *list, i;
 
  if (argc != 2)
  {
    printf ("Usage: %s proc_name\n", argv[0]);
    return 0;
  }
  list = pidof (argv[1]);
  for (i=0; list[i] != -1; i++)
  {
    printf ("%d ", list[i]);
  }
  free (list);
  if (list[0] != -1)
  {
    printf ("\n");
  }
  return 0;
}*/
