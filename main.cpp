//#define DEBUG
//#define SUD

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include <f_reader.h>
#include <vec.h>
#include <clause.h>
#include <solver.h>
#include <prep.h>
#include <sudoku.h>
#include <utils.h>

int main(int argc, char **argv)
{
    srand(unsigned(time(NULL)));
    if (argc == 1) {
        printf("Usage:\n");
        printf("This program has two functions.\n");
        printf("1. Solve boolean satisfiability problem (SAT).\n");
        printf("2. Generate and solve binary sudoku puzzle.\n\n");
        printf("1. To solve SAT problem.\n");
        printf("    For example: the executable file is [program-name] and the cnf file is xxxx.cnf\n\n");
        printf("2. To solve puzzle.\n");
        printf("    The program automatically generates a unique-answer binary sudoku puzzle\n");
        printf("    and solve it. The usage is given as follow. The default size of the generated\n");
        printf("    binary puzzle is 8 times 8. You can specify the size by given the N\n");
        printf("    parameter according to follwing instruction. Notice: the large N (>=14) will cost\n");
        printf("    relatively long time to generate, but solving it is still relatively quick.\n\n");
        printf("Example:\n");
        printf("    1. Solving cnf:\n");
        printf("              [program-name] cnf xxxx.cnf\n");
        printf("    2. Generate unique answer sudoku and solve:\n");
        printf("       Sudoku size N x N\n");
        printf("              [program-name] sud N (N can be omitted)\n");
        return 0;
    }
    
    if (strcmp(argv[1], "cnf") == 0) {
        assert(argc >= 3);
        clock_t t1 = clock();
        Formula F;
        readcnf(argv[2], F);
        
        char *output_f = (char*) malloc (sizeof(char)*100);
        strcpy(output_f, argv[2]);
        int f_name_len = strlen(argv[2]);
        output_f[f_name_len-3] = 'r';
        output_f[f_name_len-2] = 'e';
        output_f[f_name_len-1] = 's';
        freopen(output_f, "w", stdout);
        
        status result = preprocess(F);
        Solver S(F);
        if (result == ok) result = S.solve();

        clock_t t2 = clock();
        
        if (result == ok) {
            printf("s 1\n");
        }
        else {
            assert(result == conflict);
            printf("s 0\n");
        }
        S.show();
        double dur = 1000.0*(t2-t1)/CLOCKS_PER_SEC;
        printf("t %.2f\n", dur);
        
        fclose(stdout);
        free(output_f);
    }
    else {
        assert(strcmp(argv[1], "sud") == 0);
        int n = 8;
        if (argc >= 3) n = atoi(argv[2]);
        srand(unsigned(time(NULL)));
        Sudoku So(n);
        So.gen();
        So.solve(So);
        So.print();
    }
    return 0;
}
