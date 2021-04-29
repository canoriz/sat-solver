#ifndef _SUDOKU_H
#define _SUDOKU_H
#include <vec.h>
#include <utils.h>
#include <formula.h>
#include <prep.h>
#include <solver.h>

struct blk {
    int x, y, v;
};

/* Transfer (x,y) in a N x N sudoku to a var number */
inline int blkToId(int n, int x, int y)
{
    return x * n + y + 1;
}

inline void AconjB(Clause& cla1, Clause &cla2, Clause &cla3, var c, var a, var b)
{
    // C = A and B
    // equals: A or not C, b or not C, not A or not B or C
    Clause tmp1, tmp2, tmp3;
    tmp1.addVar(Lit(a));
    tmp1.addVar(Lit(-c));
    tmp2.addVar(Lit(b));
    tmp2.addVar(Lit(-c));
    tmp3.addVar(Lit(c));
    tmp3.addVar(Lit(-a));
    tmp3.addVar(Lit(-b));
    tmp1.moveTo(cla1);
    tmp2.moveTo(cla2);
    tmp3.moveTo(cla3);
}

class Sudoku {
public:
    Vec<Vec<int> > S;
    
public:
    Sudoku(int n);
    void print();
    int check();
    void rnd();
    void gen();
    blk dig();
    int toCNF(Formula &outF);
    int fromCNF(Formula &outF);
    int solve(Sudoku &AnsSu);
};

/* Generate a N x N sudoku
 * n must be even */
Sudoku::Sudoku(int n)
{
    S.resize(n);
    for(int i = 0; i < n; i++) {
        S[i].resize(n);
    }
    
    do {
        rnd();
    } while(0);
}
void Sudoku::gen()
{
    printf("Generating unique answer sudoku...\n");
    do {
        rnd();
    } while(!solve(*this));
    
    int n = S.size();
    int i = 1;
    int fail_cnt = 0;
    while (fail_cnt < 2) {
        blk b = dig();
        if (b.v == 1) S[b.x][b.y] = 0;
        else {
            assert(b.v == 0);
            S[b.x][b.y] = 1;
        }
        Sudoku AnsSu(n);
        if (!solve(AnsSu)) {
            S[b.x][b.y] = 2;
            i++;
        }
        else {
            fail_cnt++;
            S[b.x][b.y] = b.v;
        }
    }
    printf("Generated!\n");
    print();
}

/* Generate a random sudoku
 * May not satisfy requirements */
void Sudoku::rnd()
{
    int n = S.size();
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < n; j++) {
            S[i][j] = random(2);
        }
    }
    for(int i = 1; i <= n*n*5/6; i++) {
        dig();
    }
}

// Dig one position in the sudoku
blk Sudoku::dig()
{
    int n = S.size();
    int a, b;
    do {
        int p = random(n*n);
        a = p / n, b = p % n;
    } while(S[a][b] == 2);
    int v = S[a][b];
    S[a][b] = 2;
    return (blk){ a, b, v };
}

