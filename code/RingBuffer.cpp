#include "RingBuffer.h"

RingBuffer::RingBuffer() {
    _write_head = -1; //will hold index of next slot to write item into
    _read_tail = -1; //will hold index of next sample to read and return
    samples_buffered = 0; //total number of unread samples in buffer
}

RingBuffer::~RingBuffer() {
}

bool RingBuffer::isEmpty() {
    //buffer is empty when there are no fresh samples to read
    //either all have been read or none have been written at all
    return (samples_buffered == 0);
}

bool RingBuffer::isFull() {
    //full when all samples have been written and none read
    return (samples_buffered == _buffer_capacity);
}

bool RingBuffer::writeSample(sample_type sample){
    //full buffer case
    if (samples_buffered == _buffer_capacity) {
        return false; //we can't write any more samples
    }

    //increment write head to next available slot and write to it
    _write_head = (_write_head + 1)% _buffer_capacity;//if you reach end of buffer, circle back to beginning
    buffer[_write_head] = sample;//place sample
    ++samples_buffered;

    return true;
}

sample_type RingBuffer::readSample() {
    sample_type sample;

   if (samples_buffered == 0) {
       sample = EMPTY_BUFFER;
    }
    else {
        _read_tail = (_read_tail + 1)% _buffer_capacity; //move read tail to next element to be read
        sample = buffer[_read_tail]; //getting the sample to return
        --samples_buffered; //decrease record of buffered samples
    }

    return sample;
}


shared_ptr<RingBuffer> RingBuffer::loopBuffer(double num_times) {
    int new_size = num_times*(this->samples_buffered); //calculating new, larger buffer size

    //store current read index
    int start_i = this->getReadTail();
    //store current num samples in buffer
    int readable_samples = this->samples_buffered;

    //create new buffer with new_size where looped samples will go
    shared_ptr<RingBuffer> loop_b (new RingBuffer);
    loop_b->setBufferCapacity(new_size);

    //Empty buffer case
    if (this->isEmpty()) {
        loop_b->setBufferCapacity(num_times*this->getBufferCapacity()); //set capacity to looping factor times empty capacity
    }
    else { //orig buffer has samples to copy and loop

        //copy values from old buffer to new, looping the copy when you reach the end of the old buffer
        while (loop_b->samples_buffered < new_size) {

            //get each sample from old buffer and reset read and samples_buffered when you reach buffer end
            if (this->samples_buffered == 0) {
                this->_read_tail = start_i;
                this->samples_buffered = readable_samples;
            }
            sample_type sample = this->readSample(); //get sample

            loop_b->writeSample(sample); //copy sample into new buffer
        }

        //in case we want to loop the orig buffer again, we'll reset the read and samples elements
        this->_read_tail = start_i;
        this->samples_buffered = readable_samples;
    }

    return loop_b;
}

void RingBuffer::setBufferCapacity(int capacity) {
    _buffer_capacity = capacity;
    buffer.reserve(capacity); //resizes capacity of vector without adding elements to it.
    //vector gets moved if necessary to new memory
}

//returns private buffer capacity value.
int RingBuffer::getBufferCapacity() {
    return _buffer_capacity;
}

//returns private read tail index
int RingBuffer::getReadTail() {
    return _read_tail;
}

void RingBuffer::setWriteHead(int index) {
    _write_head = index;
}

void RingBuffer::setReadTail(int index) {
    _read_tail = index;
}