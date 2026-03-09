#include "boring_json.h"

struct wrong_bool_member_type {
	int value;
	_Bool value_exist;
	_Bool value_flags;
};

struct wrong_bool_flags_field_type {
	_Bool value;
	_Bool value_exist;
	_Bool value_flags;
};

struct wrong_int_member_type {
	double value;
	_Bool value_exist;
};

struct wrong_int64_member_type {
	double value;
	_Bool value_exist;
	_Bool value_flags;
};

struct wrong_int64_flags_field_type {
	int64_t value;
	_Bool value_exist;
	_Bool value_flags;
};

struct wrong_double_member_type {
	int value;
	_Bool value_exist;
	_Bool value_flags;
};

struct wrong_double_flags_field_type {
	double value;
	_Bool value_exist;
	_Bool value_flags;
};

struct nested_object {
	int id;
	_Bool id_exist;
};

struct different_nested_object {
	double id;
	_Bool id_exist;
};

struct wrong_exist_field_type {
	int value;
	int value_exist;
};

struct missing_exist_field {
	int value;
};

struct wrong_inferred_exist_type {
	int value;
	int value_exist;
};

struct wrong_flags_field_type {
	int value;
	_Bool value_exist;
	_Bool value_flags;
};

struct wrong_cstr_pointer_type {
	char *value;
	_Bool value_exist;
	_Bool value_flags;
};

struct wrong_cstr_flags_field_type {
	char value[16];
	_Bool value_exist;
	_Bool value_flags;
};

struct wrong_count_field_type {
	int values[4];
	_Bool values_exist;
	int values_count;
	size_t values_count_ok;
	unsigned char values_flags_ok;
	_Bool values_flags;
};

struct wrong_object_flags_field_type {
	struct nested_object value;
	_Bool value_exist;
	_Bool value_flags;
};

struct wrong_typed_object_member_type {
	struct different_nested_object value;
	_Bool value_exist;
	unsigned char value_flags;
};

struct inferred_typed_object_missing_exist {
	struct nested_object value;
};

struct wrong_typed_array_element_type {
	double values[4];
	_Bool values_exist;
	size_t values_count;
	unsigned char values_flags;
};

struct inferred_typed_array_missing_exist {
	int values[4];
	size_t values_count;
	unsigned char values_flags;
};

