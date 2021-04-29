#ifndef _LITERAL_H
#define _LITERAL_H
#include <utils.h>

/*
 * The Lit class
 * x stores variable
 * type 1 or -1
 */
class Lit {
public:
    var x;
    type op;

public:
    Lit(var _x = 0);
    Lit rev();
};

Lit::Lit(var _x)
{
    if (_x < 0) {
        op = neg;
        x = -_x;
    }
    else {
        op = no;
        x = _x;
    }
}

inline Lit Lit::rev()
{
    if (op == neg) {
        return Lit(x);
    }
    else {
        return Lit(-x);
    }
}

struct decision {
    Lit L;
    int decisionLevel;
};
#endif