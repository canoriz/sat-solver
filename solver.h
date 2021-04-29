#ifndef _SOLVER_H
#define _SOLVER_H

#include <formula.h>
#include <vec.h>
#include <utils.h>
//#include <time.h>
#include <que.h>
#include <prep.h>

struct ClauseResult {
    unsigned clauseId;
    status s;
};

class Solver {
private:
    float var_inc;
    float var_decay;
    int fixedClauseN;
    int varNum;
    int decisionLevel;
    Formula F;
    
private:
    /* Q stores the id of clause to be extended
     * can implement with self-write unique element queue */
    Queue<qelm> propQ;
    Stack<decision> stk;
    Vec<int> inq;
    Vec<int> level;
    Vec<int> reason;
    Vec<varAct> varOrder;
    
private:
    ClauseResult propagate();
    var decideVar();
    status enqueue(Lit p, int inferredFrom);
    void calcReason(status s, Clause &cla, Clause &out_reason);
    void undoOne();
    void analyze(int clauseId, Clause &out_learnt, int &out_btlevel);
    void cancel();
    void cancelUntil(int level);
    void record(Clause &cla);
    int locked(int clauseId);
    void reduce();
    int lbd(Clause &cla);
    int clearQ();
    void varBump(var x);
    void varDecay();
    void varRescale();
    status search(int nof_conflicts, int nof_learnts);
    
public:
    Solver(Formula& _F);
    void toF(Formula& _F);
    status solve();
    void show();
};


Solver::Solver(Formula& _F)
{
    var_inc = 50.0;
    fixedClauseN = _F.clauseN();
    _F.moveTo(F);
    varNum = F.varN();
    inq.resize(varNum + 1);
    reason.resize(varNum + 1);
    level.resize(varNum + 1);
    stk.setSize(2*varNum);
    propQ.setSize(2*varNum);
    
    for(int i = 0; i <= varNum; i++) {
        inq[i] = 0;
        level[i] = -1;
        reason[i] = -1;
    }
    decisionLevel = -1;
    for(var i = 1; i <= varNum; i++) {
        varOrder.push((varAct){ i, (float)1 });
    }
}

void Solver::toF(Formula& _F)
{
    _F.moveTo(F);
}

int Solver::clearQ()
{
    while(!propQ.empty()) {
        inq[propQ.front().x] = 0;
        propQ.pop();
    }
    return 0;
}

status Solver::enqueue(Lit L, int inferredFrom)
{
    if (F.litCompute(L) != und) {
        if (F.litCompute(L) == ufal) {
            // Conflict!
            return conflict;
        }
        else {
            assert(F.litCompute(L) == utru);
            return ok;
        }
    }
    else {
        // NO conflict!
        assert(F.assign[L.x] == und);
        F.assign[L.x] = utru;
        if (F.litCompute(L) == ufal) {
            F.assign[L.x] = ufal;
        }
        reason[L.x] = inferredFrom;
        level[L.x] = decisionLevel;
        assert(F.assignedN < varNum);
        F.assignedN++;
        stk.push((decision){ L, decisionLevel });
        
        if (inq[L.x] == 0) {
            propQ.push((qelm){ L.x });
            inq[L.x] = 1;
        }
        return ok;
    }
}

ClauseResult Solver::propagate()
{
    while(!propQ.empty()) {
        var p = propQ.front().x;
        propQ.pop();
        inq[p] = 0;
        Vec<location> tmp;
        F.watches[p].moveTo(tmp);
        // F.watches[p.x] <---> tmp
        
        unsigned clauseId;
        for(unsigned i = 0; i < tmp.size(); i++) {
            // F.watches[p.x][i] is the number of the clause
            clauseId = tmp[i].clauseId;
            Clause &cla = F.claux[clauseId];
            if (cla.lits[1].x == p) {
                swap(cla.lits[1], cla.lits[0]);
            }
            assert(cla.lits[0].x == p);
            if (F.litCompute(cla.lits[0]) == utru) {
                F.watches[p].push((location){ clauseId, 1 });
                continue;
            }
            
            // Make sure the ufal literal is at cla.lits[1]
            if (F.litCompute(cla.lits[0]) == ufal) {
                swap(cla.lits[0], cla.lits[1]);
            }
            
            if (F.litCompute(cla.lits[0]) == utru) {
                //Continue
                F.watches[p].push((location){ clauseId, 1 });
                continue;
            }
            
            int flag = 0;
            for(unsigned j = 2; j < cla.litN(); j++) {
                if (F.litCompute(cla.lits[j]) != ufal) {
                    // Found an undefined literal
                    // and swap it with cla.lits[1]
                    swap(cla.lits[j], cla.lits[1]);
                    F.watches[cla.lits[1].x].push((location){ clauseId, 1 });
                    flag = 1;
                    break;
                }
            }
            
            if (flag == 1) {}
            else {
                F.watches[p].push((location){ clauseId, 1 });
                Lit L = cla.lits[0];
                if (enqueue(L, clauseId) == conflict) {
                    clearQ();
                    for(unsigned j = i+1; j < tmp.size(); j++) {
                        unsigned clauseId = tmp[j].clauseId;
                        F.watches[p].push((location){ clauseId, 1 });
                    }
                    return (ClauseResult){ clauseId, conflict };
                }
            }
        }
    }
    return { 0, ok };
}