#if defined(TEST_WRONG_BOOL_MEMBER_TYPE)
static const struct bo_json_obj_attr_desc wrong_bool_member_type_attrs[] = {
	BO_JSON_OBJECT_ATTR_BOOL_EXIST(struct wrong_bool_member_type, value, value_exist),
};
#elif defined(TEST_WRONG_INT_MEMBER_TYPE)
static const struct bo_json_obj_attr_desc wrong_int_member_type_attrs[] = {
	BO_JSON_OBJECT_ATTR_INT_EXIST(struct wrong_int_member_type, value, value_exist),
};
#elif defined(TEST_WRONG_INT64_MEMBER_TYPE)
static const struct bo_json_obj_attr_desc wrong_int64_member_type_attrs[] = {
	BO_JSON_OBJECT_ATTR_INT64_EXIST(struct wrong_int64_member_type, value, value_exist),
};
#elif defined(TEST_WRONG_DOUBLE_MEMBER_TYPE)
static const struct bo_json_obj_attr_desc wrong_double_member_type_attrs[] = {
	BO_JSON_OBJECT_ATTR_DOUBLE_EXIST(struct wrong_double_member_type, value, value_exist),
};
#elif defined(TEST_WRONG_EXIST_FIELD_TYPE)
static const struct bo_json_obj_attr_desc wrong_exist_field_type_attrs[] = {
	BO_JSON_OBJECT_ATTR_INT_EXIST(struct wrong_exist_field_type, value, value_exist),
};
#elif defined(TEST_INFERRED_EXIST_FIELD_MISSING)
static const struct bo_json_obj_attr_desc missing_exist_field_attrs[] = {
	BO_JSON_OBJECT_ATTR_INT(struct missing_exist_field, value),
};
#elif defined(TEST_INFERRED_EXIST_FIELD_WRONG_TYPE)
static const struct bo_json_obj_attr_desc wrong_inferred_exist_field_type_attrs[] = {
	BO_JSON_OBJECT_ATTR_INT(struct wrong_inferred_exist_type, value),
};
#elif defined(TEST_WRONG_FLAGS_FIELD_TYPE)
static const struct bo_json_obj_attr_desc wrong_flags_field_type_attrs[] = {
	BO_JSON_OBJECT_ATTR_INT_OR_NULL_EXIST(struct wrong_flags_field_type, value, value_exist,
					      value_flags),
};
#elif defined(TEST_WRONG_BOOL_OR_NULL_FLAGS_FIELD_TYPE)
static const struct bo_json_obj_attr_desc wrong_bool_or_null_flags_field_type_attrs[] = {
	BO_JSON_OBJECT_ATTR_BOOL_OR_NULL_EXIST(struct wrong_bool_flags_field_type, value,
					       value_exist, value_flags),
};
#elif defined(TEST_WRONG_INT_OR_NULL_FLAGS_FIELD_TYPE)
static const struct bo_json_obj_attr_desc wrong_int_or_null_flags_field_type_attrs[] = {
	BO_JSON_OBJECT_ATTR_INT_OR_NULL_EXIST(struct wrong_flags_field_type, value, value_exist,
					      value_flags),
};
#elif defined(TEST_WRONG_INT64_OR_NULL_FLAGS_FIELD_TYPE)
static const struct bo_json_obj_attr_desc wrong_int64_or_null_flags_field_type_attrs[] = {
	BO_JSON_OBJECT_ATTR_INT64_OR_NULL_EXIST(struct wrong_int64_flags_field_type, value,
						value_exist, value_flags),
};
#elif defined(TEST_WRONG_DOUBLE_OR_NULL_FLAGS_FIELD_TYPE)
static const struct bo_json_obj_attr_desc wrong_double_or_null_flags_field_type_attrs[] = {
	BO_JSON_OBJECT_ATTR_DOUBLE_OR_NULL_EXIST(struct wrong_double_flags_field_type, value,
						 value_exist, value_flags),
};
#elif defined(TEST_CSTR_ARRAY_POINTER_TYPE)
static const struct bo_json_obj_attr_desc wrong_cstr_pointer_type_attrs[] = {
	BO_JSON_OBJECT_ATTR_CSTR_ARRAY_EXIST(struct wrong_cstr_pointer_type, value, value_exist),
};
#elif defined(TEST_CSTR_ARRAY_OR_NULL_POINTER_TYPE)
static const struct bo_json_obj_attr_desc wrong_cstr_array_or_null_pointer_type_attrs[] = {
	BO_JSON_OBJECT_ATTR_CSTR_ARRAY_OR_NULL_EXIST(struct wrong_cstr_flags_field_type, value,
						     value_exist, value_flags),
};
#elif defined(TEST_OBJECT_OR_NULL_WRONG_FLAGS_FIELD_TYPE)
static const struct bo_json_obj_attr_desc nested_object_attrs[] = {
	BO_JSON_OBJECT_ATTR_INT(struct nested_object, id),
};
static const struct bo_json_obj_attr_desc wrong_object_or_null_flags_field_type_attrs[] = {
	BO_JSON_OBJECT_ATTR_OBJECT_OR_NULL_EXIST(struct wrong_object_flags_field_type, value,
						 nested_object_attrs, value_exist, value_flags),
};
#elif defined(TEST_WRONG_COUNT_FIELD_TYPE)
static const struct bo_json_obj_attr_desc wrong_count_field_type_attrs[] = {
	BO_JSON_OBJECT_ATTR_ARRAY_EXIST(struct wrong_count_field_type, values, &bo_json_int_desc, 4,
					values_exist, values_count),
};
#elif defined(TEST_ARRAY_OR_NULL_WRONG_COUNT_FIELD_TYPE)
static const struct bo_json_obj_attr_desc wrong_array_or_null_count_field_type_attrs[] = {
	BO_JSON_OBJECT_ATTR_ARRAY_OR_NULL_EXIST(struct wrong_count_field_type, values,
						&bo_json_int_desc, 4, values_exist, values_count_ok,
						values_flags),
};
#elif defined(TEST_VALUE_STRUCT_WRONG_INT_MEMBER_TYPE)
static const struct bo_json_value_desc wrong_value_struct_int_member_type_desc =
	BO_JSON_VALUE_STRUCT_INT(struct wrong_int_member_type, value);
#elif defined(TEST_VALUE_STRUCT_WRONG_INT64_MEMBER_TYPE)
static const struct bo_json_value_desc wrong_value_struct_int64_member_type_desc =
	BO_JSON_VALUE_STRUCT_INT64(struct wrong_int64_member_type, value);
#elif defined(TEST_VALUE_STRUCT_WRONG_DOUBLE_MEMBER_TYPE)
static const struct bo_json_value_desc wrong_value_struct_double_member_type_desc =
	BO_JSON_VALUE_STRUCT_DOUBLE(struct wrong_double_member_type, value);
