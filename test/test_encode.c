#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "unity.h"
#include "boring_json.h"
#include "boring_json_simple_writer.h"

static char error_message[256];

void test_encode_bool(void)
{
	const struct {
		bool input;
		const char *expect;
	} testcases[] = {
		{.input = false, .expect = "false"},
		{.input = true, .expect = "true"},
	};

	struct bo_json_simple_writer writer;
	char buf[8];

	for (size_t i = 0; i < BO_ARRAY_SIZE(testcases); i++) {
		sprintf(error_message, "testcase[%zu]", i);

		bo_json_simple_writer_init(&writer, buf, sizeof(buf));
		struct bo_json_error err =
			bo_json_encode(&testcases[i].input, &bo_json_bool_desc, &writer.ctx);

		TEST_ASSERT_EQUAL_MESSAGE(BO_JSON_ERROR_NONE, err.err, error_message);
		TEST_ASSERT_EQUAL_STRING_LEN(testcases[i].expect, buf, strlen(testcases[i].expect));
	}
}

void test_encode_int(void)
{
	const struct {
		size_t input;
		const char *expect;
	} testcases[] = {
		{.input = 0, .expect = "0"},
		{.input = 100, .expect = "100"},
		{.input = -100, .expect = "-100"},
	};

	struct bo_json_simple_writer writer;
	char buf[8];

	for (size_t i = 0; i < BO_ARRAY_SIZE(testcases); i++) {
		sprintf(error_message, "testcase[%zu]", i);

		bo_json_simple_writer_init(&writer, buf, sizeof(buf));
		struct bo_json_error err =
			bo_json_encode(&testcases[i].input, &bo_json_int_desc, &writer.ctx);

		TEST_ASSERT_EQUAL_MESSAGE(BO_JSON_ERROR_NONE, err.err, error_message);
		TEST_ASSERT_EQUAL_STRING_LEN(testcases[i].expect, buf, strlen(testcases[i].expect));
	}
}

void test_encode_double(void)
{
	const struct {
		double input;
		const char *expect;
	} testcases[] = {
		{.input = 0.1, .expect = "0.1"},
		{.input = -100.2, .expect = "-100.2"},
		{.input = 100.3, .expect = "100.3"},
	};

	struct bo_json_simple_writer writer;
	char buf[8];

	for (size_t i = 0; i < BO_ARRAY_SIZE(testcases); i++) {
		sprintf(error_message, "testcase[%zu]", i);

		bo_json_simple_writer_init(&writer, buf, sizeof(buf));
		struct bo_json_error err =
			bo_json_encode(&testcases[i].input, &bo_json_double_desc, &writer.ctx);

		TEST_ASSERT_EQUAL_MESSAGE(BO_JSON_ERROR_NONE, err.err, error_message);
		TEST_ASSERT_EQUAL_STRING_LEN(testcases[i].expect, buf, strlen(testcases[i].expect));
	}
}

void test_encode_cstr(void)
{
	const struct {
		const char *input;
		const char *expect;
	} testcases[] = {
		{.input = "boring json", .expect = "\"boring json\""},
	};

	struct bo_json_simple_writer writer;
	char buf[16];

	for (size_t i = 0; i < BO_ARRAY_SIZE(testcases); i++) {
		sprintf(error_message, "testcase[%zu]", i);

		const struct bo_json_value_desc cstr_desc =
			BO_JSON_VALUE_CSTR(strlen(testcases[i].input));

		bo_json_simple_writer_init(&writer, buf, sizeof(buf));
		struct bo_json_error err =
			bo_json_encode(testcases[i].input, &cstr_desc, &writer.ctx);

		TEST_ASSERT_EQUAL_MESSAGE(BO_JSON_ERROR_NONE, err.err, error_message);
		TEST_ASSERT_EQUAL_STRING_LEN(testcases[i].expect, buf, strlen(testcases[i].expect));
	}
}

struct prims {
	bool f;
	bool f_exist;

	bool t;
	bool t_exist;

	int positive_int;
	bool positive_int_exist;

	int negative_int;
	bool negative_int_exist;

	double positive_double;
	bool positive_double_exist;

	double negative_double;
	bool negative_double_exist;

	char str[32];
	bool str_exist;

	int arr_integer[8];
	bool arr_integer_exist;
	size_t arr_integer_count;

	bool inexistence;
	bool inexistence_exist;
};

static const struct bo_json_obj_attr_desc prims_attrs[] = {
	BO_JSON_OBJECT_ATTR_BOOL(struct prims, f, f_exist),
	BO_JSON_OBJECT_ATTR_BOOL(struct prims, t, t_exist),
	BO_JSON_OBJECT_ATTR_INT(struct prims, positive_int, positive_int_exist),
	BO_JSON_OBJECT_ATTR_INT(struct prims, negative_int, negative_int_exist),
	BO_JSON_OBJECT_ATTR_DOUBLE(struct prims, positive_double, positive_double_exist),
	BO_JSON_OBJECT_ATTR_DOUBLE(struct prims, negative_double, negative_double_exist),
	BO_JSON_OBJECT_ATTR_CSTR_ARRAY(struct prims, str, str_exist),
	BO_JSON_OBJECT_ATTR_ARRAY(struct prims, arr_integer, &bo_json_int_desc, 8,
				  arr_integer_exist, arr_integer_count),
};