int Sudoku::toCNF(Formula &outF)
{
    int n = S.size();
    int varTotal = n * n * (3*n - 2);
    Formula(varTotal).moveTo(outF);
    
    /* Adding single literals */
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < n; j++) {
            int tmpVar = blkToId(n, i, j);
            Clause tmpCla;
            Lit tmpLit;
            if (S[i][j] == 0) {
                tmpLit = Lit(-tmpVar);
                tmpCla.addVar(tmpLit);
                outF.addClause(tmpCla);
            }
            else if (S[i][j] == 1) {
                tmpLit = Lit(tmpVar);
                tmpCla.addVar(tmpLit);
                outF.addClause(tmpCla);
            }
        }
    }
    
    /* No continiously 3 1 or 0s */
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < n-2; j++) {
            Clause tmpCla;
            Lit tmpLit;
            tmpLit = Lit(blkToId(n, i, j));
            tmpCla.addVar(tmpLit);
            tmpLit = Lit(blkToId(n, i, j+1));
            tmpCla.addVar(tmpLit);
            tmpLit = Lit(blkToId(n, i, j+2));
            tmpCla.addVar(tmpLit);
            //tmpCla.show();
            outF.addClause(tmpCla);
        }
    }
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < n-2; j++) {
            Clause tmpCla;
            Lit tmpLit;
            tmpLit = Lit(-blkToId(n, i, j));
            tmpCla.addVar(tmpLit);
            tmpLit = Lit(-blkToId(n, i, j+1));
            tmpCla.addVar(tmpLit);
            tmpLit = Lit(-blkToId(n, i, j+2));
            tmpCla.addVar(tmpLit);
            //tmpCla.show();
            outF.addClause(tmpCla);
        }
    }
    for(int j = 0; j < n; j++) {
        for(int i = 0; i < n-2; i++) {
            Clause tmpCla;
            Lit tmpLit;
            tmpLit = Lit(blkToId(n, i, j));
            tmpCla.addVar(tmpLit);
            tmpLit = Lit(blkToId(n, i+1, j));
            tmpCla.addVar(tmpLit);
            tmpLit = Lit(blkToId(n, i+2, j));
            tmpCla.addVar(tmpLit);
            //tmpCla.show();
            outF.addClause(tmpCla);
        }
    }
    for(int j = 0; j < n; j++) {
        for(int i = 0; i < n-2; i++) {
            Clause tmpCla;
            Lit tmpLit;
            tmpLit = Lit(-blkToId(n, i, j));
            tmpCla.addVar(tmpLit);
            tmpLit = Lit(-blkToId(n, i+1, j));
            tmpCla.addVar(tmpLit);
            tmpLit = Lit(-blkToId(n, i+2, j));
            tmpCla.addVar(tmpLit);
            //tmpCla.show();
            outF.addClause(tmpCla);
        }
    }
    
    /* Make sure  0 and 1s are exactly n/2 */
    int *c = NULL;
    initcomb(&c, n, n/2+1);
    do {
        for(int i = 0; i < n; i++) {
            {
                Clause tmpCla;
                Lit tmpLit;
                for(int j = 0; j <= n/2; j++) {
                    tmpLit = Lit(blkToId(n, i, c[j]));
                    tmpCla.addVar(tmpLit);
                }
                //tmpCla.show();
                outF.addClause(tmpCla);
            }
            {
                Clause tmpCla;
                Lit tmpLit;
                for(int j = 0; j <= n/2; j++) {
                    tmpLit = Lit(-blkToId(n, i, c[j]));
                    tmpCla.addVar(tmpLit);
                }
                //tmpCla.show();
                outF.addClause(tmpCla);
            }
            {
                Clause tmpCla;
                Lit tmpLit;
                for(int j = 0; j <= n/2; j++) {
                    tmpLit = Lit(blkToId(n, c[j], i));
                    tmpCla.addVar(tmpLit);
                }
                //tmpCla.show();
                outF.addClause(tmpCla);
            }
            {
                Clause tmpCla;
                Lit tmpLit;
                for(int j = 0; j <= n/2; j++) {
                    tmpLit = Lit(-blkToId(n, c[j], i));
                    tmpCla.addVar(tmpLit);
                }
                //tmpCla.show();
                outF.addClause(tmpCla);
            }
        }
    } while(comb(c, n, n/2+1));
    rmcomb(&c);
    
    /* Make sure no same rows and lines */
    
    int cnt = n*n + 1;
    initcomb(&c, n, 2);
    do {
        {
            int r1 = c[0];
            int r2 = c[1];
            for(int i = 0; i < n; i++) {
            var v1 = blkToId(n, r1, i);
            var v2 = blkToId(n, r2, i);
            Clause c1, c2, c3;
            
            AconjB(c1, c2, c3, cnt + i, v1, v2);
            outF.addClause(c1);
            outF.addClause(c2);
            outF.addClause(c3);
            
            AconjB(c1, c2, c3, cnt + n + i, -v1, -v2);
            outF.addClause(c1);
            outF.addClause(c2);
            outF.addClause(c3);
            
            AconjB(c1, c2, c3, -(cnt + 2*n + i), -(cnt + n + i), -(cnt + i));
            outF.addClause(c1);
            outF.addClause(c2);
            outF.addClause(c3);
            }
            Clause cla;
            for(int i = 0; i < n; i++) {
                cla.addVar(Lit(-(cnt + 2*n + i)));
            }
            outF.addClause(cla);
            cnt += 3*n;
        }
        {
            int r1 = c[0];
            int r2 = c[1];
            for(int i = 0; i < n; i++) {
            var v1 = blkToId(n, i, r1);
            var v2 = blkToId(n, i, r2);
            Clause c1, c2, c3;
            
            AconjB(c1, c2, c3, cnt + i, v1, v2);
            outF.addClause(c1);
            outF.addClause(c2);
            outF.addClause(c3);
            
            AconjB(c1, c2, c3, cnt + n + i, -v1, -v2);
            outF.addClause(c1);
            outF.addClause(c2);
            outF.addClause(c3);
            
            AconjB(c1, c2, c3, -(cnt + 2*n + i), -(cnt + n + i), -(cnt + i));
            outF.addClause(c1);
            outF.addClause(c2);
            outF.addClause(c3);
            }
            Clause cla;
            for(int i = 0; i < n; i++) {
                cla.addVar(Lit(-(cnt + 2*n + i)));
            }
            outF.addClause(cla);
            cnt += 3*n;
        }
    } while(comb(c, n, 2));
    rmcomb(&c);
    
    return 1;
}

