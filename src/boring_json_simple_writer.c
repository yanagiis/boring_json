#include "boring_json_simple_writer.h"
#include "boring_json.h"

#include <string.h>

struct bo_json_error _simple_write(struct bo_json_writer *writer, const void *data, size_t len)
{
	struct bo_json_simple_writer *swriter = (struct bo_json_simple_writer *)writer;
	if (swriter->cap - swriter->len < len) {
		return BO_JSON_ERROR(BO_JSON_ERROR_INSUFFICIENT_SPACE, 0, NULL);
	}

	memcpy(swriter->buf + swriter->len, data, len);
	swriter->len += len;
	return BO_JSON_OK();
}

void bo_json_simple_writer_init(struct bo_json_simple_writer *writer, void *outbuf, size_t cap)
{
	writer->ctx.write = _simple_write;
	writer->buf = outbuf;
	writer->cap = cap;
	writer->len = 0;
}
