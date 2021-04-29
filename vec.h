#ifndef _VEC_H
#define _VEC_H
#define INIT_SIZE 16
#define EXPAND_CONST 2
#include <utils.h>

/*
 * class Vec
 * a flexible vector
 */
 
/*
 * Should never simply push(T)
 * when T contains a pointer
 * This may cause double free problem
 * 
 * For time complexity
 * because now the = operator is doing shallow copy
 * If want to fix
 * overloading the = to do deep copy
 */

class expand;
template<class T>
class Vec {
protected:
    unsigned alloc_size;
    unsigned used_size;
    T *data;

private:
    void shrink();
    void destroy();
    
public:
    Vec(unsigned vecSize = INIT_SIZE);
    ~Vec();
    void expand();
    void resize(int sz);
    int empty();
    unsigned size();
    void push(const T &inserted);
    void push();
    void rm(unsigned i);
    void pop();
    void moveTo(Vec<T> &other);
    void copyTo(Vec<T> &other);
    void qsort(int l, int r);
    void sort();
    T& operator[](unsigned index);
};

/*
 * For the sake of performance
 * No cross border check
 */
template<class T>
inline T& Vec<T>::operator[](unsigned index)
{
    assert(index < used_size || index >= 0);
    return data[index];
}

template<class T>
inline unsigned Vec<T>::size()
{
    return used_size;
}

template<class T>
Vec<T>::Vec(unsigned vecSize)
{
    data = new T[vecSize];
    alloc_size = vecSize;
    used_size = 0;
}

/*
 * Moving this to other
 * In fact exchange two
 * vecs
 */
template<class T>
inline void Vec<T>::moveTo(Vec<T> &other)
{
    swap(alloc_size, other.alloc_size);
    swap(used_size, other.used_size);
    swap(data, other.data);
}

template<class T>
Vec<T>::~Vec()
{
    delete[] data;
}

template<class T>
inline void Vec<T>::expand()
{
    unsigned old_size = alloc_size;
    alloc_size *= EXPAND_CONST;
    T *new_addr = new T[alloc_size]; 
    //T *new_addr = (T*)malloc(sizeof(T) * alloc_size); 
    {
        /*
         * Swapping
         * Preventing double free
         * !!!VERY VERY IMPORTANT!!!
         */
        unsigned msize = sizeof(T) * old_size;
        unsigned char *data_end = (unsigned char*) data + msize;
        unsigned char *p1 = (unsigned char*)data;
        unsigned char *p2 = (unsigned char*)new_addr;
        unsigned char buf;
        while (p1 < data_end) {
            buf = *p1;
            *p1 = *p2;
            *p2 = buf;
            p1++;
            p2++;
        }
    }
    delete[] data;
    data = new_addr;
}

/*
 * Immediately reset
 * alloc_size and used_size to sz
 */
template<class T>
inline void Vec<T>::resize(int sz)
{
    if ((unsigned) sz <= alloc_size) {
        used_size = sz;
        return;
    }
    unsigned old_size = alloc_size;
    alloc_size = sz;
    used_size = sz;
    T *new_addr = new T[alloc_size]; 
   
    {
        /*
         * Swapping
         * Preventing double free
         * !!!VERY VERY IMPORTANT!!!
         */
        unsigned msize = sizeof(T) * old_size;
        unsigned char *data_end = (unsigned char*) data + msize;
        unsigned char *p1 = (unsigned char*)data;
        unsigned char *p2 = (unsigned char*)new_addr;
        unsigned char buf;
        while (p1 < data_end) {
            buf = *p1;
            *p1 = *p2;
            *p2 = buf;
            p1++;
            p2++;
        }
    }
    
    delete[] data;
    data = new_addr;
}

template<class T>
void Vec<T>::shrink()
{
    //Need fixing
}

template<class T>
void Vec<T>::destroy()
{
    /*
     * [DEPERECATED]
     * Should not use any more
     */
    alloc_size = 0;
    used_size = 0;
    if (data) delete[] data;
    data = NULL;
}

template<class T>
inline void Vec<T>::push(const T &inserted)
{
    if (used_size >= alloc_size) {
        expand();
    }
    data[used_size] = inserted;
    used_size++;
}

template<class T>
inline void Vec<T>::push()
{
    if (used_size >= alloc_size) {
        expand();
    }
    used_size++;
}

/*
 * Remove data[i]
 */
template<class T>
void Vec<T>::rm(unsigned i)
{
    used_size--;
    T* buf = (T*)malloc(sizeof(T));
    (*buf) = data[i];
    for(unsigned j = i; j < used_size; j++) {
        data[j] = data[j+1];
    }
    data[used_size] = (*buf);
    free(buf);
}

template<class T>
void Vec<T>::pop()
{
    used_size--;
}

template<class T>
int Vec<T>::empty()
{
    return used_size == 0;
}

template<class T>
void Vec<T>::qsort(int l, int r)
{
    if (l >= r) return;
    int lt = l;
    int gt = r + 1;
    
    /* Pick a pivot randomly
     * random(m) in utils.h returns a [0,m) int */
    swap(data[l], data[random(r-l) + l]);
    T pivot = data[l];
    for(int i = l+1; i < gt;) {
        if (data[i] < pivot) {
            lt++;
            swap(data[i], data[lt]);
            i++;
        }
        else if (data[i] == pivot) {
            i++;
        }
        else {
            gt--;
            swap(data[i], data[gt]);
        }
    }
    swap(data[lt], data[l]);
    qsort(l, lt-1);
    qsort(gt, r);
}

template<class T>
void Vec<T>::sort()
{
    qsort(0, used_size - 1);
}

#undef INIT_SIZE
#undef EXPAND_CONST
#endif