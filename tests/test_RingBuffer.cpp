// Tests include: setBufferCapacity, isFull, isEmpty, writeBuffer
// readBuffer, loopBuffer
// Tests are dependent on preceding tested functions
#include <gtest/gtest.h>
#include "../code/RingBuffer.h"

//#include <iostream>
//#include <string>

using namespace std;

class test_RingBuffer : public ::testing::Test {
protected:
	// This function runs only once before any TEST_F function
	static void SetUpTestCase(){
	}

	// This function runs after all TEST_F functions have been executed
	static void TearDownTestCase(){
	}
    
	// this function runs before every TEST_F function
	void SetUp() override {
    }

	// this function runs after every TEST_F function
	void TearDown() override {
	}
};

//creates full buffer of size 20 with random samples ranging from 0 to 249
shared_ptr<RingBuffer> randomBuffer() {
	//make new buffer with cap of 20
	sample_type new_sample;
	shared_ptr<RingBuffer> randB(new RingBuffer);
	randB->setBufferCapacity(20);

	//create random unit8 samples for buffer
	for(int i = 0; i < 20; ++i) {
		new_sample = sample_type(rand()%250); //keep it in range for uint8
		randB->buffer.push_back(new_sample);
	}
	randB->setWriteHead(19); //place write index where it would be if buffer had been filled
	randB->samples_buffered = 20; //capacity is 20 and we filled it so set sb to 20

	return randB;
}

//Creates buffer with ascending integer values from zero to num_cap
//and then will decrease until zero is reached
//ex: [0, 1, 2, 3, 4, 3, 2, 1, 0] where num_cap is 4
shared_ptr<RingBuffer> palindromeBuffer(int num_cap) {
	//make new, loopable buffer
	shared_ptr<RingBuffer> loopB(new RingBuffer);
	int buff_cap = 2*(num_cap) + 1;
	loopB->setBufferCapacity(buff_cap);

	sample_type sample = 0;

	//write the ascending values
	while (int(sample) <= num_cap) {
		loopB->writeSample(sample);
		++sample; //increment sample
	}

	if (sample > 1) {
		sample -= 1;
	}

	//write the descending values
	while (sample > 0) {
		--sample;
		loopB->writeSample(sample);
	}

	return loopB;
}

//Tests setter function for buffer capacity is working properly
TEST_F(test_RingBuffer, Test_setBufferCapacity){
	//Testing empty case. Reserve 10 spots and check capacity
	shared_ptr<RingBuffer> emptyB(new RingBuffer);
	emptyB->setBufferCapacity(10);
	ASSERT_EQ(emptyB->buffer.capacity(), 10); //test actual capacity of buffer vector to ensure it was reserved
	ASSERT_EQ(emptyB->getBufferCapacity(), 10);

	//resize empty buffer and make sure capacity is correct
	emptyB->setBufferCapacity(20);
	ASSERT_EQ(emptyB->buffer.capacity(), 20); //test actual capacity of buffer vector to ensure it was reserved
	ASSERT_EQ(emptyB->getBufferCapacity(), 20);

    //Testing resizing randB to be oversized
	shared_ptr<RingBuffer> randB = randomBuffer();
    randB->setBufferCapacity(40);
	ASSERT_EQ(randB->buffer.capacity(), 40); //test actual capacity of buffer vector to ensure it was reserved
	ASSERT_EQ(randB->getBufferCapacity(), 40); //test buffer cap against expected value
}

//Tests isEmpty() with a full, partially full and empty case
TEST_F(test_RingBuffer, Test_isEmpty){
	//Testing empty case
	shared_ptr<RingBuffer> emptyB(new RingBuffer);
	ASSERT_TRUE(emptyB->isEmpty());

	emptyB->setBufferCapacity(20); //resize
	ASSERT_TRUE(emptyB->isEmpty());

    //Testing full case
	shared_ptr<RingBuffer> randB = randomBuffer();
    bool full_case = randB->isEmpty();
	ASSERT_FALSE(full_case);

	//partially full case
	randB->setBufferCapacity(30); //resize buffer cap
	ASSERT_FALSE(randB->isEmpty());	
}

//Tests isFull() with a full, partially full and empty buffer case
TEST_F(test_RingBuffer, Test_isFull){
	//full buffer case
	shared_ptr<RingBuffer> randB = randomBuffer();
	ASSERT_EQ(randB->getBufferCapacity(), randB->samples_buffered); //sanity check
	ASSERT_TRUE(randB->isFull());

	//partially full case
	randB->setBufferCapacity(30);
	ASSERT_FALSE(randB->isFull());

	//empty case
	shared_ptr<RingBuffer> emptyB(new RingBuffer);
	emptyB->setBufferCapacity(30);
	ASSERT_FALSE(emptyB->isFull());
}

