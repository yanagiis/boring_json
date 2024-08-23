#ifndef BORING_JSON_H_
#define BORING_JSON_H_

#include <sys/types.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BO_JSON_ERROR(err_, pos_, desc_)                                                           \
	(struct bo_json_error)                                                                     \
	{                                                                                          \
		.err = err_, .desc = desc_, .pos = pos_                                            \
	}

#define BO_JSON_OK()                                                                               \
	(struct bo_json_error)                                                                     \
	{                                                                                          \
		.err = 0, .desc = NULL, .pos = NULL                                                \
	}

#define BO_ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

enum bo_json_error_code {
	BO_JSON_ERROR_NONE = 0,
	BO_JSON_ERROR_PARTIAL,
	BO_JSON_ERROR_INVALID_JSON,
	BO_JSON_ERROR_INSUFFICIENT_SPACE,
	BO_JSON_ERROR_TYPE_NOT_MATCH,
	BO_JSON_ERROR_OVERFLOW,
	BO_JSON_ERROR_NOT_SUPPORT,
	BO_JSON_ERROR_CONVERT,
};

struct bo_json_error {
	enum bo_json_error_code err;
	const struct bo_json_value_desc *desc;
	const char *pos;
};

enum bo_json_value_type {
	// <JSON type> -> <C type>
	BO_JSON_VALUE_TYPE_NULL = 0, // null -> bool
	BO_JSON_VALUE_TYPE_BOOL,     // bool -> bool
	BO_JSON_VALUE_TYPE_INT,	     // number -> int
	BO_JSON_VALUE_TYPE_DOUBLE,   // number -> double
	BO_JSON_VALUE_TYPE_CSTR,     // string -> char[]
	BO_JSON_VALUE_TYPE_OBJECT,   // object -> struct
	BO_JSON_VALUE_TYPE_ARRAY,    // array -> struct[]
};

#define BO_JSON_VALUE_NULL()                                                                       \
	{                                                                                          \
		.type = BO_JSON_VALUE_TYPE_NULL, .value_offset = 0,                                \
	}

#define BO_JSON_VALUE_BOOL()                                                                       \
	{                                                                                          \
		.type = BO_JSON_VALUE_TYPE_BOOL, .value_offset = 0,                                \
	}

#define BO_JSON_VALUE_INT()                                                                        \
	{                                                                                          \
		.type = BO_JSON_VALUE_TYPE_INT, .value_offset = 0,                                 \
	}

#define BO_JSON_VALUE_DOUBLE()                                                                     \
	{                                                                                          \
		.type = BO_JSON_VALUE_TYPE_DOUBLE, .value_offset = 0,                              \
	}

#define BO_JSON_VALUE_CSTR(capacity_)                                                              \
	{                                                                                          \
		.type = BO_JSON_VALUE_TYPE_CSTR, .value_offset = 0,                                \
		.string = {                                                                        \
			.capacity = capacity_,                                                     \
		},                                                                                 \
	}

#define BO_JSON_VALUE_OBJECT(obj_attrs_)                                                           \
	{                                                                                          \
		.type = BO_JSON_VALUE_TYPE_OBJECT, .value_offset = 0,                              \
		.object = {                                                                        \
			.attr_descs = obj_attrs_,                                                  \
			.n_attr_descs = BO_ARRAY_SIZE(obj_attrs_),                                 \
		},                                                                                 \
	}

// macros for member in struct

#define BO_JSON_VALUE_STRUCT_NULL(struct_, member_)                                                \
	{                                                                                          \
		.type = BO_JSON_VALUE_TYPE_NULL, .value_offset = offsetof(struct_, member_),       \
	}

#define BO_JSON_VALUE_STRUCT_BOOL(struct_, member_)                                                \
	{                                                                                          \
		.type = BO_JSON_VALUE_TYPE_BOOL, .value_offset = offsetof(struct_, member_),       \
	}

#define BO_JSON_VALUE_STRUCT_INT(struct_, member_)                                                 \
	{                                                                                          \
		.type = BO_JSON_VALUE_TYPE_INT, .value_offset = offsetof(struct_, member_),        \
	}

#define BO_JSON_VALUE_STRUCT_DOUBLE(struct_, member_)                                              \
	{                                                                                          \
		.type = BO_JSON_VALUE_TYPE_DOUBLE, .value_offset = offsetof(struct_, member_),     \
	}

