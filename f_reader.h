#ifndef _F_READER_H
#define _F_READER_H
//#include <stdio.h>
//#include <stdlib.h>
//#include <assert.h>
#include <vec.h>
#include <clause.h>
#include <formula.h>
#include <utils.h>

#define MAX_CH_PER_LINE 300

void readcnf(const char file[], Formula &outFormula)
{
    FILE *pFile;
    pFile = fopen(file, "r");
    assert(pFile != NULL);
    
    char option;
    
    //Reading comments
    char *buf = (char*)malloc(sizeof(char) * MAX_CH_PER_LINE); //Buffer for comment lines
    for(;;) {
        if (EOF == fscanf(pFile, "%c", &option)) exit(1);
        if (option == 'c') {
            fgets(buf, MAX_CH_PER_LINE, pFile);
        }
        else break;
    }
    free(buf);
    
    //Reading problem type
    assert(option == 'p');
    char *satType = (char*)malloc(sizeof(char) * MAX_CH_PER_LINE);
    fscanf(pFile, "%s", satType);
    free(satType);
    
    int varNum, clauseNum;
    fscanf(pFile, "%d%d", &varNum, &clauseNum);
    
    Formula(varNum).moveTo(outFormula);
    for(int i = 0; i < clauseNum; i++) {
        
        Clause tmpCla;
        
        for(;;) {
            int tmpVar;
            fscanf(pFile, "%d", &tmpVar);
            if (tmpVar == 0) break;
            Lit tmpLit(tmpVar);
            tmpCla.addVar(tmpLit);
        }
        if (tmpCla.taotu() != tru) {
            outFormula.addClause(tmpCla);
        }
    }
    fclose(pFile);
}

#undef MAX_CH_PER_LINE
#endif