//Tests writeSample() function with an empty buffer, full and partially full case
TEST_F(test_RingBuffer, Test_writeSample){
	//empty case
	shared_ptr<RingBuffer> emptyB(new RingBuffer);
	emptyB->setBufferCapacity(2);
	ASSERT_TRUE(emptyB->writeSample(1)); //write a sample
	ASSERT_TRUE(emptyB->writeSample(3));
	ASSERT_EQ(emptyB->buffer[0], 1); //check sample is there
	ASSERT_EQ(emptyB->buffer[1], 3);
	ASSERT_TRUE(emptyB->isFull()); //make sure it is now full


	//full buffer case - writing to already full buffer
	shared_ptr<RingBuffer> randB = randomBuffer();
	ASSERT_EQ(randB->samples_buffered, randB->getBufferCapacity()); //sanity check for buffer fullness
	int samples_written = randB->samples_buffered; //get the current number of samples buffered
	bool write_to_full = randB->writeSample(5); //try to write one more sample to full buffer
	ASSERT_FALSE(write_to_full); //check write was unsuccessful
	ASSERT_EQ(randB->samples_buffered, samples_written); //check number of samples buffered is unchanged


	//partially full case
	randB->setBufferCapacity(22);
	bool write_one_more = randB->writeSample(2);
	ASSERT_TRUE(write_one_more);
	samples_written++;
	ASSERT_EQ(randB->buffer[20], 2); //check to make sure our value is there
	//buffer = [val1, val2, ...., 2, -]
	
	//Fill buffer with one more write
	ASSERT_TRUE(randB->writeSample(3));
	samples_written++;
	ASSERT_EQ(randB->buffer[21], 3); //make sure the sample is there
	ASSERT_EQ(randB->samples_buffered, randB->getBufferCapacity()); //check samples buffered is correct
	//buffer should be full now
	ASSERT_TRUE(randB->isFull()); //sanity check

	//try to write to now full buffer
	ASSERT_FALSE(randB->writeSample(5));
	ASSERT_EQ(randB->samples_buffered, samples_written); //check buffered value has not changed
}

//Tests readSample() function for an empty buffer, two full buffers - one filled with read and write
//in their starting positions and one with read and write indices part-way into the buffer.
TEST_F(test_RingBuffer, Test_readSample){
	//empty case
	shared_ptr<RingBuffer> buff1(new RingBuffer);
	buff1->setBufferCapacity(2);
	ASSERT_EQ(buff1->readSample(), EMPTY_BUFFER); //check there is nothing to read
	ASSERT_EQ(buff1->samples_buffered, 0); //check samples buffered is still zero

	//full case #1 - read all samples and then add more
	buff1->writeSample(1); //write a sample
	buff1->writeSample(2); //now it's full
	ASSERT_TRUE(buff1->isFull()); //sanity check

	ASSERT_EQ(buff1->readSample(), 1); //read first sample we put in
	ASSERT_EQ(buff1->samples_buffered, 1); //check only one readable sample in buffer left
	ASSERT_EQ(buff1->getReadTail(), 0); //read index should be at first item after reading it

	ASSERT_EQ(buff1->readSample(), 2); //read second sample we put in
	ASSERT_EQ(buff1->getReadTail(), 1);
	ASSERT_EQ(buff1->samples_buffered, 0); //check there are no readable samples left

	//full case #2 - read a sample when read & write indices are not at start
	buff1->setBufferCapacity(3);
	buff1->writeSample(3);
	buff1->writeSample(4);
	buff1->writeSample(5);
	ASSERT_TRUE(buff1->isFull());//sanity check

	ASSERT_EQ(buff1->readSample(), 3); //check 3 is the next sample
	ASSERT_FALSE(buff1->isFull()); //check samples_buffered has been decremented making buffer no longer full

	//try filling it again
	ASSERT_EQ(buff1->readSample(), 4); //4 should still be the next sample
	ASSERT_EQ(buff1->readSample(), 5);
	ASSERT_EQ(buff1->readSample(), EMPTY_BUFFER); //read and write indices are not at start, yet buffer is correctly flagged as empty
}

//Tests loopBuffer() with an empty buffer, partially filled and partially read from buffer, and a full buffer
TEST_F(test_RingBuffer, Test_loopBuffer){
	//empty case.  Will loop buffer of size 2, 3 times which should produce a buffer of size 6
	shared_ptr<RingBuffer> buff1(new RingBuffer);
	buff1->setBufferCapacity(2);
	shared_ptr<RingBuffer> empty_looped = buff1->loopBuffer(4);
	ASSERT_EQ(empty_looped->getBufferCapacity(), 4*2); //test new looped buffer is correct size
	ASSERT_EQ(empty_looped->samples_buffered, 0); //check samples buffered is zero for new looped buffer

	//partially filled case
	buff1->setBufferCapacity(4); //resize to make more room for testing
	buff1->writeSample(2);
	buff1->writeSample(3);
	buff1->readSample();
	buff1->writeSample(4);
	//buffer should now look like: [-, 3, 4, -]

	shared_ptr<RingBuffer> part_fill_loop = buff1->loopBuffer(3); //loop partially filled buffer
	ASSERT_TRUE(part_fill_loop); //ensure it's not a null pointer
	ASSERT_EQ(part_fill_loop->getBufferCapacity(), 2*3); //check buffer cap is set correctly to looping factor times number of readable samples in buff1
	ASSERT_EQ(part_fill_loop->samples_buffered, 2*3); //check samples buffered is correct, ie full buffer
	vector<sample_type> expected_result = {3, 4, 3, 4, 3, 4};
	//test every element to make sure desired samples are there
	for (int i = 0; i < part_fill_loop->samples_buffered; ++i) {
		ASSERT_EQ(part_fill_loop->buffer[i], expected_result[i]);
	}

	//full buffer case
	shared_ptr<RingBuffer> palindrome = palindromeBuffer(3);
	shared_ptr<RingBuffer> looped_pal = palindrome->loopBuffer(2); //loop palidrome twice
	expected_result = {0, 1, 2, 3, 2, 1, 0, 0, 1, 2, 3, 2, 1, 0};
	//test every element to make sure they are the same
	for (int i = 0; i < looped_pal->samples_buffered; ++i) {
		ASSERT_EQ(looped_pal->buffer[i], expected_result[i]); //test buffer has samples desired
	}
	//make sure read index and samples buffered are still at their original values 
	ASSERT_EQ(palindrome->getReadTail(), -1);
	ASSERT_EQ(palindrome->samples_buffered, palindrome->getBufferCapacity());
}