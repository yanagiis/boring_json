#ifndef BORING_JSON_MEM_WRITER_H_
#define BORING_JSON_MEM_WRITER_H_

#include "boring_json.h"

#ifdef __cplusplus
extern "C" {
#endif

struct bo_json_mem_writer {
	struct bo_json_writer ctx;
	size_t max_cap;
	size_t increase_bytes;

	void *buf;
	size_t cap;
	size_t len;
};

void bo_json_mem_writer_init(struct bo_json_mem_writer *writer, size_t increase_bytes,
			     size_t max_cap);
void bo_json_mem_writer_cleanup(struct bo_json_mem_writer *writer);

#ifdef __cplusplus
}
#endif

#endif /* BORING_JSON_MEM_WRITER_H_ */
