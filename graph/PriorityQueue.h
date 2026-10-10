#ifndef PRIORITYQUEUE_H
#define PRIORITYQUEUE_H


#include <ostream>
#include <vector>
#include <cassert>

//----------------------- swap -------------------------------------------
//
//  used to swap two values
//------------------------------------------------------------------------
template<class T>
void swap(T &a, T &b)
{
  T temp = a;
  a = b;
  b = temp;
}

//-------------------- reorderUpwards ------------------------------------
//
//  given a heap and a node in the heap, this function moves upwards
//  through the heap swapping elements until the heap is ordered
//------------------------------------------------------------------------
template<class T>
void reorderUpwards(std::vector<T>& heap, int nd)
{
  //move up the heap swapping the elements until the heap is ordered
  while ( (nd>1) && (heap[nd/2] < heap[nd]))
  {
    swap(heap[nd/2], heap[nd]);

    nd /= 2;
  }
}

//--------------------- reorderDownwards ---------------------------------
//
//  given a heap, the heapsize and a node in the heap, this function
//  reorders the elements in a top down fashion by moving down the heap
//  and swapping the current node with the greater of its two children
//  (provided a child is larger than the current node)
//------------------------------------------------------------------------
template<class T>
void reorderDownwards(std::vector<T>& heap, int nd, int heapSize)
{
  //move down the heap from node nd swapping the elements until
  //the heap is reordered
  while (2*nd <= heapSize)
  {
    int child = 2 * nd;

    //set child to largest of nd's two children
    if ( (child < heapSize) && (heap[child] < heap[child+1]) )
    {
      ++child;
    }

    //if this nd is smaller than its child, swap
    if (heap[nd] < heap[child])
    {
      swap(heap[child], heap[nd]);

      //move the current node down the tree
      nd = child;
    }

    else
    {
      break;
    }
  }
}



//--------------------- PriorityQ ----------------------------------------
//
//  basic heap based priority queue implementation
//------------------------------------------------------------------------
template<class T>
class PriorityQ
{
private:

  std::vector<T>  mHeap;

  int             mSize;

  int             mMaxSize;

  //given a heap and a node in the heap, this function moves upwards
  //through the heap swapping elements until the heap is ordered
  void reorderUpwards(std::vector<T>& heap, int nd)
  {
    //move up the heap swapping the elements until the heap is ordered
    while ( (nd>1) && (heap[nd/2] < heap[nd]))
    {
      swap(heap[nd/2], heap[nd]);

      nd /= 2;
    }
  }

  //given a heap, the heapsize and a node in the heap, this function
  //reorders the elements in a top down fashion by moving down the heap
  //and swapping the current node with the greater of its two children
  //(provided a child is larger than the current node)
  void reorderDownwards(std::vector<T>& heap, int nd, int heapSize)
  {
    //move down the heap from node nd swapping the elements until
    //the heap is reordered
    while (2*nd <= heapSize)
    {
     int child = 2 * nd;

      //set child to largest of nd's two children
      if ( (child < heapSize) && (heap[child] < heap[child+1]) )
     {
        ++child;
      }

      //if this nd is smaller than its child, swap
      if (heap[nd] < heap[child])
      {
        swap(heap[child], heap[nd]);

        //move the current node down the tree
        nd = child;
      }

      else
      {
        break;
      }
    }
  }

public:

  PriorityQ(int maxSize):mMaxSize(maxSize), mSize(0)
  {
    mHeap.assign(maxSize+1, T());
  }

  bool empty()const{return (mSize==0);}

  //to insert an item into the queue it gets added to the end of the heap
  //and then the heap is reordered
  void insert(const T item)
  {

    assert (mSize+1 <= mMaxSize);

    ++mSize;

    mHeap[mSize] = item;

    reorderUpwards(mHeap, mSize);
  }

  //to get the max item the first element is exchanged with the lowest
  //in the heap and then the heap is reordered from the top down.
  T pop()
  {
    swap(mHeap[1], mHeap[mSize]);

    reorderDownwards(mHeap, 1, mSize-1);

    return mHeap[mSize--];
  }

  //so we can take a peek at the first in line
  const T& peek()const{return mHeap[1];}
};

//--------------------- PriorityQLow -------------------------------------
//
//  basic 2-way heap based priority queue implementation. This time the priority
//  is given to the lowest valued key
//------------------------------------------------------------------------
template<class T>
class PriorityQLow
{
private:

  std::vector<T>  mHeap;

  int             mSize;

  int             mMaxSize;

  //given a heap and a node in the heap, this function moves upwards
  //through the heap swapping elements until the heap is ordered
  void reorderUpwards(std::vector<T>& heap, int nd)
  {
    //move up the heap swapping the elements until the heap is ordered
    while ( (nd>1) && (heap[nd/2] > heap[nd]))
    {
      swap(heap[nd/2], heap[nd]);

      nd /= 2;
    }
  }

