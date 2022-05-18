#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h> 
#include <errno.h>


typedef struct {
    char** branches;
    int n_branches;
} BRANCHES;

void no_git() {
    printf("Git not installed\n");
    exit(1);
}

void no_repo() {
    printf("Not found Git repository in the current directory\n");
    exit(1);
}

BRANCHES* get_branches()
{
    char stout[1024];
    char cmd[] = "type git | which git";
    struct dirent *dir;
    char **choices;

    //Check git
    FILE *fp = popen(cmd, "r");
    if (fp == NULL) {
      printf("Failed to check git binary\n");
      exit(1);
    }
    if (fgets(stout, sizeof(stout), fp) != NULL) {
        pclose(fp);
        if (strstr(stout, "not") != NULL) {
            no_git();
        }
    }
    else {
        pclose(fp);
        no_git();
    }

    // Get branches
    BRANCHES* res = (BRANCHES*)malloc(sizeof(BRANCHES));
    DIR *d = opendir("./.git/refs/heads");
    int br = 0;
    if (d != NULL) {
        choices = (char**)malloc(1 * sizeof(char*));
            while ((dir = readdir(d)) != NULL) {
              if (br > 0) {
                  choices = (char**)realloc(choices, (br+1)*sizeof(char*));
              }
              if (strlen(dir->d_name) > 0 && dir->d_name[0] != '.') {
                choices[br] = (char*)calloc(strlen(dir->d_name) + 1, sizeof(char));
                strncpy(choices[br], dir->d_name, strlen(dir->d_name));
                br++;
              }
            }
            closedir(d);
    }
    else no_repo();

    res->branches = choices;
    res->n_branches = br;
    return res;
}

void switch_branch(BRANCHES *bs, int choice) {
    if (choice < bs->n_branches) {
      char *branch = (bs->branches)[choice];
      char cmd[1024];
      FILE *fp;

      sprintf(cmd, "git checkout %s", branch);
      fp = popen(cmd, "r");
      if (fp == NULL) {
        printf("Error switching branch");
        exit(1);
      }
      pclose(fp);
    }
    else {
        printf("Branch number out of range. Please enter a number in [1, %d]\n", bs->n_branches);
    }
}

int main(int argc, char** argv) {
    int i;
    char input[1024], *endptr;
    BRANCHES *bs = get_branches();
    
    if (bs->n_branches > 0) {
      for(i=0; i<bs->n_branches; i++) {
        printf("%d: %s\n", i+1, (bs->branches)[i]);
      }

      printf("Select branch number: ");
      fscanf(stdin, "%s", input);

      printf("Selected branch: %s\n", input);
      long branch_number = strtol(input, &endptr, 10);
      if (errno != 0) {
        perror("strtol");
        exit(EXIT_FAILURE);
      }

      if (endptr == input) {
        fprintf(stderr, "No valid branch number. Introduce a number.\n");
        exit(EXIT_FAILURE);
      }

      switch_branch(bs, branch_number - 1);
    }
    else {
        printf("No branches found\n");
    }
    
    // Free memomry
    for(i=0; i<bs->n_branches; i++) {
      free(bs->branches[i]);
    }
    free(bs->branches);
    free(bs);
    bs = NULL;
}

