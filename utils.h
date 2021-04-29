#ifndef _UTILS_H
#define _UTILS_H
#include <stdio.h>
//#include <time.h>
//#include <assert.h>
/*
 * Enumerate
 * recording status of a literal
 * namely
 * true false
 * undefined
 * undefined true
 * undefined false
 */
 
typedef int var;

enum status {
    tru, fal, utru, ufal, und, 
    unit, conflict, normal, satisfied, err, ok
};

enum type {
    no, neg
};

struct qelm {
    int x;
};

struct location {
    unsigned clauseId;
    unsigned litId;
};

/* random(m) -> [0, m) */
inline int random(int m)
{
    //srand(unsigned(time(NULL)));
    return rand() % m;
}

/*
 * For best performance
 * This swap should only be used on
 * basic types int, char, etc.
 * no complicated class T
 */
template<class T>
inline void swap(T& a, T& b)
{
    T tmp;
    tmp = a;
    a = b;
    b = tmp;
}

void initcomb(int **c, int n, int k)
{
    assert(k <= n);
    *c = (int*)malloc(sizeof(int)*k);
    for(int i = 0; i < k; i++) (*c)[i] = i;
}

void rmcomb(int **c)
{
    free(*c);
    *c = NULL;
}

/* Generate next k combination out of n
 * Elements: 0..n-1
 * Stored in: c[0..k-1] */
int comb(int *c, int n, int k)
{
    for(int i = k-1; i >= 0; i--) {
        if (c[i] < n - (k-i)) {
            c[i]++;
            for(int j = i+1; j < k; j++) {
                c[j] = c[j-1]+1;
            }
            return 1;
        }
    }
    return 0;
}

struct varAct {
    var x;
    float act;
    bool operator < (const varAct &other) const {
        return act < other.act;
    }
    bool operator == (const varAct &other) const {
        return act == other.act;
    }
};
#endif