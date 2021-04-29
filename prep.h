#ifndef _PREP_H
#define _PREP_H
#include <formula.h>
#include <assert.h>

status preprocess(Formula &outF)
{
    Queue<qelm> propQ(outF.varN()+1);
    Vec<Vec<unsigned> > W;
    W.resize(outF.varN() + 1);
    // Assign variables in single literal clauses in outF
    for(unsigned i = 0; i < outF.clauseN(); i++) {
        // Copyed from Formula::addclause
        /* Special treatment for songle literal clause */
        Clause &other = outF.claux[i];
        for(unsigned j = 0; j < other.litN(); j++) {
            W[other.lits[j].x].push(i);
        }
        if (other.litN() < 2) {
            assert(other.litN() == 1);
            Lit L = other.lits[0];
            if (outF.litCompute(L) != und) {
                if (outF.litCompute(L) == ufal) {
                    /* Conflict */
                    return conflict;
                }
                else {
                    /* Not conflict */
                }
            }
            else {
                outF.assign[L.x] = utru;
                if (outF.litCompute(L) == ufal) {
                    outF.assign[L.x] = ufal;
                }
                // Adding inferred tag in monitor
                // which is not pushed to stack
                // Because this will not change any time in the future
                outF.assignedN++;
                propQ.push((qelm){ L.x });
            }
        }
    }
    
    // Using F.assign[] to simplify clauses in outF
    // moving necessary clauses to F
    while(!propQ.empty()) {
        int varId = propQ.front().x;
        propQ.pop();
        for(unsigned i = 0; i < W[varId].size(); i++) {
            int clauseId = W[varId][i];
            Clause &other = outF.claux[clauseId];

            if (other.litN() < 2) {
                assert(other.litN() == 1);
                // Single literal clause, do nothing
            }
            else {
                status result = outF.simplifyClause(clauseId);
                if (result == unit) {
                    // New varisble inferred, reset.
                    Lit &L = other.lits[0];
                    outF.assignedN++;
                    // Adding inferred tag in monitor
                    // which is not pushed to stack
                    // Because this will not change any time in the future
                    propQ.push((qelm){ L.x });
                }
                else if (result == conflict) {
                    return conflict;
                }
                else {
                    assert(result == ok);
                }
            }
        }
    }
    
    // Move outF to F
    // Constructing a standard cnf in F
    Formula F;
    outF.extMoveTo(F);
    
    for(unsigned i = 0; i < outF.clauseN(); i++) {
        // Copyed from Formula::addclause
        /* Special treatment for songle literal clause */
        Clause &other = outF.claux[i];
        if (other.litN() > 1) {
            F.addClause(other);
        }
    }
    
    // Moving F back to outF
    F.moveTo(outF);
    return ok;
}
#endif
