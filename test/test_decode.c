#include <stdbool.h>
#include <string.h>

#include "unity.h"
#include "boring_json.h"

static char error_message[256];

void test_decode_bool(void)
{
	const struct {
		const char *input;
		bool expect;
	} testcases[] = {
		{.input = "false", .expect = false},
		{.input = "true", .expect = true},
	};

	for (size_t i = 0; i < BO_ARRAY_SIZE(testcases); i++) {
		sprintf(error_message, "testcase[%zu]", i);

		const char *input = testcases[i].input;
		const bool expect = testcases[i].expect;
		bool actual = false;

		struct bo_json_error err =
			bo_json_decode(input, strlen(input), &bo_json_bool_desc, &actual);

		// check decode result
		TEST_ASSERT_EQUAL_MESSAGE(BO_JSON_ERROR_NONE, err.err, error_message);

		// check decode value
		TEST_ASSERT_EQUAL_MESSAGE(expect, actual, error_message);
	}
}

void test_decode_int(void)
{
	const struct {
		const char *input;
		int expect;
	} testcases[] = {
		{.input = "0", .expect = 0},
		{.input = "-100", .expect = -100},
		{.input = "100", .expect = 100},
	};

	for (size_t i = 0; i < BO_ARRAY_SIZE(testcases); i++) {
		sprintf(error_message, "testcase[%zu]", i);

		const char *input = testcases[i].input;
		const int expect = testcases[i].expect;
		int actual = false;

		struct bo_json_error err =
			bo_json_decode(input, strlen(input), &bo_json_int_desc, &actual);

		// check decode result
		TEST_ASSERT_EQUAL_MESSAGE(BO_JSON_ERROR_NONE, err.err, error_message);

		// check decode value
		TEST_ASSERT_EQUAL_INT_MESSAGE(expect, actual, error_message);
	}
}

void test_decode_double(void)
{
	const struct {
		const char *input;
		double expect;
	} testcases[] = {
		{.input = "0.1", .expect = 0.1},     {.input = "-100.1", .expect = -100.1},
		{.input = "100.1", .expect = 100.1}, {.input = "-100.2", .expect = -100.2},
		{.input = "100.3", .expect = 100.3},
	};

	for (size_t i = 0; i < BO_ARRAY_SIZE(testcases); i++) {
		sprintf(error_message, "testcase[%zu]", i);

		const char *input = testcases[i].input;
		const int expect = testcases[i].expect;
		int actual = false;

		struct bo_json_error err =
			bo_json_decode(input, strlen(input), &bo_json_int_desc, &actual);

		// check decode result
		TEST_ASSERT_EQUAL_MESSAGE(BO_JSON_ERROR_NONE, err.err, error_message);

		// check decode value
		TEST_ASSERT_EQUAL_DOUBLE_MESSAGE(expect, actual, error_message);
	}
}

void test_decode_cstr(void)
{
	const struct {
		const char *input;
		const char *expect;
		size_t cap;
	} testcases[] = {
		{.input = "\"boring json\"", .expect = "boring json", .cap = 16},
	};

	for (size_t i = 0; i < BO_ARRAY_SIZE(testcases); i++) {
		sprintf(error_message, "testcase[%zu]", i);

		char actual[32];

		const char *input = testcases[i].input;
		const char *expect = testcases[i].expect;
		const struct bo_json_value_desc desc = BO_JSON_VALUE_CSTR(sizeof(actual));

		struct bo_json_error err = bo_json_decode(input, strlen(input), &desc, &actual);

		// check decode result
		TEST_ASSERT_EQUAL_MESSAGE(BO_JSON_ERROR_NONE, err.err, error_message);

		// check decode value
		TEST_ASSERT_EQUAL_STRING_MESSAGE(expect, actual, error_message);
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
				  arr_integer_exist),
};

static const struct bo_json_value_desc prims_desc = BO_JSON_VALUE_OBJECT(prims_attrs);

