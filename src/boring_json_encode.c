#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include "boring_json.h"

static struct bo_json_error encode_null(const void *in, const struct bo_json_value_desc *in_desc,
					struct bo_json_writer *writer)
{
	(void)in;
	(void)writer;
	return BO_JSON_ERROR(BO_JSON_ERROR_NOT_SUPPORT, NULL, in_desc);
}

static struct bo_json_error encode_bool(const void *in, const struct bo_json_value_desc *in_desc,
					struct bo_json_writer *writer)
{
	static const char true_str[] = "true";
	static const char false_str[] = "false";

	const bool v = *((bool *)((const char *)in + in_desc->value_offset));
	if (v) {
		return bo_json_writer_write(writer, true_str, sizeof(true_str) - 1);
	} else {
		return bo_json_writer_write(writer, false_str, sizeof(false_str) - 1);
	}
}

static struct bo_json_error encode_int(const void *in, const struct bo_json_value_desc *in_desc,
				       struct bo_json_writer *writer)
{
	char number_str[12];
	int len = snprintf(number_str, sizeof(number_str), "%d",
			   *((const int *)((const char *)in + in_desc->value_offset)));
	if (len < 0) {
		return BO_JSON_ERROR(BO_JSON_ERROR_CONVERT, NULL, in_desc);
	}
	return bo_json_writer_write(writer, number_str, len);
}

static struct bo_json_error encode_int64(const void *in, const struct bo_json_value_desc *in_desc,
					 struct bo_json_writer *writer)
{
	char number_str[24];
	int len = snprintf(number_str, sizeof(number_str), "%" PRIi64,
			   *((const int64_t *)((const char *)in + in_desc->value_offset)));
	if (len < 0) {
		return BO_JSON_ERROR(BO_JSON_ERROR_CONVERT, NULL, in_desc);
	}
	return bo_json_writer_write(writer, number_str, len);
}

static struct bo_json_error encode_double(const void *in, const struct bo_json_value_desc *in_desc,
					  struct bo_json_writer *writer)
{
	char number_str[26];
	int len = snprintf(number_str, sizeof(number_str), "%1.15g",
			   *((const double *)((const char *)in + in_desc->value_offset)));
	if (len < 0) {
		return BO_JSON_ERROR(BO_JSON_ERROR_CONVERT, NULL, in_desc);
	}
	return bo_json_writer_write(writer, number_str, len);
}

static struct bo_json_error encode_cstr(const void *in, const struct bo_json_value_desc *in_desc,
					struct bo_json_writer *writer)
{
	struct bo_json_error err;

	err = bo_json_writer_write(writer, "\"", 1);
	if (err.err != BO_JSON_ERROR_NONE) {
		return err;
	}

	const char *str = (const char *)in + in_desc->value_offset;
	const char *ptr = str;

	while (*ptr != '\0') {
		const char *c = ptr;
		ptr++;

		switch (*c) {
		case '\"':
			err = bo_json_writer_write(writer, "\\\"", 2);
			break;
		case '\\':
			err = bo_json_writer_write(writer, "\\\\", 2);
			break;
		case '\b':
			err = bo_json_writer_write(writer, "\\b", 2);
			break;
		case '\f':
			err = bo_json_writer_write(writer, "\\f", 2);
			break;
		case '\n':
			err = bo_json_writer_write(writer, "\\n", 2);
			break;
		case '\r':
			err = bo_json_writer_write(writer, "\\r", 2);
			break;
		case '\t':
			err = bo_json_writer_write(writer, "\\t", 2);
			break;
		default:
			err = bo_json_writer_write(writer, c, 1);
			break;
		}

		if (err.err != BO_JSON_ERROR_NONE) {
			return err;
		}
	}

	return bo_json_writer_write(writer, "\"", 1);
}

static struct bo_json_error encode_value(const void *in, const struct bo_json_value_desc *in_desc,
					 struct bo_json_writer *writer);

static struct bo_json_error encode_object(const void *in, const struct bo_json_value_desc *in_desc,
					  struct bo_json_writer *writer)
{
	struct bo_json_error err;
	err = bo_json_writer_write(writer, "{", 1);
	if (err.err != BO_JSON_ERROR_NONE) {
		return err;
	}

