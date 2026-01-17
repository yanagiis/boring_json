# Boring JSON

Boring JSON is a lightweight JSON library written in C99, designed specifically for embedded systems. It utilizes metadata descriptors to map C structures directly to JSON values, significantly simplifying the encoding and decoding process without the need for dynamic allocation.

The implementation is heavily inspired by the `json.c` module in Zephyr RTOS.

## Features

- **C99 Compatible**: Standard C implementation with no modern dependencies.
- **Metadata-Driven**: Define your data structures once and map them using simple macros.
- **Static Allocation**: Zero dynamic memory usage (`malloc`/`free`) during core operations, making it safe for heap-constrained systems.
- **RFC 8259 Compliant**: Robust number validation and full support for JSON escape sequences.
- **Flexible Writing**: Supports writing to fixed buffers or custom memory writers.
- **Fully Tested**: Extensive unit test suite covering primitives, nested objects, and arrays.

## Getting Started

### Prerequisites

- **CMake** (version 3.12.4 or higher)
- **Ninja** (optional, recommended for fast builds)
- **GCC/Clang** or any C99-compliant compiler

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
};

static const struct bo_json_obj_attr_desc my_data_attrs[] = {
    BO_JSON_OBJECT_ATTR_INT(struct my_data, id, id_exist),
    BO_JSON_OBJECT_ATTR_BOOL(struct my_data, active, active_exist),
    BO_JSON_OBJECT_ATTR_CSTR_ARRAY(struct my_data, name, name_exist),
};

static const struct bo_json_value_desc my_data_desc = BO_JSON_VALUE_OBJECT(my_data_attrs);
```

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

- [ ] Remove the naming pattern requirement in struct field declarations (the need for `_exist` fields).
- [ ] Provide more advanced examples for `BO_JSON_OBJECT_ATTR_*_NAMED`.
- [ ] Add a configurable recursion limit for the decoder.

## License

Distributed under the MIT License. See `LICENSE.txt` for more information.

## Acknowledgments

- [cJSON](https://github.com/DaveGamble/cJSON)
- [jsmn](https://github.com/zserge/jsmn.git)
- [Zephyr RTOS](https://github.com/zephyrproject-rtos/zephyr)
- [L. Pereira article](https://tia.mat.br/posts/2020/02/10/optimizing_a_json_serializer.html)