void test_decode_object_primitive_types(void)
{
	// clang-format off
	const char *input = "{"
		"\"f\": false,"
		"\"t\": true,"
		"\"positive_int\": 100,"
		"\"negative_int\": -100,"
		"\"positive_double\": 100.1,"
		"\"negative_double\": -100.1,"
		"\"str\": \"boring json\","
		"\"arr_integer\": [0,1,-1,2,-2,3,-3]"
	"}";
	// clang-format on

	const struct prims expect = {
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
	struct prims actual = {0};
	struct bo_json_error err;

	// check decode result
	err = bo_json_decode(input, strlen(input), &prims_desc, &actual);
	TEST_ASSERT_EQUAL(BO_JSON_ERROR_NONE, err.err);

	// check value
	TEST_ASSERT_EQUAL(expect.f_exist, actual.f_exist);
	TEST_ASSERT_EQUAL(expect.f, actual.f);
	TEST_ASSERT_EQUAL(expect.t_exist, actual.t_exist);
	TEST_ASSERT_EQUAL(expect.t, actual.t);
	TEST_ASSERT_EQUAL(expect.positive_int_exist, actual.positive_int_exist);
	TEST_ASSERT_EQUAL_INT(expect.positive_int, actual.positive_int);
	TEST_ASSERT_EQUAL(expect.negative_int_exist, actual.negative_int_exist);
	TEST_ASSERT_EQUAL_INT(expect.negative_int, actual.negative_int);
	TEST_ASSERT_EQUAL(expect.positive_double_exist, actual.positive_double_exist);
	TEST_ASSERT_EQUAL_DOUBLE(expect.positive_double, actual.positive_double);
	TEST_ASSERT_EQUAL(expect.negative_double_exist, actual.negative_double_exist);
	TEST_ASSERT_EQUAL_DOUBLE(expect.negative_double, actual.negative_double);
	TEST_ASSERT_EQUAL(expect.str_exist, actual.str_exist);
	TEST_ASSERT_EQUAL_STRING(expect.str, actual.str);
	TEST_ASSERT_EQUAL(expect.arr_integer_exist, actual.arr_integer_exist);
	TEST_ASSERT_EQUAL_INT(expect.arr_integer_count, actual.arr_integer_count);
	TEST_ASSERT_EQUAL_INT_ARRAY(expect.arr_integer, actual.arr_integer,
				    expect.arr_integer_count);
}

void test_decode_array_bool(void)
{
	struct array_boolean {
		bool booleans[4];
		size_t booleans_count;
	};

	// clang-format off
	const char *input = "["
		"true, false, false, true"
	"]";
	// clang-format on

	const struct array_boolean expect = {
		.booleans = {true, false, false, true},
		.booleans_count = 4,
	};
	struct array_boolean actual = {0};

	const struct bo_json_value_desc array_desc =
		BO_JSON_VALUE_STRUCT_ARRAY(struct array_boolean, booleans, &bo_json_bool_desc, 4);

	struct bo_json_error err;

	// check decode result
	err = bo_json_decode(input, strlen(input), &array_desc, &actual);
	TEST_ASSERT_EQUAL(BO_JSON_ERROR_NONE, err.err);

	// check value
	TEST_ASSERT_EQUAL_INT(expect.booleans_count, actual.booleans_count);
	for (size_t i = 0; i < expect.booleans_count; i++) {
		TEST_ASSERT(expect.booleans[i] == actual.booleans[i]);
	}
}

void test_decode_array_int(void)
{
	struct array_int {
		int ints[7];
		size_t ints_count;
	};

	// clang-format off
	const char *input = "["
		"0, 1, -1, 2, -2, 3, -3"
	"]";
	// clang-format on

	const struct array_int expect = {
		.ints = {0, 1, -1, 2, -2, 3, -3},
		.ints_count = 7,
	};
	struct array_int actual = {0};

	const struct bo_json_value_desc array_desc =
		BO_JSON_VALUE_STRUCT_ARRAY(struct array_int, ints, &bo_json_int_desc, 7);

	struct bo_json_error err;

	// check decode result
	err = bo_json_decode(input, strlen(input), &array_desc, &actual);
	TEST_ASSERT_EQUAL(BO_JSON_ERROR_NONE, err.err);

	// check value
	TEST_ASSERT_EQUAL_INT(expect.ints_count, actual.ints_count);
	TEST_ASSERT_EQUAL_INT_ARRAY(&expect, &actual, expect.ints_count);
}

void test_decode_array_double(void)
{
	struct array_double {
		double doubles[7];
		size_t doubles_count;
	};

	// clang-format off
	const char *input = "["
		"0.1, 1.2, -1.3, 2.4, -2.5, 3.6, -3.7"
	"]";
	// clang-format on

	const struct array_double expect = {
		.doubles = {0.1, 1.2, -1.3, 2.4, -2.5, 3.6, -3.7},
		.doubles_count = 7,
	};
	struct array_double actual = {0};

	const struct bo_json_value_desc array_desc =
		BO_JSON_VALUE_STRUCT_ARRAY(struct array_double, doubles, &bo_json_double_desc, 7);

	struct bo_json_error err;

	// check decode result
	err = bo_json_decode(input, strlen(input), &array_desc, &actual);
	TEST_ASSERT_EQUAL(BO_JSON_ERROR_NONE, err.err);

	// check value
	TEST_ASSERT_EQUAL_INT(expect.doubles_count, actual.doubles_count);
	TEST_ASSERT_EQUAL_INT(expect.doubles_count, actual.doubles_count);
	for (size_t i = 0; i < expect.doubles_count; i++) {
		sprintf(error_message, "str[%zu]", i);
		TEST_ASSERT_EQUAL_DOUBLE_MESSAGE(expect.doubles[i], actual.doubles[i],
						 error_message);
	}
}

void test_decode_array_cstr(void)
{
	struct array_cstr {
		char strs[5][8];
		size_t strs_count;
	};

	// clang-format off
	const char *input = "["
		"\"\","
		"\"foo\","
		"\"bar\","
		"\"hello\","
		"\"world!!\","
	"]";
	// clang-format on

	const struct array_cstr expect = {
		.strs =
			{
				[0] = "",
				[1] = "foo",
				[2] = "bar",
				[3] = "hello",
				[4] = "world!!",
			},
		.strs_count = 5,
	};
	struct array_cstr actual = {0};

	const struct bo_json_value_desc cstr_desc = BO_JSON_VALUE_CSTR(8);
	const struct bo_json_value_desc array_desc =
		BO_JSON_VALUE_STRUCT_ARRAY(struct array_cstr, strs, &cstr_desc, 5);

	struct bo_json_error err;

	// check decode result
	err = bo_json_decode(input, strlen(input), &array_desc, &actual);
	TEST_ASSERT_EQUAL(BO_JSON_ERROR_NONE, err.err);

	// check value
	TEST_ASSERT_EQUAL_INT(expect.strs_count, actual.strs_count);
	for (size_t i = 0; i < expect.strs_count; i++) {
		sprintf(error_message, "str[%zu]", i);
		TEST_ASSERT_EQUAL_STRING_MESSAGE(expect.strs[i], &actual.strs[i], error_message);
	}
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
				  arr_integer_exist),
};