#define BO_JSON_VALUE_STRUCT_CSTR_ARRAY(struct_, member_)                                          \
	{                                                                                          \
		.type = BO_JSON_VALUE_TYPE_CSTR, .value_offset = offsetof(struct_, member_),       \
		.string = {                                                                        \
			.capacity = sizeof(((struct_ *)(0))->member_),                             \
		},                                                                                 \
	}

#define BO_JSON_VALUE_STRUCT_OBJECT(struct_, member_, obj_attrs_, n_obj_attrs_)                    \
	{                                                                                          \
		.type = BO_JSON_VALUE_TYPE_OBJECT, .value_offset = offsetof(struct_, member_),     \
		.object = {                                                                        \
			.attr_descs = obj_attrs_,                                                  \
			.n_attr_descs = n_obj_attrs_,                                              \
		},                                                                                 \
	}

#define BO_JSON_VALUE_STRUCT_ARRAY(struct_, member_, elem_desc_, capacity_)                        \
	{                                                                                          \
		.type = BO_JSON_VALUE_TYPE_ARRAY, .value_offset = offsetof(struct_, member_),      \
		.array = {                                                                         \
			.elem_attr_desc = elem_desc_,                                              \
			.elem_size = sizeof(((struct_ *)(0))->member_[0]),                         \
			.capacity = capacity_,                                                     \
			.count_offset = offsetof(struct_, member_##_count),                        \
		},                                                                                 \
	}

#define BO_JSON_OBJECT_ATTR_ARRAY(struct_, member_, elem_desc_, capacity_, exist_)                 \
	{                                                                                          \
		.name = #member_, .exist_offset = offsetof(struct_, exist_),                       \
		.desc = BO_JSON_VALUE_STRUCT_ARRAY(struct_, member_, elem_desc_, capacity_),       \
	}

#define BO_JSON_OBJECT_ATTR_NULL_NAMED(struct_, member_, name_, exist_)                            \
	{                                                                                          \
		.name = name_, .exist_offset = offsetof(struct_, exist_),                          \
		.desc = BO_JSON_VALUE_STRUCT_NULL(struct_, member_),                               \
	}

#define BO_JSON_OBJECT_ATTR_BOOL_NAMED(struct_, member_, name_, exist_)                            \
	{                                                                                          \
		.name = name_, .exist_offset = offsetof(struct_, exist_),                          \
		.desc = BO_JSON_VALUE_STRUCT_BOOL(struct_, member_),                               \
	}

#define BO_JSON_OBJECT_ATTR_INT_NAMED(struct_, member_, name_, exist_)                             \
	{                                                                                          \
		.name = name_, .exist_offset = offsetof(struct_, exist_),                          \
		.desc = BO_JSON_VALUE_STRUCT_INT(struct_, member_),                                \
	}

#define BO_JSON_OBJECT_ATTR_DOUBLE_NAMED(struct_, member_, name_, exist_)                          \
	{                                                                                          \
		.name = name_, .exist_offset = offsetof(struct_, exist_),                          \
		.desc = BO_JSON_VALUE_STRUCT_DOUBLE(struct_, member_),                             \
	}

#define BO_JSON_OBJECT_ATTR_CSTR_ARRAY_NAMED(struct_, member_, name_, exist_)                      \
	{                                                                                          \
		.name = name_, .exist_offset = offsetof(struct_, exist_),                          \
		.desc = BO_JSON_VALUE_STRUCT_CSTR_ARRAY(struct_, member_),                         \
	}

#define BO_JSON_OBJECT_ATTR_OBJECT_NAMED(struct_, member_, obj_attrs_, name_, exist_)              \
	{                                                                                          \
		.name = name_, .exist_offset = offsetof(struct_, exist_),                          \
		.desc = BO_JSON_VALUE_STRUCT_OBJECT(struct_, member_, obj_attrs_,                  \
						    BO_ARRAY_SIZE(obj_attrs_)),                    \
	}

#define BO_JSON_OBJECT_ATTR_ARRAY_NAMED(struct_, member_, elem_desc_, capacity_, name_, exist_)    \
	{                                                                                          \
		.name = name_, .exist_offset = offsetof(struct_, exist_),                          \
		.desc = BO_JSON_VALUE_STRUCT_ARRAY(struct_, member_, elem_desc_, capacity_),       \
	}

#define BO_JSON_OBJECT_ATTR_NULL(struct_, member_, exist_)                                         \
	BO_JSON_OBJECT_ATTR_NULL_NAMED(struct_, member_, #member_, exist_)

#define BO_JSON_OBJECT_ATTR_BOOL(struct_, member_, exist_)                                         \
	BO_JSON_OBJECT_ATTR_BOOL_NAMED(struct_, member_, #member_, exist_)

#define BO_JSON_OBJECT_ATTR_INT(struct_, member_, exist_)                                          \
	BO_JSON_OBJECT_ATTR_INT_NAMED(struct_, member_, #member_, exist_)

#define BO_JSON_OBJECT_ATTR_DOUBLE(struct_, member_, exist_)                                       \
	BO_JSON_OBJECT_ATTR_DOUBLE_NAMED(struct_, member_, #member_, exist_)

#define BO_JSON_OBJECT_ATTR_CSTR_ARRAY(struct_, member_, exist_)                                   \
	BO_JSON_OBJECT_ATTR_CSTR_ARRAY_NAMED(struct_, member_, #member_, exist_)

#define BO_JSON_OBJECT_ATTR_OBJECT(struct_, member_, obj_attrs_, exist_)                           \
	BO_JSON_OBJECT_ATTR_OBJECT_NAMED(struct_, member_, obj_attrs_, #member_, exist_)

struct bo_json_value_desc {
	union {
		struct {
			const struct bo_json_obj_attr_desc *attr_descs;
			size_t n_attr_descs;
		} object;
		struct {
			const struct bo_json_value_desc *elem_attr_desc;
			size_t elem_size;
			size_t capacity;
			size_t count_offset;
		} array;
		struct {
			size_t capacity;
		} string;
	};
	size_t value_offset;
	enum bo_json_value_type type;
};

struct bo_json_obj_attr_desc {
	struct bo_json_value_desc desc;
	const char *name;
	size_t exist_offset;
};

/* primitives types of boring json */
extern struct bo_json_value_desc bo_json_null_desc;
extern struct bo_json_value_desc bo_json_bool_desc;
extern struct bo_json_value_desc bo_json_int_desc;
extern struct bo_json_value_desc bo_json_double_desc;

/**
 * @brief Decode a JSON string into a structured C data object based on a provided description.
 *
 * This function takes a JSON string input, along with a description of the expected structure of
 * the JSON data, and decodes it into a structured C data object. The description is provided in the
 * form of a `bo_json_value_desc` struct, which defines the expected value types in the JSON data.
 *
 * @param[in] in The JSON string to decode.
 * @param[in] in_len The length of the input JSON string.
 * @param[in] desc A pointer to a `bo_json_value_desc` struct that describes the expected structure
 * of the JSON data.
 * @param[out] out A pointer to the output data object where the decoded JSON data will be stored.
 * @return A `struct bo_json_error` object indicating the success or failure of the decoding
 * process. If successful, the error code will be `BO_JSON_ERROR_NONE`.
 */
struct bo_json_error bo_json_decode(const char *in, const size_t in_len,
				    const struct bo_json_value_desc *desc, void *out);

struct bo_json_writer {
	struct bo_json_error (*write)(struct bo_json_writer *writer, const void *data, size_t len);
};

static inline struct bo_json_error bo_json_writer_write(struct bo_json_writer *writer,
							const void *data, size_t len)
{
	return writer->write(writer, data, len);
}

/**
 * Encodes a JSON value into a JSON string and writes it to the provided JSON writer.
 *
 * This function takes a JSON value and its description, encodes it into a JSON string, and writes
 * it to the provided JSON writer.
 *
 * @param[in] in The JSON value to encode.
 * @param[in] in_desc The description of the JSON value.
 * @param writer The JSON writer to write the encoded JSON string to.
 * @return A struct bo_json_error indicating any errors that occurred during encoding.
 */
struct bo_json_error bo_json_encode(const void *in, const struct bo_json_value_desc *in_desc,
				    struct bo_json_writer *writer);

int bo_json_error_to_errno(const struct bo_json_error *err);

#ifdef __cplusplus
}
#endif

#endif /* BORING_JSON_H_ */