#elif defined(TEST_VALUE_STRUCT_CSTR_ARRAY_POINTER_TYPE)
static const struct bo_json_value_desc wrong_value_struct_cstr_array_pointer_type_desc =
	BO_JSON_VALUE_STRUCT_CSTR_ARRAY(struct wrong_cstr_pointer_type, value);
#elif defined(TEST_VALUE_STRUCT_WRONG_COUNT_FIELD_TYPE)
static const struct bo_json_value_desc wrong_value_struct_array_count_field_type_desc =
	BO_JSON_VALUE_STRUCT_ARRAY(struct wrong_count_field_type, values, &bo_json_int_desc, 4,
				   values_count);
#elif defined(TEST_TYPED_OBJECT_WRONG_MEMBER_TYPE)
static const struct bo_json_obj_attr_desc nested_object_attrs[] = {
	BO_JSON_OBJECT_ATTR_INT(struct nested_object, id),
};
static const struct bo_json_obj_attr_desc wrong_typed_object_member_type_attrs[] = {
	BO_JSON_OBJECT_ATTR_OBJECT_TYPED_EXIST(struct wrong_typed_object_member_type, value,
					       struct nested_object, nested_object_attrs,
					       value_exist),
};
#elif defined(TEST_TYPED_OBJECT_INFERRED_EXIST_FIELD_MISSING)
static const struct bo_json_obj_attr_desc nested_object_attrs[] = {
	BO_JSON_OBJECT_ATTR_INT(struct nested_object, id),
};
static const struct bo_json_obj_attr_desc typed_object_missing_exist_field_attrs[] = {
	BO_JSON_OBJECT_ATTR_OBJECT_TYPED(struct inferred_typed_object_missing_exist, value,
					 struct nested_object, nested_object_attrs),
};
#elif defined(TEST_TYPED_OBJECT_OR_NULL_WRONG_MEMBER_TYPE)
static const struct bo_json_obj_attr_desc nested_object_attrs[] = {
	BO_JSON_OBJECT_ATTR_INT(struct nested_object, id),
};
static const struct bo_json_obj_attr_desc wrong_typed_object_or_null_member_type_attrs[] = {
	BO_JSON_OBJECT_ATTR_OBJECT_OR_NULL_TYPED_EXIST(struct wrong_typed_object_member_type, value,
						       struct nested_object, nested_object_attrs,
						       value_exist, value_flags),
};
#elif defined(TEST_TYPED_ARRAY_WRONG_ELEMENT_TYPE)
static const struct bo_json_obj_attr_desc wrong_typed_array_element_type_attrs[] = {
	BO_JSON_OBJECT_ATTR_ARRAY_TYPED_EXIST(struct wrong_typed_array_element_type, values, int,
					      &bo_json_int_desc, 4, values_exist, values_count),
};
#elif defined(TEST_TYPED_ARRAY_INFERRED_EXIST_FIELD_MISSING)
static const struct bo_json_obj_attr_desc inferred_typed_array_missing_exist_attrs[] = {
	BO_JSON_OBJECT_ATTR_ARRAY_TYPED(struct inferred_typed_array_missing_exist, values, int,
					&bo_json_int_desc, 4, values_count),
};
#elif defined(TEST_TYPED_ARRAY_OR_NULL_WRONG_ELEMENT_TYPE)
static const struct bo_json_obj_attr_desc wrong_typed_array_or_null_element_type_attrs[] = {
	BO_JSON_OBJECT_ATTR_ARRAY_OR_NULL_TYPED_EXIST(struct wrong_typed_array_element_type, values,
						      int, &bo_json_int_desc, 4, values_exist,
						      values_count, values_flags),
};
#elif defined(TEST_VALUE_STRUCT_OBJECT_TYPED_WRONG_MEMBER_TYPE)
static const struct bo_json_obj_attr_desc nested_object_attrs[] = {
	BO_JSON_OBJECT_ATTR_INT(struct nested_object, id),
};
static const struct bo_json_value_desc wrong_value_struct_object_typed_member_type_desc =
	BO_JSON_VALUE_STRUCT_OBJECT_TYPED(struct wrong_typed_object_member_type, value,
					  struct nested_object, nested_object_attrs);
#elif defined(TEST_VALUE_STRUCT_ARRAY_TYPED_WRONG_ELEMENT_TYPE)
static const struct bo_json_value_desc wrong_value_struct_array_typed_element_type_desc =
	BO_JSON_VALUE_STRUCT_ARRAY_TYPED(struct wrong_typed_array_element_type, values, int,
					 &bo_json_int_desc, 4, values_count);
#else
static const struct bo_json_value_desc *bo_json_macro_compile_fail_fixture_anchor =
	&bo_json_int_desc;
#endif
