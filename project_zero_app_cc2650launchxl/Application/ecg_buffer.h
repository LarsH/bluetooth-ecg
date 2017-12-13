#include <stdint.h>

typedef struct {
	uint32_t timestamp;
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