static const struct bo_json_value_desc prims_desc = BO_JSON_VALUE_OBJECT(prims_attrs);

void test_encode_object_primitive_types(void)
{
	const struct prims input = {
		.f = false,
		.f_exist = true,
		.t = true,
		.t_exist = true,
		.positive_int = 100,
		.positive_int_exist = true,
		.negative_int = -100,
		.negative_int_exist = true,
		.positive_double = 100.1,
		.positive_double_exist = true,
		.negative_double = -100.1,
		.negative_double_exist = true,
		.str = "boring json",
		.str_exist = true,
		.arr_integer = {0, 1, -1, 2, -2, 3, -3},
		.arr_integer_count = 7,
		.arr_integer_exist = true,
		.inexistence = false,
		.inexistence_exist = false,
	};

	// clang-format off
	const char expect[] = "{"
		"\"f\":false,"
		"\"t\":true,"
		"\"positive_int\":100,"
		"\"negative_int\":-100,"
		"\"positive_double\":100.1,"
		"\"negative_double\":-100.1,"
		"\"str\":\"boring json\","
		"\"arr_integer\":[0,1,-1,2,-2,3,-3]"
	"}";
	// clang-format on

	char actual[sizeof(expect)];

	struct bo_json_error err;

	struct bo_json_simple_writer writer;
	bo_json_simple_writer_init(&writer, actual, sizeof(actual));

	// check encode result
	err = bo_json_encode(&input, &prims_desc, &writer.ctx);
	TEST_ASSERT_EQUAL(BO_JSON_ERROR_NONE, err.err);
	TEST_ASSERT_EQUAL_STRING_LEN(expect, actual, strlen(expect));
}

void test_encode_array_bool(void)
{
	struct array_boolean {
		bool booleans[4];
		size_t booleans_count;
	} input = {.booleans = {true, false, false, true}, .booleans_count = 4};

	const struct bo_json_value_desc array_desc = BO_JSON_VALUE_STRUCT_ARRAY(
		struct array_boolean, booleans, &bo_json_bool_desc, 4, booleans_count);

	// clang-format off
	const char expect[] = "["
		"true,false,false,true"
	"]";
	// clang-format on

	char actual[sizeof(expect)];

	struct bo_json_error err;

	struct bo_json_simple_writer writer;
	bo_json_simple_writer_init(&writer, actual, sizeof(actual));

	// check encode result
	err = bo_json_encode(&input, &array_desc, &writer.ctx);
	TEST_ASSERT_EQUAL(BO_JSON_ERROR_NONE, err.err);

	// check value
	TEST_ASSERT_EQUAL_STRING_LEN(expect, actual, strlen(expect));
}

void test_encode_array_int(void)
{
	struct array_ints {
		int ints[5];
		size_t ints_count;
	} input = {.ints = {0, -100, 101, -9999, 10000}, .ints_count = 5};

	const struct bo_json_value_desc array_desc = BO_JSON_VALUE_STRUCT_ARRAY(
		struct array_ints, ints, &bo_json_int_desc, 5, ints_count);

	// clang-format off
	const char expect[] = "["
		"0,-100,101,-9999,10000"
	"]";
	// clang-format on

	char actual[sizeof(expect)];

	struct bo_json_error err;

	struct bo_json_simple_writer writer;
	bo_json_simple_writer_init(&writer, actual, sizeof(actual));

	// check encode result
	err = bo_json_encode(&input, &array_desc, &writer.ctx);
	TEST_ASSERT_EQUAL(BO_JSON_ERROR_NONE, err.err);

	// check value
	TEST_ASSERT_EQUAL_STRING_LEN(expect, actual, strlen(expect));
}

void test_encode_array_double(void)
{
	struct array_doubles {
		double doubles[5];
		size_t doubles_count;
	} input = {.doubles = {0.0, -100.1, 101.2, -9999.3, 10000.4}, .doubles_count = 5};

	const struct bo_json_value_desc array_desc = BO_JSON_VALUE_STRUCT_ARRAY(
		struct array_doubles, doubles, &bo_json_double_desc, 5, doubles_count);

	// clang-format off
	const char expect[] = "["
		"0,-100.1,101.2,-9999.3,10000.4"
	"]";
	// clang-format on

	char actual[sizeof(expect)];

	struct bo_json_error err;

	struct bo_json_simple_writer writer;
	bo_json_simple_writer_init(&writer, actual, sizeof(actual));

	// check encode result
	err = bo_json_encode(&input, &array_desc, &writer.ctx);
	TEST_ASSERT_EQUAL(BO_JSON_ERROR_NONE, err.err);

	// check value
	TEST_ASSERT_EQUAL_STRING_LEN(expect, actual, strlen(expect));
}

