#ifndef BORING_JSON_SIMPLE_WRITER_H_
#define BORING_JSON_SIMPLE_WRITER_H_

#include "boring_json.h"

#ifdef __cplusplus
extern "C" {
#endif

struct bo_json_simple_writer {
	struct bo_json_writer ctx;
	void *buf;
	size_t cap;
	size_t len;
};

void bo_json_simple_writer_init(struct bo_json_simple_writer *writer, void *outbuf, size_t cap);

#ifdef __cplusplus
}
#endif

#endif /* BORING_JSON_SIMPLE_WRITER_H_ */
