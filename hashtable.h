#ifndef _HASHTABLE_H
#define _HASHTABLE_H
//#include <stdlib.h>
//#include <assert.h>
/*
 * Simple Hash table implamentation
 */

/*
 * Linear probing fixed size hash table
 * only suppport int
 */
 
class Hash {
    int *ha;
    int sz;
public:
    Hash(int _sz);
    ~Hash();
    int insert(int x);
};

/*
 * Construction function
 * Fixed size
 * No overflow check!
 */
Hash::Hash(int _sz)
{
    sz = _sz;
    ha = (int*)malloc(sizeof(int) * sz);
    memset(ha, -1, sizeof(int) * sz);
}

Hash::~Hash()
{
    free(ha);
}

/*
 * Hash insert
 * insert x into hash table
 * if x not in hash table
 * insert x
 * return 1
 * if x already in hash table
 * return 0
 */
inline int Hash::insert(int x)
{
    int pos = x % sz;
    for(int i = pos; i < pos+sz; i++) {
        if (ha[i % sz] == -1) {
            ha[i % sz] = x;
            return 1;
        }
        else if (ha[i % sz] == x) {
            return 0;
        }
    }
    assert("Insert(x) in hashtable.h. This should not happen!");
    return 0;
}
#endif