#ifndef _HOST_INTERFACE_H_
#define _HOST_INTERFACE_H_

#include "main.h"

#ifdef __cplusplus
extern "C"
{
#endif

int HostIface_Start();
int HostIface_PutData(uint16_t id, const uint8_t *buff, uint16_t len);
int HostIface_Send();

#ifdef __cplusplus
}
#endif

#endif // _HOST_INTERFACE_H_