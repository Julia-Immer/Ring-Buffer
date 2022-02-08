#ifndef RINGBUFFER_H__
#define RINGBUFFER_H__

#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <cstdint>


#define sample_type uint8_t 
#define EMPTY_BUFFER 255

using namespace std;

class RingBuffer {
    private:
        int _write_head; //head pointer for writing to the buffer
        int _read_tail; //tail pointer which reads from the cached audio data in buffer
        int _buffer_capacity; //the total capacity for samples in the buffer

    public:
        vector<sample_type> buffer; //stores samples
        int samples_buffered;  //number of samples in buffer which have not been read

        //Constructor initializes write head, read tail to -1, and samples_buffered to 0
        RingBuffer();
        ~RingBuffer();

        //Returns true if buffer is empty, containing no samples
        //or all possible samples have been read
        bool isEmpty();

        //Returns true if buffer holds maximum number of unread samples
        //maximun number of samples is the buffer's capacity
        bool isFull();

        //Writes a sample of sample_type to buffer. Returns true if successful.
        //Returns false if buffer is full already, ie samples_buffered equals _buffer_capacity
        //samples_buffered is incremented after write
        bool writeSample(sample_type sample);

        //readSample fetches the next sample in the buffer and returns it
        //If there are no fresh samples, because all have been read or none
        //have been written, it returns the value EMPTY_BUFFER
        sample_type readSample();
        

        // Loops a buffer for a given, possibly decimal looping factor called num_times. 
        // A smart pointer to a new, RingBuffer containing the looped samples is returned.
        // _read_head and samples_buffered for the original buffer are
        // returned to the values they had when the function was called.
        // Returned looping RingBuffer will have _write_head in last position
        // and samples_buffered will reflect buffer's full status
        shared_ptr<RingBuffer> loopBuffer(double num_times);

        //set the number of samples a buffer can take in
        //function reserves the given capacity for the buffer vector
        void setBufferCapacity(int capacity);

        //returns private buffer capacity value.
        int getBufferCapacity();

        void setWriteHead(int index); //set private write head value

        void setReadTail(int index); //set private read tail. used in tests

        //returns value of private read tail index
        int getReadTail();

};

#endif //RINGBUFFER_H__