void test_encode_array_cstr(void)
{
	struct array_strs {
		const char strs[5][32];
		size_t strs_count;
	} input = {.strs = {"hello", "boring", "json", "world", "loooooooooooooooooooooooooooong"},
		   .strs_count = 5};

	const struct bo_json_value_desc str_desc = BO_JSON_VALUE_CSTR(32);

	const struct bo_json_value_desc array_desc =
		BO_JSON_VALUE_STRUCT_ARRAY(struct array_strs, strs, &str_desc, 5, strs_count);

	// clang-format off
	const char expect[] = "["
		"\"hello\",\"boring\",\"json\",\"world\",\"loooooooooooooooooooooooooooong\""
	"]";
	// clang-format on

	char actual[sizeof(expect)];

	struct bo_json_error err;

	struct bo_json_simple_writer writer;
	bo_json_simple_writer_init(&writer, actual, sizeof(actual));

	// check encode result
	err = bo_json_encode(&input, &array_desc, &writer.ctx);
	TEST_ASSERT_EQUAL(BO_JSON_ERROR_NONE, err.err);

	// check value
	TEST_ASSERT_EQUAL_STRING_LEN(expect, actual, strlen(expect));
}

struct obj_nest_2 {
	bool f;
	bool f_exist;

	bool t;
	bool t_exist;

	int positive_int;
	bool positive_int_exist;

	int negative_int;
	bool negative_int_exist;

	double positive_double;
	bool positive_double_exist;

	double negative_double;
	bool negative_double_exist;

	char str[32];
	bool str_exist;

	int arr_integer[8];
	bool arr_integer_exist;
	size_t arr_integer_count;

	bool inexistence;
	bool inexistence_exist;
};

struct obj_nest_1 {
	struct obj_nest_2 nest2;
	bool nest2_exist;
};

struct obj_root {
	struct obj_nest_1 nest1;
	bool nest1_exist;
};

static const struct bo_json_obj_attr_desc obj_nest_2_attrs[] = {
	BO_JSON_OBJECT_ATTR_BOOL(struct obj_nest_2, f, f_exist),
	BO_JSON_OBJECT_ATTR_BOOL(struct obj_nest_2, t, t_exist),
	BO_JSON_OBJECT_ATTR_INT(struct obj_nest_2, positive_int, positive_int_exist),
	BO_JSON_OBJECT_ATTR_INT(struct obj_nest_2, negative_int, negative_int_exist),
	BO_JSON_OBJECT_ATTR_DOUBLE(struct obj_nest_2, positive_double, positive_double_exist),
	BO_JSON_OBJECT_ATTR_DOUBLE(struct obj_nest_2, negative_double, negative_double_exist),
	BO_JSON_OBJECT_ATTR_CSTR_ARRAY(struct obj_nest_2, str, str_exist),
	BO_JSON_OBJECT_ATTR_ARRAY(struct obj_nest_2, arr_integer, &bo_json_int_desc, 8,
				  arr_integer_exist, arr_integer_count),
};

static const struct bo_json_obj_attr_desc obj_nest_1_attrs[] = {
	BO_JSON_OBJECT_ATTR_OBJECT(struct obj_nest_1, nest2, obj_nest_2_attrs, nest2_exist),
};

static const struct bo_json_obj_attr_desc obj_root_attrs[] = {
	BO_JSON_OBJECT_ATTR_OBJECT(struct obj_root, nest1, obj_nest_1_attrs, nest1_exist),
};
static const struct bo_json_value_desc obj_root_desc = BO_JSON_VALUE_OBJECT(obj_root_attrs);

void test_encode_object_nest(void)
{
	// clang-format off
	const char expect[] = "{"
		"\"nest1\":{"
			"\"nest2\":{"
				"\"f\":false,"
				"\"t\":true,"
				"\"positive_int\":100,"
				"\"negative_int\":-100,"
				"\"positive_double\":100.1,"
				"\"negative_double\":-100.1,"
				"\"str\":\"boring json\","
				"\"arr_integer\":[0,1,-1,2,-2,3,-3]"
			"}"
		"}"
	"}";
	// clang-format on

	const struct obj_root input = {
		.nest1_exist = true,
		.nest1 =
			{
				.nest2_exist = true,
				.nest2 =
					{
						.f = false,
						.f_exist = true,
						.t = true,
						.t_exist = true,
						.positive_int = 100,
						.positive_int_exist = true,
						.negative_int = -100,
						.negative_int_exist = true,
						.positive_double = 100.1,
						.positive_double_exist = true,
						.negative_double = -100.1,
						.negative_double_exist = true,
						.str = "boring json",
						.str_exist = true,
						.arr_integer = {0, 1, -1, 2, -2, 3, -3},
						.arr_integer_count = 7,
						.arr_integer_exist = true,
						.inexistence = false,
						.inexistence_exist = false,
					},
			},
	};

	char actual[sizeof(expect)];

	struct bo_json_error err;

	struct bo_json_simple_writer writer;
	bo_json_simple_writer_init(&writer, actual, sizeof(actual));

	// check encode result
	err = bo_json_encode(&input, &obj_root_desc, &writer.ctx);
	TEST_ASSERT_EQUAL(BO_JSON_ERROR_NONE, err.err);

	// check value
	TEST_ASSERT_EQUAL_STRING_LEN(expect, actual, strlen(expect));
}