  //given a heap, the heapsize and a node in the heap, this function
  //reorders the elements in a top down fashion by moving down the heap
  //and swapping the current node with the smaller of its two children
  //(provided a child is larger than the current node)
  void reorderDownwards(std::vector<T>& heap, int nd, int heapSize)
  {
    //move down the heap from node nd swapping the elements until
    //the heap is reordered
    while (2*nd <= heapSize)
    {
     int child = 2 * nd;

      //set child to largest of nd's two children
      if ( (child < heapSize) && (heap[child] > heap[child+1]) )
     {
        ++child;
      }

      //if this nd is smaller than its child, swap
      if (heap[nd] > heap[child])
      {
        swap(heap[child], heap[nd]);

        //move the current node down the tree
        nd = child;
      }

      else
      {
        break;
      }
    }
  }

public:

  PriorityQLow(int maxSize):mMaxSize(maxSize), mSize(0)
  {
    mHeap.assign(maxSize+1, T());
  }

  bool empty()const{return (mSize==0);}

  //to insert an item into the queue it gets added to the end of the heap
  //and then the heap is reordered
  void insert(const T item)
  {
    assert (mSize+1 <= mMaxSize);

    ++mSize;

    mHeap[mSize] = item;

    reorderUpwards(mHeap, mSize);
  }

  //to get the max item the first element is exchanged with the lowest
  //in the heap and then the heap is reordered from the top down.
  T pop()
  {
    swap(mHeap[1], mHeap[mSize]);

    reorderDownwards(mHeap, 1, mSize-1);

    return mHeap[mSize--];
  }

  //so we can take a peek at the first in line
  const T& peek()const{return mHeap[1];}
};

//----------------------- IndexedPriorityQLow ---------------------------
//
//  priority queue based on an index into a set of keys. The queue is
//  maintained as a 2-way heap.
//
//  The priority in this implementation is the lowest valued key
//------------------------------------------------------------------------
template<class KeyType>
class IndexedPriorityQLow
{
private:

  std::vector<KeyType>&  mKeys;

  std::vector<int>       mHeap;

  std::vector<int>       mInverseHeap;

  int                    mSize,
                         mMaxSize;

  void swap(int a, int b)
  {
    int temp = mHeap[a]; mHeap[a] = mHeap[b]; mHeap[b] = temp;

    //change the handles too
    mInverseHeap[mHeap[a]] = a; mInverseHeap[mHeap[b]] = b;
  }

  void reorderUpwards(int nd)
  {
    //move up the heap swapping the elements until the heap is ordered
    while ( (nd>1) && (mKeys[mHeap[nd/2]] > mKeys[mHeap[nd]]) )
    {
      swap(nd/2, nd);

      nd /= 2;
    }
  }

  void reorderDownwards(int nd, int heapSize)
  {
    //move down the heap from node nd swapping the elements until
    //the heap is reordered
    while (2*nd <= heapSize)
    {
      int child = 2 * nd;

      //set child to smaller of nd's two children
      if ((child < heapSize) && (mKeys[mHeap[child]] > mKeys[mHeap[child+1]]))
      {
        ++child;
      }

      //if this nd is larger than its child, swap
      if (mKeys[mHeap[nd]] > mKeys[mHeap[child]])
      {
        swap(child, nd);

        //move the current node down the tree
        nd = child;
      }

      else
      {
        break;
      }
    }
  }


public:

  //you must pass the function Object() { [native code] } a reference to the std::vector the PQ
  //will be indexing into and the maximum size of the queue.
  IndexedPriorityQLow(std::vector<KeyType>& keys,
                      int              maxSize):mKeys(keys),
                                                mMaxSize(maxSize),
                                                mSize(0)
  {
    mHeap.assign(maxSize+1, 0);
    mInverseHeap.assign(maxSize+1, 0);
  }

  bool empty()const{return (mSize==0);}

  //to insert an item into the queue it gets added to the end of the heap
  //and then the heap is reordered from the bottom up.
  void insert(const int idx)
  {
    assert (mSize+1 <= mMaxSize);

    ++mSize;

    mHeap[mSize] = idx;

    mInverseHeap[idx] = mSize;

    reorderUpwards(mSize);
  }

  //to get the min item the first element is exchanged with the lowest
  //in the heap and then the heap is reordered from the top down.
  int pop()
  {
    swap(1, mSize);

    reorderDownwards(1, mSize-1);

    return mHeap[mSize--];
  }

  //if the value of one of the client key's changes then call this with
  //the key's index to adjust the queue accordingly
  void changePriority(const int idx)
  {
    reorderUpwards(mInverseHeap[idx]);
  }
};


#endif