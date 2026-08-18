#ifndef __GSM_H__
#define __GSM_H__

#include "types.h"

void GSM_Init(void);
void GSM_SendSMS(u8 *number, u8 *message);

#endif