var Solver::decideVar()
{
    //varOrder.sort();
    float mm = -1;
    var p = 0;
    for(unsigned i = 0; i < varOrder.size(); i++) {
        //varOrder[i].act *= 1;
        //if (F.monitor[varOrder[i].x] == idle) {
        if (F.assign[varOrder[i].x] == und) {
            if (varOrder[i].act > mm) {
                mm = varOrder[i].act;
                p = varOrder[i].x;
            }
        }
    }
    assert(p > 0 && p <= varNum);
    //printf("Decide%d\n", p);
    return p;
}

void Solver::undoOne()
{
    assert(!stk.empty());
    Lit L;
    if (stk.top().L.op == no) {
        L = Lit(stk.top().L.x);
    }
    else {
        L = Lit(-stk.top().L.x);
        assert(stk.top().L.op == neg);
    }
    
    stk.pop();
    assert(F.assignedN <= varNum);
    F.assignedN--;
    F.assign[L.x] = und;
    reason[L.x] = -1;
    level[L.x] = -1;
}

void Solver::cancel()
{
    #ifdef SEQUENCE
    printf("cancel layer: %d\n", decisionLevel);
    #endif
    while (!stk.empty() && stk.top().decisionLevel == decisionLevel) {
        undoOne();
    }
    decisionLevel--;
}

void Solver::cancelUntil(int level)
{
    while (decisionLevel > level) {
        cancel();
    }
    assert(decisionLevel == level);
}

void Solver::calcReason(status s, Clause &cla, Clause &out_reason)
{
    Clause out_r;
    out_r.moveTo(out_reason);
    unsigned i = 1;
    // This is a conflict clause if p is now assigned und
    if (s == conflict) i = 0;
    for( ; i < cla.litN(); i++) {
        out_reason.addVar(cla.lits[i].rev());
        //assert(F.litCompute(cla.lits[i]) == ufal || (F.litCompute(cla.lits[i]) == und && i == 0) );
    }
}

void Solver::analyze(int clauseId, Clause &out_learnt, int &out_btlevel)
{
    Vec<int> seen;
    seen.resize(varNum + 1);
    int counter = 0;
    Lit p;
    Clause p_reason;
    for(unsigned i = 0; i < seen.size(); i++) {
        seen[i] = 0;
    }
    status s = conflict;
    
    out_learnt.addVar(-1);
    out_btlevel = -1; // leave room for the asserting literal
    do {
        Clause tmp;
        tmp.moveTo(p_reason);
        Clause &confl = F.claux[clauseId];
        assert(clauseId >= 0);
        #ifdef SEQUENCE
        printf("Trace clause:");
        confl.show();
        #endif
        calcReason(s, confl, p_reason);
        s = unit;
        
        //- Trace reason for p
        for(unsigned j = 0; j < p_reason.litN(); j++) {
            Lit q = p_reason.lits[j];
            
            if (!seen[q.x]) {
                seen[q.x] = 1;
                if (level[q.x] == decisionLevel) {
                    counter++;
                }
                else if (level[q.x] > -1) {
                    //assert(level[q.x] >= 0);
                    out_learnt.addVar(q.rev());
                    if (level[q.x] > out_btlevel) {
                        out_btlevel = level[q.x];
                    }
                }
            }
        }
        
        //- Select next literal to look at
        do {
            p = stk.top().L;
            clauseId = reason[p.x];
            assert(clauseId >= -1);
            undoOne();
        } while(!seen[p.x]);
        counter--;
    } while(counter > 0);
    out_learnt.lits[0] = p.rev();
    #ifdef SEQUENCE
    printf("Learned clause:");
    out_learnt.show();
    #endif
}