static const struct bo_json_obj_attr_desc obj_nest_1_attrs[] = {
	BO_JSON_OBJECT_ATTR_OBJECT(struct obj_nest_1, nest2, obj_nest_2_attrs, nest2_exist),
};

static const struct bo_json_obj_attr_desc obj_root_attrs[] = {
	BO_JSON_OBJECT_ATTR_OBJECT(struct obj_root, nest1, obj_nest_1_attrs, nest1_exist),
};
static const struct bo_json_value_desc obj_root_desc = BO_JSON_VALUE_OBJECT(obj_root_attrs);

void test_decode_object_nest(void)
{
	// clang-format off
	const char *input = "{"
		"\"nest1\": {"
			"\"nest2\": {"
				"\"f\": false,"
				"\"t\": true,"
				"\"positive_int\": 100,"
				"\"negative_int\": -100,"
				"\"positive_double\": 100.1,"
				"\"negative_double\": -100.1,"
				"\"str\": \"boring json\","
				"\"arr_integer\": [0,1,-1,2,-2,3,-3]"
			"}"
		"}"
	"}";
	// clang-format on

	const struct obj_root expect = {
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
	struct obj_root actual;

	struct bo_json_error err;

	// check decode result
	err = bo_json_decode(input, strlen(input), &obj_root_desc, &actual);
	TEST_ASSERT_EQUAL(BO_JSON_ERROR_NONE, err.err);

	// check value
	TEST_ASSERT_EQUAL(expect.nest1_exist, actual.nest1_exist);
	TEST_ASSERT_EQUAL(expect.nest1.nest2_exist, actual.nest1.nest2_exist);

	TEST_ASSERT_EQUAL(expect.nest1.nest2.f_exist, actual.nest1.nest2.f_exist);
	TEST_ASSERT_EQUAL(expect.nest1.nest2.f, actual.nest1.nest2.f);
	TEST_ASSERT_EQUAL(expect.nest1.nest2.t_exist, actual.nest1.nest2.t_exist);
	TEST_ASSERT_EQUAL(expect.nest1.nest2.t, actual.nest1.nest2.t);
	TEST_ASSERT_EQUAL(expect.nest1.nest2.positive_int_exist,
			  actual.nest1.nest2.positive_int_exist);
	TEST_ASSERT_EQUAL_INT(expect.nest1.nest2.positive_int, actual.nest1.nest2.positive_int);
	TEST_ASSERT_EQUAL(expect.nest1.nest2.negative_int_exist,
			  actual.nest1.nest2.negative_int_exist);
	TEST_ASSERT_EQUAL_INT(expect.nest1.nest2.negative_int, actual.nest1.nest2.negative_int);
	TEST_ASSERT_EQUAL(expect.nest1.nest2.positive_double_exist,
			  actual.nest1.nest2.positive_double_exist);
	TEST_ASSERT_EQUAL_DOUBLE(expect.nest1.nest2.positive_double,
				 actual.nest1.nest2.positive_double);
	TEST_ASSERT_EQUAL(expect.nest1.nest2.negative_double_exist,
			  actual.nest1.nest2.negative_double_exist);
	TEST_ASSERT_EQUAL_DOUBLE(expect.nest1.nest2.negative_double,
				 actual.nest1.nest2.negative_double);
	TEST_ASSERT_EQUAL(expect.nest1.nest2.str_exist, actual.nest1.nest2.str_exist);
	TEST_ASSERT_EQUAL_STRING(expect.nest1.nest2.str, actual.nest1.nest2.str);
	TEST_ASSERT_EQUAL(expect.nest1.nest2.arr_integer_exist,
			  actual.nest1.nest2.arr_integer_exist);
	TEST_ASSERT_EQUAL_INT(expect.nest1.nest2.arr_integer_count,
			      actual.nest1.nest2.arr_integer_count);
	TEST_ASSERT_EQUAL_INT_ARRAY(expect.nest1.nest2.arr_integer, actual.nest1.nest2.arr_integer,
				    expect.nest1.nest2.arr_integer_count);
}
