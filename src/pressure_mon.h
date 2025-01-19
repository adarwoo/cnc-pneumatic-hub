#ifndef PRESSURE_MON_H_
#define PRESSURE_MON_H_
/*
 * pressure_mon.h
 *
 * Created: 07/05/2024 17:08:37
 *  Author: micro
 */
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void pressure_mon_init(void);

opcodes_reply_t pressure_mon_get(void);

#ifdef __cplusplus
}
#endif

#endif /* PRESSURE_MON_H_ */