void Solver::record(Clause &cla)
{
    Clause c(1);
    cla.moveTo(c);
    assert(c.litN() > 0);
    Lit L = c.lits[0];
    if (c.litN() == 1) {
        F.assign[L.x] = utru;
        if (F.litCompute(L) == ufal) {
            F.assign[L.x] = ufal;
        }
        level[L.x] = -1;
        reason[L.x] = -1;
        F.assignedN++;
        assert(propQ.empty());
        propQ.push((qelm){ L.x });
        varBump(L.x);
    }
    else {
        // Enqueue can not be failed
        for(unsigned i = 0; i < c.litN(); i++) {
            varBump(c.lits[i].x);
        }
        enqueue(L, F.clauseN());
        F.addClause(c);
        assert((int)F.clauseN() == reason[L.x] + 1);
    }
}

inline int Solver::locked(int clauseId)
{
    assert(clauseId >= 0);
    return reason[F.claux[clauseId].lits[0].x] == clauseId;
}

void Solver::reduce()
{
    Vec<varAct> claOrder(F.clauseN());
    Vec<int> clause_old_to_new;
    clause_old_to_new.resize(F.clauseN());
    
    for(unsigned i = 0; i < F.clauseN(); i++) {
        Clause &other = F.claux[i];
        claOrder.push((varAct){ (var)i, (float)lbd(other) });
        clause_old_to_new[i] = -1;
    }
    claOrder.sort();
    
    Formula newF;
    F.extMoveTo(newF);
    float key = claOrder[claOrder.size()/2].act;
    key = key > 2.0 ? key : 2.0;
    for(unsigned i = 0; i < claOrder.size(); i++) {
        int clauseId = claOrder[i].x;
        float cla_act = claOrder[i].act;
        Clause &other = F.claux[clauseId];
        if (!other.isLearnt() || locked(clauseId) || cla_act <= key) {
            // Clause is preserved
            assert(clause_old_to_new[clauseId] == -1);
            clause_old_to_new[clauseId] = newF.clauseN();
            newF.addClause(other);
        }
    }
    
    for(unsigned i = 1; i <= F.varN(); i++) {
        if (reason[i] != -1) {
            reason[i] = clause_old_to_new[reason[i]];
        }
    }
    
    // Moving newF back to F
    newF.moveTo(F);
}

int Solver::lbd(Clause &cla)
{
    Hash H(decisionLevel+4);
    int num = 0;
    for(unsigned i = 0; i < cla.litN(); i++) {
        int p = level[cla.lits[i].x];
        // prevent p == -1 cause hashtable array overflow
        if (p == -1) p = decisionLevel+2;
        if(H.insert(p)) num++;
    }
    return num;
}


inline void Solver::varBump(var x)
{
    if ((varOrder[x-1].act += var_inc) > 1e100) {
        varRescale();
    }
}

inline void Solver::varDecay()
{
    var_inc *= var_decay;
}

inline void Solver::varRescale()
{
    for(unsigned i = 0; i < varOrder.size(); i++) {
        varOrder[i].act *= 1e-100;
    }
    var_inc *= 1e-100;
}

status Solver::search(int nof_conflicts, int nof_learnts)
{
    int conflictC = 0;
    var_decay = 1.0 / 0.95;
    while(1) {
        ClauseResult res = propagate();
        if(res.s == conflict) {
            assert(res.s == conflict);
            conflictC++;
            Clause learnt_clause(1);
            int bt_lvl;
            
            /* Top level conflict found */
            if (decisionLevel == -1) return conflict;
            
            analyze(res.clauseId, learnt_clause, bt_lvl);
            cancelUntil(bt_lvl);
            record(learnt_clause);
            varDecay();
        }
        else {
            if ((int)F.clauseN() - (int)F.assignedN - fixedClauseN >= nof_learnts) {
                reduce();
            }
            
            if (F.assignedN == varNum) {
                return ok;
            }
            
            if (conflictC >= nof_conflicts) {
                cancelUntil(-1);
                return und;
            }
            
            else {
                decisionLevel++;
                var d = decideVar();
                enqueue(Lit(-d), -1);
            }
        }
    }
}

status Solver::solve()
{
    int nof_learnts = F.clauseN() / 3;
    int nof_conflicts = 100;
    status s = und;
    
    while (s == und) {
        s = search(nof_conflicts, nof_learnts);
        nof_conflicts *= 1.5;
        nof_learnts *= 1.1;
    }
    return s;
}

void Solver::show()
{
    F.show();
}
#endif