# RingBuffer
   

## How To Compile And Run RingBuffer
   
### Dependencies

Both gcc and cmake are required to compile RingBuffer.  The VScode environment in JupyterHub only needs to have the C/C++ extension installed to provide your dependencies.
  
### Compiling the code and running tests

The project can be easily compiled by running the included bash script:
build-n-test.sh

`$ bash build-n-test.sh`  

    
## About This Project
  
The RingBuffer project here provides a cicular array data structure in which sound samples, or other kinds of data, could be stored. The nature of the circular array, or ring buffer, allows reading and writing to happen simultaneously, and at different rates, enabling many applications in the area of audio programming.  Some of those include the looping sound effect this program implements, as well as other audio applications like playback, continuous buffering for large sound files, filtering and editing audio.

The Ring Buffer abstract data type posesses a "pointer" to the last written to slot in the buffer, as well as a read "pointer" which keeps track of the last read element in the buffer.  The concept of a pointer in this case is abstract.  I have chosen to implement them with simple integer variables which track their respective read and write indices.

The sample_type definition provides an easy way to alter the RingBuffer to store the type of data the user requires.  The tests are set up with numbers, integers which are cast to the sample_type, but if necessary, these could be changed to reflect the user's chosen sample_type.

The RingBuffer can be written to with writeSample() and read from with readSample(). If there are no unread samples, readSample() returns a predefined value, EMPTY_BUFFER, informing the user that the buffer is empty.

If the buffer still has available slots for samples, writeBuffer() will be able to write values to the buffer, returning true after a successful write.  If the number of samples buffered equals the buffer capacity, however, it is full and writeSample() will return false.

The functions isEmpty() and isFull() both return true or false values and can be used to determine the status of the buffer.

Loaded buffers can be looped in whole or fractionally using loopBuffer() which takes in a double-type looping factor.  For example, a buffer with 100 unread samples could be looped 2.75 times, producing a buffer of 275 samples.  loopBuffer() would return a smart pointer to that buffer composed of the 100 samples twice followed by the fractional first 75 samples.

In future applications, the looping algortithm could be used to either loop audio files to produce a looping sound effect, or to clip audio, trimming samples off the end.  At the end of calling loopSamples(), the read pointer of the original buffer is back in its original position and samples_buffered is returned to its pre-function value, allowing the buffer to be read again if desired.

loopBuffer() does not start at the first sample in the buffer but instead intelligently starts at the first readable sample.  This avoids looping over old samples that were previously read out of the buffer, or random values in the unused portions of the buffer.

The test file test_RingBuffer.cpp in the tests folder contains a number of tests, each testing a member function of the RingBuffer class.

  
### RingBuffer Attributes
  
- SAMPLE_TYPE: The data type of samples. Defined to be float, but most likely would be Uint 8 in practice with real sound samples.
- _buffer: A vector which stores samples, or sample_type values
- _read_tail: Private. Keeps track of the index of the last read sample. It is initialized to -1 when none have been read.
- _write_head: Private. Keeps track of the index of the last written to buffer element and is initialized to -1.
- _samples_buffered: Keeps track of the number of unread samples in the buffer. Initialized to 0.  
    

## Resources
---
https://web.archive.org/web/20210506093935/https://theaudioprogrammer.com/about-lifo-and-fifo-data-structures-the-basics/ - The archive of the original article I referenced for an introduction to ring buffers.  I did use some code ideas from here but wrote all my own algorithms as this code is incorrect and the structure of the buffer in this example does not perform as it should.

https://www.youtube.com/watch?v=uvD9_Wdtjtw&ab_channel=TechVedas.learn - General understanding.


https://www.youtube.com/watch?v=jNSiZqSQis4&ab_channel=CppNow - Video resource I used early on for general buffer and audio programming understanding.

SDL Library for future development with my RingBuffer class - https://github.com/libsdl-org/SDL

