#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <f_reader.h>
#include <vec.h>
#include <clause.h>
#include <solver.h>
#include <prep.h>
#include <sys/time.h>

int main(int argc, char **argv)
{
    Formula F;
    char *input_f = (char*) malloc (sizeof(char)*100);
    strcpy(input_f, argv[1]);
    //strcpy(input_f, "/home/cyh/Desktop/test/satisfied/S/problem9-100.cnf");
    readcnf(input_f, F);
    printf("%s\n", input_f);
    fflush(stdout);
    readcnf(input_f, F);
    char *output_f = (char*) malloc (sizeof(char)*100);
    strcpy(output_f, input_f);
    int f_name_len = strlen(input_f);
    output_f[f_name_len-3] = 'r';
    output_f[f_name_len-2] = 'e';
    output_f[f_name_len-1] = 's';
    printf("%s\n", output_f);
    FILE *p = fopen(output_f, "r");
    if (p == NULL) {
        printf("Bad structure!\n");
        return 0;
    }
    char ch;
    int x;
    
    char *buf = (char*)malloc(sizeof(char) * 300);
    fgets(buf, 300, p);
    free(buf);
    fscanf(p, "%c", &ch);
    if (ch != 'v') {
        printf("Bad structure!\n");
        return 0;
    }
    
    for(int i = 1; i <= F.varN(); i++) {
        fscanf(p, "%d", &x);
        //printf("%d %d\n", x, i);
        if (x == i) F.assign[i] = utru;
        else if (x == -i) F.assign[i] = ufal;
        assert(F.assign[i] != und);
    }
    printf("%d\n", F.compute() == ok ? 1 : 0);
    fclose(p);
    free(output_f);
    free(input_f);
    return 0;
}
