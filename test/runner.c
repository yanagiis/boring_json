#include "unity.h"

extern void test_decode_bool(void);
extern void test_decode_int(void);
extern void test_decode_int_invalid(void);
extern void test_decode_double(void);
extern void test_decode_number_large(void);
extern void test_decode_cstr(void);
extern void test_decode_object_primitive_types(void);
extern void test_decode_array_bool(void);
extern void test_decode_array_int(void);
extern void test_decode_array_double(void);
extern void test_decode_array_cstr(void);
extern void test_decode_object_nest(void);

extern void test_encode_bool(void);
extern void test_encode_int(void);
extern void test_encode_double(void);
extern void test_encode_cstr(void);
extern void test_encode_object_primitive_types(void);
extern void test_encode_array_bool(void);
extern void test_encode_array_int(void);
extern void test_encode_array_double(void);
extern void test_encode_array_cstr(void);
extern void test_encode_object_nest(void);

void setUp(void)
{
}

void tearDown(void)
{
}

int main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;

	UnityBegin("boring json test");

	RUN_TEST(test_decode_bool);
	RUN_TEST(test_decode_int);
	RUN_TEST(test_decode_int_invalid);
	RUN_TEST(test_decode_double);
	RUN_TEST(test_decode_number_large);
	RUN_TEST(test_decode_cstr);
	RUN_TEST(test_decode_object_primitive_types);
	RUN_TEST(test_decode_array_bool);
	RUN_TEST(test_decode_array_int);
	RUN_TEST(test_decode_array_double);
	RUN_TEST(test_decode_array_cstr);
	RUN_TEST(test_decode_object_nest);

	RUN_TEST(test_encode_bool);
	RUN_TEST(test_encode_int);
	RUN_TEST(test_encode_double);
	RUN_TEST(test_encode_cstr);
	RUN_TEST(test_encode_object_primitive_types);
	RUN_TEST(test_encode_array_bool);
	RUN_TEST(test_encode_array_int);
	RUN_TEST(test_encode_array_double);
	RUN_TEST(test_encode_array_cstr);
	RUN_TEST(test_encode_object_nest);

	return (UnityEnd());
}
