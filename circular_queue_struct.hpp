///
/// Circular Queue Struct
///
/// A data structure which supprts FIFO queue
/// functionality, and which is implemented as
/// a circular array structure.
///
/// Similar to the circular queue, with the
/// exception that items are not stored as
/// pointers but as values. This proves
/// useful in situation when the item type is
/// a primitive, a struct, or a a simple class.
///
/// This also means that the item type used must
/// declare a default constructor that takes no
/// arguments.
///

#pragma once


namespace DataStructures
{
    template<typename T>class CircularQueueStruct
    {
    private:
        T* _queue;
        int _front, _back, _size, _count;

    public:
        CircularQueueStruct(int size) : _size(size)
        {
            if(size < 1)
            {
                size = 1;
            }
            _queue = new T[size];
            _size = size;
            _front = 0; // this is where we remove
            _back = 0; // this is where we insert
            _count = 0;
        }
        ~CircularQueueStruct()
        {
            delete _queue;
        }

        // return false if the queue is full
        bool Enqueue(T item)
        {
            if(_count >= _size)
            {
                return false;
            }

            _queue[_back++] = item;
            _back *= ((_back == _size) ? 0 : 1);
            _count++;

            return true;
        }

        bool IsEmpty()
        {
            return _count <= 0;
        }

        // this function will always return a value,
        // so it is *strongly* adviced to call `IsEmpty()'
        // first to check, if there actually are items queued.
        T Dequeue()
        {
            int i = _front++;
            _count--;
            if(_count < 0) { _count = 0; }
            _front *= (_front == _size) ? 0 : 1;
            return _queue[i];
        }

        int GetSize()
        {
            return _size;
        }
        int GetCount()
        {
            return _count;
        }

    }; // class CircularQueueStruct
}