int Sudoku::fromCNF(Formula &outF)
{
    int n = S.size();
    for(int i = 1; i <= n*n; i++) {
        int x = (i-1) / n;
        int y = (i-1) % n;
        if (outF.assign[i] == utru) S[x][y] = 1;
        else if (outF.assign[i] == ufal) S[x][y] = 0;
        else S[x][y] = 2;
    }
    return 1;
}

int Sudoku::solve(Sudoku &AnsSu)
{
    Formula F;
    toCNF(F);
    status result = preprocess(F);
    if (result != ok) return 0;
    Solver S(F);
    result = S.solve();
    if (result != ok) return 0;
    S.toF(F);
    AnsSu.fromCNF(F);
    return 1;
}

/* Check whether the soduku is well initialized */
int Sudoku::check()
{
    int n = S.size();
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < n-2; j++) {
            if (S[i][j] != 2 && S[i][j] == S[i][j+1] && S[i][j] == S[i][j+2]) return 0;
        }
    }
    for(int j = 0; j < n; j++) {
        for(int i = 0; i < n-2; i++) {
            if (S[i][j] != 2 && S[i][j] == S[i+1][j] && S[i][j] == S[i+2][j]) return 0;
        }
    }
    
    int cnt = 0;
    for(int i = 0; i < n; i++) {
        cnt = 0;
        for(int j = 0; j < n; j++) {
            if (S[i][j] == 1) cnt++;
        }
        if (2*cnt > n) return 0;
        cnt = 0;
        for(int j = 0; j < n; j++) {
            if (S[i][j] == 0) cnt++;
        }
        if (2*cnt > n) return 0;
    }
    
    for(int j = 0; j < n; j++) {
        cnt = 0;
        for(int i = 0; i < n; i++) {
            if (S[i][j] == 1) cnt++;
        }
        if (2*cnt > n) return 0;
        cnt = 0;
        for(int i = 0; i < n; i++) {
            if (S[i][j] == 1) cnt++;
        }
        if (2*cnt > n) return 0;
    }
    int *c;
    initcomb(&c, n, 2);
    do {
        int r1 = c[0];
        int r2 = c[1];
        int same = 1;
        for(int i = 0; i < n; i++) {
            if (S[r1][i] != S[r2][i]) {
                same = 0;
                break;
            }
        }
        if (same) return 0;
        same = 1;
        for(int i = 0; i < n; i++) {
            if (S[i][r1] != S[i][r2]) {
                same = 0;
                break;
            }
        }
        if (same) return 0;
    } while(comb(c, n, 2));
    rmcomb(&c);
    return 1;
}

void Sudoku::print()
{
    int n = S.size();
    for(int j = 0; j < n; j++) {
        for(int i = 0; i <= 4*n; i++) {
            if (j == 0) {
                if (i == 0 || i == 4*n) printf("+");
                else printf("~");
            }
            else {
                if (i % 4 == 0) {
                    if (i == 0 || i == 4*n) printf("|");
                    else printf("+");
                }
                else {
                    printf("~");
                }
            }
        }
        printf("\n");
        for(int i = 0; i <= 4*n; i++) {
            if (i % 4 == 0) {
                printf("|");
            }
            else if (i % 4 == 2) {
                if (S[j][i/4] == 2) printf(" ");
                else printf("%d", S[j][i/4]);
            }
            else {
                printf(" ");
            }
        }
        printf("\n");
    }
    for(int i = 0; i <= 4*n; i++) {
        if (i == 0 || i == 4*n) {
            printf("+");
        }
        else {
            printf("~");
        }
    }
    printf("\n");
}
#endif