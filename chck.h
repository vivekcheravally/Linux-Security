#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *fp1;

// Function to remove all spaces from a given string
void removeSpaces(char *str)
{
    // To keep track of non-space character count
    int count = 0;
    int i;
    // Traverse the given string. If current character
    // is not space, then place it at index 'count++'
    for (i = 0; str[i]; i++)
        if (str[i] != ' ')
            str[count++] = str[i]; // here count is
                                   // incremented
    str[count] = '\0';
}

/*int token(char *str1)
{
   char *perm;
   fp1=fopen("put.txt","a");
   perm = (char *) malloc (sizeof (char)*20);
   /* get the second token */
   /*strtok(str1, "></");
   strcpy(perm, strtok(NULL , "<"));
   printf("%s\n", perm);
   fprintf(fp1,"%s\n", perm);
   return(0);
}

/*int main(void)
{
    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    char *word= "permission";
    fp = fopen("rsf.xml", "r");
    
    if (fp == NULL)
        exit(EXIT_FAILURE);

    while ((read = getline(&line, &len, fp)) != -1) {
        if (strstr(line, word) != NULL){
            removeSpaces(line);
            token(line);
            //printf("%s", line);
            }
    }

    free(line);
    exit(EXIT_SUCCESS);
}
*/
