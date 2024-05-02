#ifndef BORING_JSON_PORT_H_
#define BORING_JSON_PORT_H_

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_DEBUG

#include <stdio.h>

#define BO_DEBUG(fmt, ...)                                                                         \
	do {                                                                                       \
		printf("[DBG] " fmt "\n", __VA_ARGS__);                                            \
	} while (0)
#else

#define BO_DEBUG(fmt, ...)

#endif // CONFIG_DEBUG

#ifdef __cplusplus
}
#endif

#endif /* BORING_JSON_PORT_H_ */