	size_t count = 0;
	for (size_t i = 0; i < in_desc->object.n_attr_descs; i++) {
		const struct bo_json_obj_attr_desc *desc = &in_desc->object.attr_descs[i];

		if (!(*(const bool *)((const char *)in + in_desc->value_offset +
				      desc->exist_offset))) {
			continue;
		}

		if (count > 0) {
			err = bo_json_writer_write(writer, ",", 1);
			if (err.err != BO_JSON_ERROR_NONE) {
				return err;
			}
		}

		err = bo_json_writer_write(writer, "\"", 1);
		if (err.err != BO_JSON_ERROR_NONE) {
			return err;
		}

		err = bo_json_writer_write(writer, desc->name, strlen(desc->name));
		if (err.err != BO_JSON_ERROR_NONE) {
			return err;
		}

		err = bo_json_writer_write(writer, "\"", 1);
		if (err.err != BO_JSON_ERROR_NONE) {
			return err;
		}

		err = bo_json_writer_write(writer, ":", 1);
		if (err.err != BO_JSON_ERROR_NONE) {
			return err;
		}

		err = encode_value((const char *)in + in_desc->value_offset, &desc->desc, writer);
		if (err.err != BO_JSON_ERROR_NONE) {
			return err;
		}

		count++;
	}

	err = bo_json_writer_write(writer, "}", 1);
	if (err.err != BO_JSON_ERROR_NONE) {
		return err;
	}

	return BO_JSON_OK();
}

static struct bo_json_error encode_array(const void *in, const struct bo_json_value_desc *in_desc,
					 struct bo_json_writer *writer)
{
	struct bo_json_error err;
	err = bo_json_writer_write(writer, "[", 1);
	if (err.err != BO_JSON_ERROR_NONE) {
		return err;
	}

	const size_t count = *((size_t *)((const char *)in + in_desc->array.count_offset));
	const struct bo_json_value_desc *elem_desc = in_desc->array.elem_attr_desc;
	const size_t elem_size = in_desc->array.elem_size;
	const void *array_base = (const char *)in + in_desc->value_offset;

	for (size_t i = 0; i < count; i++) {
		err = encode_value((const char *)array_base + (i * elem_size), elem_desc, writer);
		if (err.err != BO_JSON_ERROR_NONE) {
			return err;
		}

		if (i < count - 1) {
			err = bo_json_writer_write(writer, ",", 1);
			if (err.err != BO_JSON_ERROR_NONE) {
				return err;
			}
		}
	}

	err = bo_json_writer_write(writer, "]", 1);
	if (err.err != BO_JSON_ERROR_NONE) {
		return err;
	}

	return BO_JSON_OK();
}

static struct bo_json_error encode_value(const void *in, const struct bo_json_value_desc *in_desc,
					 struct bo_json_writer *writer)
{
	switch (in_desc->type) {
	case BO_JSON_VALUE_TYPE_NULL:
		return encode_null(in, in_desc, writer);
	case BO_JSON_VALUE_TYPE_BOOL:
		return encode_bool(in, in_desc, writer);
	case BO_JSON_VALUE_TYPE_INT:
		return encode_int(in, in_desc, writer);
	case BO_JSON_VALUE_TYPE_INT64:
		return encode_int64(in, in_desc, writer);
	case BO_JSON_VALUE_TYPE_DOUBLE:
		return encode_double(in, in_desc, writer);
	case BO_JSON_VALUE_TYPE_CSTR:
		return encode_cstr(in, in_desc, writer);
	case BO_JSON_VALUE_TYPE_OBJECT:
		return encode_object(in, in_desc, writer);
	case BO_JSON_VALUE_TYPE_ARRAY:
		return encode_array(in, in_desc, writer);
	default:
		return BO_JSON_ERROR(BO_JSON_ERROR_NOT_SUPPORT, NULL, in_desc);
	}

	return BO_JSON_OK();
}

struct bo_json_error bo_json_encode(const void *in, const struct bo_json_value_desc *in_desc,
				    struct bo_json_writer *writer)
{
	return encode_value(in, in_desc, writer);
}
