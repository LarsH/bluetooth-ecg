#include <stdint.h>

#define ECG_BUFFER_USE_TIMESTAMP (0)

typedef struct {
	#if ECG_BUFFER_USE_TIMESTAMP != 0
	uint32_t timestamp;
	#endif
	uint16_t data[1];
} ecg_data_t;

int buffer_put(ecg_data_t *);
int buffer_get(ecg_data_t *);
void buffer_clear(void);
int buffer_isEmpty(void);

void requestNewData(void);
int shouldSendNewData(void);

void setADCstore(int);
int shouldADCstore(void);

