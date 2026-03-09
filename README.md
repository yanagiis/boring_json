# Boring JSON

Boring JSON is a lightweight JSON library written in C11, designed specifically for embedded systems. It utilizes metadata descriptors to map C structures directly to JSON values, significantly simplifying the encoding and decoding process without the need for dynamic allocation.

The implementation is heavily inspired by the `json.c` module in Zephyr RTOS.

## Features

- **C11 Compatible**: Uses standard C11 features for compile-time type validation.
- **Metadata-Driven**: Define your data structures once and map them using simple macros.
- **Type-Safe Macros**: Descriptor macros reject incorrect scalar fields, bookkeeping fields, and `char *` string members at compile time, with `_TYPED` variants for nested object and array storage checks.
- **Static Allocation**: Zero dynamic memory usage (`malloc`/`free`) during core operations, making it safe for heap-constrained systems.
- **RFC 8259 Compliant**: Robust number validation and full support for JSON escape sequences.
- **Flexible Writing**: Supports writing to fixed buffers or custom memory writers.
- **Fully Tested**: Extensive unit test suite covering primitives, nested objects, and arrays.

## Getting Started

### Prerequisites

- **CMake** (version 3.12.4 or higher)
- **Ninja** (optional, recommended for fast builds)
- **GCC/Clang** or any C11-compliant compiler

### Build

```bash
mkdir build && cd build
cmake -GNinja ..
ninja
```

To build and run tests:

```bash
cmake -GNinja -DCONFIG_BORING_JSON_TESTING=ON ..
ninja
./boring_json_test
```

## Usage

### 1. Define your structure and metadata

```c
#include "boring_json.h"

struct my_data {
    int id;
    bool active;
    char name[32];
    bool id_exist;
    bool active_exist;
    bool name_exist;
};

static const struct bo_json_obj_attr_desc my_data_attrs[] = {
    BO_JSON_OBJECT_ATTR_INT(struct my_data, id),
    BO_JSON_OBJECT_ATTR_BOOL(struct my_data, active),
    BO_JSON_OBJECT_ATTR_CSTR_ARRAY(struct my_data, name),
};

static const struct bo_json_value_desc my_data_desc = BO_JSON_VALUE_OBJECT(my_data_attrs);
```

By default, object attribute macros infer the presence field as `<member>_exist`. In the example above, `BO_JSON_OBJECT_ATTR_INT(struct my_data, id)` uses `id_exist` automatically.

The object attribute macros validate field types at compile time. For example, `BO_JSON_OBJECT_ATTR_INT(...)` requires an `int` member, inferred or explicit exist fields must be `bool`, nullable `flags_` fields must be `unsigned char`, array `count_` fields must be `size_t`, and `BO_JSON_OBJECT_ATTR_CSTR_ARRAY(...)` requires `char name[N]` storage rather than `char *`. If the inferred `<member>_exist` field is missing, the compiler points at that missing field name; if it exists with the wrong type, the compiler emits the existing `BO_JSON_expected_bool_exist_field` diagnostic token.

When you need a custom bookkeeping field name, use the explicit `_EXIST` escape-hatch macros:

```c
struct custom_presence {
    int id;
    bool has_id;
};

static const struct bo_json_obj_attr_desc custom_presence_attrs[] = {
    BO_JSON_OBJECT_ATTR_INT_EXIST(struct custom_presence, id, has_id),
};
```

The same pattern applies to named forms: `BO_JSON_OBJECT_ATTR_INT_NAMED(struct my_data, id, "identifier")` still infers `id_exist`, while `BO_JSON_OBJECT_ATTR_INT_NAMED_EXIST(...)` lets you override both the JSON key and the exist field explicitly.

For nested objects and arrays, use the `_TYPED` variants when you want compile-time storage checks as well, such as `BO_JSON_OBJECT_ATTR_OBJECT_TYPED(...)`, `BO_JSON_OBJECT_ATTR_ARRAY_TYPED(...)`, `BO_JSON_VALUE_STRUCT_OBJECT_TYPED(...)`, and `BO_JSON_VALUE_STRUCT_ARRAY_TYPED(...)`. These variants validate the object member type or array element type against an explicit C type token.

Untyped object and array macros remain available for backward compatibility, but they still rely on the caller to keep member storage and descriptors aligned.

When you have nested object or array fields, prefer the typed variants so the compiler can validate the storage shape too:

```c
struct child {
    int id;
    bool id_exist;
};

struct parent {
    struct child child;
    bool child_exist;

    int values[4];
    bool values_exist;
    size_t values_count;
};

static const struct bo_json_obj_attr_desc child_attrs[] = {
    BO_JSON_OBJECT_ATTR_INT(struct child, id),
};

static const struct bo_json_obj_attr_desc parent_attrs[] = {
    BO_JSON_OBJECT_ATTR_OBJECT_TYPED(struct parent, child, struct child, child_attrs),
    BO_JSON_OBJECT_ATTR_ARRAY_TYPED(struct parent, values, int, &bo_json_int_desc, 4,
                                    values_count),
};
```

The `_TYPED` variants catch mistakes like:
- using the wrong nested struct type for an object field
- using the wrong element type for an array field
- passing a pointer where a fixed array is required

They still do not prove that `obj_attrs_`, `elem_desc_`, or `capacity_` are semantically correct; those remain part of the caller contract.

### 2. Decode JSON

```c
const char *json = "{\"id\": 42, \"active\": true, \"name\": \"Boring\"}";
struct my_data data = {0};

struct bo_json_error err = bo_json_decode(json, strlen(json), &my_data_desc, &data);
if (err.err == BO_JSON_ERROR_NONE) {
    // data is now populated
}
```

### 3. Encode to JSON

```c
#include "boring_json_simple_writer.h"

char buffer[128];
struct bo_json_simple_writer writer;
bo_json_simple_writer_init(&writer, buffer, sizeof(buffer));

struct bo_json_error err = bo_json_encode(&data, &my_data_desc, &writer.ctx);
if (err.err == BO_JSON_ERROR_NONE) {
    printf("Result: %s\n", buffer);
}
```

## Limitations

- **Predefined Sizes**: Since no dynamic allocation is used, all strings and arrays must have a predefined maximum capacity in their C structure.
- **Fixed Precision**: Scientific notation is supported but converted to standard C `double` or `int64_t`.
- **Recursive Depth**: Deeply nested objects are limited by stack depth.

## TODO

- [ ] Provide more advanced examples for `BO_JSON_OBJECT_ATTR_*_NAMED`.
- [ ] Add a configurable recursion limit for the decoder.

## License

Distributed under the MIT License. See `LICENSE.txt` for more information.

## Acknowledgments

- [cJSON](https://github.com/DaveGamble/cJSON)
- [jsmn](https://github.com/zserge/jsmn.git)
- [Zephyr RTOS](https://github.com/zephyrproject-rtos/zephyr)
- [L. Pereira article](https://tia.mat.br/posts/2020/02/10/optimizing_a_json_serializer.html)
