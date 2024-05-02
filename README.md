## About The Project

Boring JSON is a JSON library written in C99.

There are numerous JSON libraries available for embedded systems. These libraries are robust and user-friendly.
However, writing and managing JSON parsing and generation can be tedious and error-prone.

Boring JSON aims to address this by utilizing metadata to map C types to JSON values, thereby simplifying the encoding and decoding process.

The Boring JSON implementation is heavily inspired by json.c in Zephyr RTOS.

## Getting Started

**TODO**

### Prerequisites

- CMake

### build

```
mkdir build && cd build
cmake -GNinja ..
```

## Features

- Compatible with C99
- Small code footprint
- Library code is covered with unit-tests

## Limitations

- Does not support encoding and decoding null values.
- Does not use dynamic allocation, meaning all fields need to have a predefined maximum size.

## Usage

**TODO**

Examples:

- Decode: [test_decode.c](./test/test_decode.c)
- Encode: [test_encode.c](./test/test_encode.c)

<p align="right">(<a href="#readme-top">back to top</a>)</p>

## TODO

- [ ] Remove the naming pattern requirement in struct field declarations. Currently, if you have a test field in a JSON object, you need both test and test_exist fields in your struct.
- [ ] Provide examples for BO_JSON_OBJECT_ATTR_*_NAMED.
- [ ] Add a maximum limit for recursive calls to avoid stack overflow when decoding non-existent fields in the descriptor.

## License

Distributed under the MIT License. See `LICENSE.txt` for more information.

<p align="right">(<a href="#readme-top">back to top</a>)</p>

## Acknowledgments

- [cJSON](https://github.com/DaveGamble/cJSON)
- [jsmn](https://github.com/zserge/jsmn.git)
- [Zephyr RTOS](https://github.com/zephyrproject-rtos/zephyr)
- [L. Pereira article](https://tia.mat.br/posts/2020/02/10/optimizing_a_json_serializer.html)
