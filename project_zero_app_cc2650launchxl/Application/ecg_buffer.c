#include "ecg_buffer.h"
#include <xdc/runtime/Log.h>

#define ECG_BUF_SIZE (64)

static unsigned int writeIdx = 0;
static unsigned int readIdx = 0;
static unsigned int len = 0;

static ecg_data_t buffer_data[ECG_BUF_SIZE];

/**
 * Add a value to the buffer.
 * Returns zero on success, non-zero on failure (buffer full).
 */
int buffer_put(ecg_data_t* value) {
	//Log_info2("Put %u,%u", readIdx, writeIdx);
	if(len >= ECG_BUF_SIZE) {
		/* Buffer is already full */
		return 1;
	}
	memcpy(&buffer_data[writeIdx], value, sizeof(ecg_data_t));
	writeIdx = (writeIdx + 1) % ECG_BUF_SIZE;
	len += 1;

	while(len != (writeIdx + ECG_BUF_SIZE - readIdx)%ECG_BUF_SIZE) {
		/* Fail! */ ;
	}
	return 0;
}

/**
 * Pop a value from the buffer.
 * Returns zero on success, non-zero on failure (buffer empty).
 */
int buffer_get(ecg_data_t *dest) {
	//Log_info2("Get %u,%u", readIdx, writeIdx);
	if(len == 0) {
		/* Buffer is empty */
		return 1;
	}
	memcpy(dest, &buffer_data[readIdx], sizeof(ecg_data_t));
	readIdx = (readIdx + 1) % ECG_BUF_SIZE;
	len -= 1;

	while(len != (writeIdx + ECG_BUF_SIZE - readIdx)%ECG_BUF_SIZE) {
		/* Fail! */ ;
	}

	return 0;
}

int buffer_isEmpty(void) {
	return (len == 0);
}

void buffer_clear(void) {
	writeIdx = 0;
	readIdx = 0;
	len = 0;
}

/* Signaling functions for the buffering */
static int shouldSend = 0;

/**
 * Request ADC thread to post bluetooth data
 * Called from the BT-read callback function.
 */
void requestNewData(void) {
	shouldSend = 1;
}

/**
 * Check if the BT-thread has requested more data.
 * Called from the ADC thread.
 */
int shouldSendNewData(void) {
	int retval;
	retval = shouldSend;
	shouldSend = 0;
	return retval;
}
