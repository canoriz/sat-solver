#include <vec.h>
/*
 * Queue(FIFO), stack(FILO) implementation
 * All elements should be static
 * and do not containing pointers
 * based on the vec.h
 * using looping head, tail for fixed size queue
 * using fixed Vec for stack
 * 
 * Remember: no extra check whether queue overflowed
 * Must not overflow
 */

// Queue implementation
template<class T>
class Queue: Vec<T> {
private:
    unsigned tail, head, qSize;
public:
    Queue(unsigned _qSize = 2);
    void setSize(unsigned _qSize);
    int empty();
    void push(const T &inserted);
    void pop();
    T front();
};

template<class T>
Queue<T>::Queue(unsigned _qSize): Vec<T>(_qSize)
{
    head = 0;
    tail = _qSize - 1;
    qSize = _qSize;
}

template<class T>
void Queue<T>::setSize(unsigned _qSize)
{
    Vec<T>::resize(_qSize);
    qSize = _qSize;
    head = 0;
    tail = qSize - 1;
}

template<class T>
inline int Queue<T>::empty()
{
    return (tail + 1) % qSize == head;
}

template<class T>
inline void Queue<T>::push(const T &inserted)
{
    tail++;
    if (tail >= qSize) tail -= qSize;
    assert(tail >= 0 && tail < qSize);
    Vec<T>::data[tail] = inserted;
}

template<class T>
inline void Queue<T>::pop()
{
    head++;
    if (head >= qSize) head -= qSize;
    assert(head >= 0 && head < qSize);
}

template<class T>
inline T Queue<T>::front()
{
    assert(head >= 0 && head < qSize);
    return Vec<T>::data[head];
}

//Stack implementation
template<class T>
class Stack: Vec<T> {
private:
    unsigned tail;
    unsigned sSize;
public:
    Stack(unsigned _sSize = 2);
    void setSize(unsigned _sSize);
    int empty();
    void push(const T &inserted);
    void pop();
    T top();
};

template<class T>
Stack<T>::Stack(unsigned _sSize): Vec<T>(_sSize)
{
    sSize = _sSize;
    tail = sSize - 1;
}

template<class T>
void Stack<T>::setSize(unsigned _sSize)
{
    Vec<T>::resize(_sSize);
    sSize = _sSize;
    tail = sSize - 1;
}

template<class T>
inline int Stack<T>::empty()
{
    return tail == sSize - 1;
}

template<class T>
inline void Stack<T>::push(const T &inserted)
{
    tail++;
    if (tail >= sSize) tail -= sSize;
    assert(tail < sSize && tail >= 0);
    Vec<T>::data[tail] = inserted;
}

template<class T>
inline void Stack<T>::pop()
{
    if (tail == 0) {
        tail =  sSize - 1;
    }
    else {
        tail--;
    }
    assert(tail < sSize && tail >= 0);
}

template<class T>
inline T Stack<T>::top()
{
    assert(tail >= 0 && tail < sSize);
    return Vec<T>::data[tail];
}