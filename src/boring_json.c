#include <errno.h>

#include "boring_json.h"

struct bo_json_value_desc bo_json_null_desc = BO_JSON_VALUE_NULL();
struct bo_json_value_desc bo_json_bool_desc = BO_JSON_VALUE_BOOL();
struct bo_json_value_desc bo_json_int_desc = BO_JSON_VALUE_INT();
struct bo_json_value_desc bo_json_int64_desc = BO_JSON_VALUE_INT64();
struct bo_json_value_desc bo_json_double_desc = BO_JSON_VALUE_DOUBLE();

int bo_json_error_to_errno(const struct bo_json_error *err)
{
	switch (err->err) {
	case BO_JSON_ERROR_NONE:
		return 0;
	case BO_JSON_ERROR_PARTIAL:
		return -EAGAIN;
	case BO_JSON_ERROR_INVALID_JSON:
		return -EINVAL;
	case BO_JSON_ERROR_INSUFFICIENT_SPACE:
		return -ENOMEM;
	case BO_JSON_ERROR_TYPE_NOT_MATCH:
		return -EINVAL;
	case BO_JSON_ERROR_OVERFLOW:
		return -EOVERFLOW;
	case BO_JSON_ERROR_NOT_SUPPORT:
		return -ENOTSUP;
	case BO_JSON_ERROR_CONVERT:
		return -EINVAL;
	}

	return -1;
}
