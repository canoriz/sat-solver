#ifndef _CLAUSE_H
#define _CLAUSE_H
#include <vec.h>
#include <literal.h>
#include <utils.h>
//#include <string.h>
#include <hashtable.h>

class Clause {
    int learnt;
    unsigned litNum;
public:
    Vec<Lit> lits;
    
public:
    Clause(int _learnt = 0);
    int isLearnt();
    void addVar(Lit nVar);
    unsigned litN();
    void resize(unsigned sz);
    void moveTo(Clause &other);
    status taotu();
    void show();
};

Clause::Clause(int _learnt)
{
    learnt = _learnt;
    litNum = 0;
}

inline int Clause::isLearnt()
{
    return learnt == 1;
}

inline void Clause::addVar(Lit nVar)
{
    lits.push(nVar);
    litNum++;
}

inline unsigned Clause::litN()
{
    return litNum;
}

inline void Clause::resize(unsigned sz)
{
    assert(sz < litNum);
    lits.resize(sz);
    litNum = sz;
}

void Clause::moveTo(Clause &other)
{
    lits.moveTo(other.lits);
    swap(learnt, other.learnt);
    swap(litNum, other.litNum);
}

/*
 * Determine whether
 * the clause is a
 * taotulogy
 */
status Clause::taotu()
{
    Hash H(5*lits.size());
    for(unsigned i = 0; i < lits.size(); i++) {
        Lit L = lits[i];
        if (H.insert(L.x) == 0) return tru;
    }
    return und;
}

void Clause::show()
{
    int sz = litNum;
    for(int i = 0; i < sz; i++) {
        //printf("%d %d", lits[i].x, taotu());
        if (lits[i].op == no) {
            printf("%d ", lits[i].x);
        }
        else printf("-%d ", lits[i].x);
    }
    //printf("  taotu: %d", taotu());
    printf("\n");
}
#endif