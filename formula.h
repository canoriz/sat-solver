#ifndef _FORMULA_H
#define _FORMULA_H
//#define PRINTING
#define DEFAULT_VARN 2
#include <vec.h>
#include <clause.h>
#include <utils.h>
//#include <assert.h>

class Formula {
    unsigned clauseNum;
    unsigned varNum;
    unsigned inferredN;
    unsigned uncertainN;
    Vec<int> trueLitN;  // How many literals are true
    Vec<int> falseLitN; // How many literals are false
    
public:
    int assignedN;
    Vec<Clause> claux;
    Vec<status> assign;
    Vec<Vec<location> >  watches;
    Vec<int> counter;
    
public:
    Formula(unsigned varN = DEFAULT_VARN);
    void addEmptyClause();
    void moveClauseFrom(Clause &other);
    void addClause(Clause &other);
    unsigned clauseN();
    unsigned varN();
    
    status litCompute(Lit L);
    status propagateClause(int clauseId, var x);
    status simplifyClause(int clauseId);
    status compute();
    void moveTo(Formula &other);
    void extMoveTo(Formula &other);
    void show();
};

Formula::Formula(unsigned varN)
{
    varNum = varN;
    clauseNum = 0;
    
    /* Set all assignments to und
     * Be careful: some code may depend on this
     * if wants to enable root level assignments
     * needs careful examine */
    watches.resize(varNum + 1);
    assign.resize(varNum + 1);
    counter.resize(varNum + 1);
    for(unsigned i = 1; i <= varNum; i++) {
        counter[i] = 0;
    }
    for(unsigned i = 0; i < assign.size(); i++) {
        assign[i] = und;
    }
    
    // Set decided variables, inferred variables and decided but not checked: uncertain variables
    assignedN = 0;
    inferredN = 0;
    uncertainN = 0;
    //===
}

void Formula::addClause(Clause &other)
{
    /* Special treatment for songle literal clause */
    if (other.litN() < 2) {
        moveClauseFrom(other);
        return;
    }
    
    /* Adding the clause number to  variable watches */
    for(unsigned i = 0; i < other.litN(); i++) {
        var p = other.lits[i].x;
        /* Adding counter */
        counter[p]++;
        /* Adding watches */
        assert((unsigned)p <= varNum);
        if (i < 2) {
            watches[p].push((location) {clauseNum, i});
        }
    }
    
    /* Adding to clause vector claux */
    moveClauseFrom(other);
}

inline void Formula::moveClauseFrom(Clause &other)
{
    if (clauseNum >= claux.size()) claux.push();
    other.moveTo(claux[clauseNum]);
    clauseNum++;
}

inline unsigned Formula::clauseN()
{
    return clauseNum;
}

inline unsigned Formula::varN()
{
    return varNum;
}

inline status Formula::litCompute(Lit L)
{
    if (L.op == no) {
        switch (assign[L.x]) {
            case utru :
                return utru;
                break;
            case ufal:
                return ufal;
                break;
            case und:
                return und;
                break;
            case tru:
                return tru;
                break;
            case fal:
                return fal;
                break;
            default:
                assert(1==0);
        }
    }
    else {
        assert(L.op == neg);
        switch (assign[L.x]) {
            case utru :
                return ufal;
                break;
            case ufal:
                return utru;
                break;
            case und:
                return und;
                break;
            case tru:
                return fal;
                break;
            case fal:
                return tru;
                break;
            default:
                assert(1==0);
        }
    }
    return err;
}

// Simplify clause and returns the undefined nums
status Formula::simplifyClause(int clauseId)
{
    Clause &cla = claux[clauseId];
    unsigned undNum = cla.litN();
    unsigned i = 0;
    while(i < undNum) {
        Lit L = cla.lits[i];
        if (litCompute(L) == utru) {
            swap(cla.lits[i], cla.lits[0]);
            cla.resize(1);
            return ok;
        }
        else if (litCompute(L) == ufal) {
            undNum--;
            swap(cla.lits[i], cla.lits[undNum]);
        }
        else {
            assert(litCompute(L) == und);
            i++;
        }
    }
    if (undNum < cla.litN()) {
        cla.resize(undNum);
    }
    if (undNum == 0) return conflict;
    else if (undNum == 1) {
        Lit L = claux[clauseId].lits[0];
        assign[L.x] = utru;
        if (litCompute(L) != utru) {
            assert(litCompute(L) == ufal);
            assign[L.x] = ufal;
        }
        return unit;
    }
    else return ok;
}

status Formula::compute()
{
    int res = 1;
    for(unsigned i = 0; i < clauseNum; i++) {
        Clause &tmpC = claux[i];
        int clauseRes = 0;
        for(unsigned j = 0; j < tmpC.lits.size(); j++) {
            Lit &tmpL = tmpC.lits[j];
            if ((assign[tmpL.x] == utru && tmpL.op == no) || (assign[tmpL.x] == ufal && tmpL.op == neg)) {
            //if (litCompute(tmpL) == 1) {
                clauseRes = (clauseRes || 1);
            }
            else {
                clauseRes = (clauseRes || 0);
            }
        }
        res = (res && clauseRes);
    }
    if (res == 1) return ok;
    return conflict;
}

void Formula::moveTo(Formula &other)
{
    other.varNum = varNum;
    other.clauseNum = clauseNum;
    other.assignedN = assignedN;
    other.inferredN = inferredN;
    other.uncertainN = uncertainN;
    
    trueLitN.moveTo(other.trueLitN);  // How many literals are true
    falseLitN.moveTo(other.falseLitN); // How many literals are false
    claux.moveTo(other.claux);
    assign.moveTo(other.assign);
    counter.moveTo(other.counter);
    watches.moveTo(other.watches);
}

/* Extended move
 * Do not move claux
 * Do not move watches
 * for the sake of pre process */
void Formula::extMoveTo(Formula &other)
{
    other.watches.resize(varNum + 1);
    trueLitN.moveTo(other.trueLitN);  // How many literals are true
    falseLitN.moveTo(other.falseLitN); // How many literals are false
    assign.moveTo(other.assign);
    counter.moveTo(other.counter);
    
    other.varNum = varNum;
    other.assignedN = assignedN;
    swap(inferredN, other.inferredN);
    swap(uncertainN, other.uncertainN);
}

void Formula::show()
{
    printf("v ");
    for(unsigned i = 1; i <= varNum; i++) {
        if (assign[i] == utru) printf("%d ", i);
        else {
            printf("-%d ", i);
        }
    }
    printf("\n");
}
#undef DEFAULT_VARN
#endif