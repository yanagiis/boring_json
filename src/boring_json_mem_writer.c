#include "boring_json_mem_writer.h"
#include "boring_json.h"

#include <stdlib.h>
#include <string.h>

struct bo_json_error _mem_write(struct bo_json_writer *writer, const void *data, size_t len)
{
	struct bo_json_mem_writer *mem_writer = (struct bo_json_mem_writer *)writer;

	// Check if there's enough space in the buffer, if not realloc
	if (mem_writer->len + len > mem_writer->cap) {
		size_t new_cap;

		new_cap = mem_writer->cap == 0 ? len : mem_writer->cap + mem_writer->increase_bytes;

		// Ensure new_cap does not exceed max_cap
		if (new_cap > mem_writer->max_cap) {
			new_cap = mem_writer->max_cap;
		}

		// Check if the new capacity is enough for the data
		if (mem_writer->len + len > new_cap) {
			return BO_JSON_ERROR(BO_JSON_ERROR_INSUFFICIENT_SPACE, NULL, NULL);
		}

		// Reallocate the buffer to the new capacity
		char *new_buf = realloc(mem_writer->buf, new_cap);
		if (!new_buf) {
			free(mem_writer->buf);
			mem_writer->buf = NULL;
			return BO_JSON_ERROR(BO_JSON_ERROR_INSUFFICIENT_SPACE, NULL, NULL);
		}

		mem_writer->buf = new_buf;
		mem_writer->cap = new_cap;
	}

	// Copy data to the buffer
	memcpy((char *)mem_writer->buf + mem_writer->len, data, len);
	mem_writer->len += len;

	return BO_JSON_OK();
}

void bo_json_mem_writer_init(struct bo_json_mem_writer *writer, size_t increase_bytes,
			     size_t max_cap)
{
	writer->ctx.write = _mem_write;
	writer->max_cap = max_cap;
	writer->increase_bytes = increase_bytes;

	writer->buf = NULL;
	writer->cap = 0;
	writer->len = 0;
}

void bo_json_mem_writer_cleanup(struct bo_json_mem_writer *writer)
{
	free(writer->buf);
	writer->buf = NULL;
	writer->cap = 0;
	writer->len = 0;
}
