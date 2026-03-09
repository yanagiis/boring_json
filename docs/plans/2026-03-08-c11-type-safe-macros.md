# C11 Type-Safe Macros Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Migrate `boring_json` to C11 and add compile-time type checks for descriptor macros so incorrect member types fail at compile time instead of silently producing broken descriptors.

**Architecture:** Keep the public macro names stable, but route them through new C11 helper macros built on `_Generic` and constant-expression compile-time checks. Validate scalar fields (`bool`, `int`, `int64_t`, `double`), string storage (`char[N]` only), and bookkeeping fields (`exist_`, `flags_`, `count_`) while leaving object/array descriptor compatibility checks explicit where the descriptor argument prevents full inference.

**Tech Stack:** C11, CMake, Unity tests, `_Generic`, `_Static_assert` or `sizeof(char[...])` compile-time assertions

---

### Task 1: Lock the project to C11

**Files:**
- Modify: `CMakeLists.txt`
- Modify: `cmake/linux.cmake`
- Review: `cmake/esp.cmake`
- Review: `cmake/zephyr.cmake`

**Step 1: Write the failing test**

Create a small compile-only probe file locally first:

```c
#include "boring_json.h"

struct probe {
    int value;
    _Bool value_exist;
};

static const struct bo_json_obj_attr_desc attrs[] = {
    BO_JSON_OBJECT_ATTR_INT(struct probe, value, value_exist),
};
```

**Step 2: Run test to verify it fails**

Run: `cmake -GNinja -DCONFIG_BORING_JSON_TESTING=ON -S . -B build && cmake --build build`

Expected: current build still succeeds, but the next task's `_Generic` helpers will not compile until C11 is enabled consistently.

**Step 3: Write minimal implementation**

Set the C standard centrally:

```cmake
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
```

If target-scoped configuration is needed on Linux tests, add:

```cmake
target_compile_features(boring_json PUBLIC c_std_11)
target_compile_features(boring_json_test PRIVATE c_std_11)
```

Do not add GNU-only extensions unless a specific platform requires them.

**Step 4: Run test to verify it passes**

Run: `cmake -GNinja -DCONFIG_BORING_JSON_TESTING=ON -S . -B build && cmake --build build`

Expected: build succeeds under C11 for the Linux target; no C99-only assumptions remain in the public header.

**Step 5: Commit**

```bash
git add CMakeLists.txt cmake/linux.cmake
git commit -m "build: require C11 for type-safe macros"
```

### Task 2: Add reusable compile-time type-check helpers

**Files:**
- Modify: `include/boring_json.h`
- Review: `README.md`

**Step 1: Write the failing test**

Add a negative compile test fixture sketch that should fail once wired into the build:

```c
struct wrong_types {
    double value;
    _Bool value_exist;
};

static const struct bo_json_obj_attr_desc attrs[] = {
    BO_JSON_OBJECT_ATTR_INT(struct wrong_types, value, value_exist),
};
```

**Step 2: Run test to verify it fails**

Run a compile-only command against the fixture after the negative-test target exists.

Expected: compilation fails with a message that the member used with `BO_JSON_OBJECT_ATTR_INT` is not an `int`.

**Step 3: Write minimal implementation**

Add helper macros that remain valid in static initializers, for example:

```c
#define BO_JSON_CT_ASSERT(pred_) (0 * (int)sizeof(char[(pred_) ? 1 : -1]))

#define BO_JSON_MEMBER_TYPE_IS(struct_, member_, type_) \
    _Generic((((struct_ *)0)->member_), type_: 1, default: 0)

#define BO_JSON_MEMBER_OFFSET_TYPED(struct_, member_, type_) \
    (offsetof(struct_, member_) + BO_JSON_CT_ASSERT(BO_JSON_MEMBER_TYPE_IS(struct_, member_, type_)))
```

For `char[N]`, validate the address type instead of the decayed array expression:

```c
#define BO_JSON_MEMBER_IS_CHAR_ARRAY(struct_, member_) \
    _Generic(&((struct_ *)0)->member_, char (*)[sizeof(((struct_ *)0)->member_)]: 1, default: 0)
```

Also add helpers for metadata fields:

```c
#define BO_JSON_EXIST_OFFSET(struct_, exist_) \
    (offsetof(struct_, exist_) + BO_JSON_CT_ASSERT(BO_JSON_MEMBER_TYPE_IS(struct_, exist_, bool)))

#define BO_JSON_FLAGS_OFFSET(struct_, flags_) \
    (offsetof(struct_, flags_) + BO_JSON_CT_ASSERT(BO_JSON_MEMBER_TYPE_IS(struct_, flags_, unsigned char)))

#define BO_JSON_COUNT_OFFSET(struct_, count_) \
    (offsetof(struct_, count_) + BO_JSON_CT_ASSERT(BO_JSON_MEMBER_TYPE_IS(struct_, count_, size_t)))
```

**Step 4: Run test to verify it passes**

Run: `cmake --build build`

Expected: existing correct uses still compile; helper macros are usable in constant-expression initializers.

**Step 5: Commit**

```bash
git add include/boring_json.h
git commit -m "feat: add compile-time type checks for descriptor helpers"
```

### Task 3: Route public attribute macros through the new checks

**Files:**
- Modify: `include/boring_json.h`

**Step 1: Write the failing test**

Add focused compile-fail fixtures for these cases:

```c
// wrong scalar type
BO_JSON_OBJECT_ATTR_BOOL(struct sample, integer_field, integer_field_exist)

// wrong existence marker type
BO_JSON_OBJECT_ATTR_INT(struct sample, int_field, wrong_exist_int)

// wrong nullability flag type
BO_JSON_OBJECT_ATTR_INT_OR_NULL(struct sample, int_field, int_field_exist, wrong_flag_bool)

// pointer passed to CSTR macro
BO_JSON_OBJECT_ATTR_CSTR_ARRAY(struct sample, string_ptr, string_ptr_exist)
```

**Step 2: Run test to verify it fails**

Run the negative compile target.

Expected: each misuse produces a compile-time failure tied to the specific helper macro.

**Step 3: Write minimal implementation**

Update each scalar/string/nullability macro to use typed offset helpers instead of raw `offsetof(...)`:

```c
#define BO_JSON_OBJECT_ATTR_INT_NAMED(struct_, member_, name_, exist_) \
    { \
        .name = name_, \
        .exist_offset = BO_JSON_EXIST_OFFSET(struct_, exist_), \
        .desc = BO_JSON_VALUE_INT_EXT(BO_JSON_MEMBER_OFFSET_TYPED(struct_, member_, int), 0, BO_JSON_FLAGS_NONE), \
    }
```

Apply the same pattern to:
- `BOOL` / `BOOL_OR_NULL`
- `INT64` / `INT64_OR_NULL`
- `DOUBLE` / `DOUBLE_OR_NULL`
- `CSTR_ARRAY` / `CSTR_ARRAY_OR_NULL`
- `BO_JSON_VALUE_STRUCT_*` convenience macros

For object/array macros, at minimum validate:
- `exist_` is `bool`
- `flags_` is `unsigned char`
- `count_` is `size_t`

Document in comments that full member-vs-descriptor structural compatibility is still caller responsibility.

**Step 4: Run test to verify it passes**

Run: `cmake --build build && ./build/boring_json_test`

Expected: runtime tests still pass and valid macro call sites compile without API changes.

**Step 5: Commit**

```bash
git add include/boring_json.h
git commit -m "feat: enforce typed object attribute macros"
```

### Task 4: Add automated negative compile tests

**Files:**
- Create: `test/test_macro_compile_fail.c`
- Modify: `cmake/linux.cmake`

**Step 1: Write the failing test**

Create a fixture with intentionally incorrect macro uses guarded by one macro per scenario:

```c
#if defined(TEST_WRONG_INT_MEMBER)
struct wrong_int_member {
    double value;
    bool value_exist;
};
static const struct bo_json_obj_attr_desc attrs[] = {
    BO_JSON_OBJECT_ATTR_INT(struct wrong_int_member, value, value_exist),
};
#endif
```

**Step 2: Run test to verify it fails**

Add custom commands such as:

```cmake
add_custom_target(boring_json_negative_tests
  COMMAND ${CMAKE_C_COMPILER} -std=c11 -I${BORING_JSON_ROOT_DIR}/include -DTEST_WRONG_INT_MEMBER ${BORING_JSON_ROOT_DIR}/test/test_macro_compile_fail.c -c -o wrong_int.o
  COMMAND ${CMAKE_COMMAND} -E false
)
```

The first draft should fail because the target still treats compile failure as a generic build failure rather than an expected pass condition.

**Step 3: Write minimal implementation**

Wrap the compile commands in a CMake script or `execute_process()` so the build passes only when compilation fails as expected and the compiler output contains the targeted macro name.

**Step 4: Run test to verify it passes**

Run: `cmake --build build --target boring_json_negative_tests`

Expected: target succeeds only because the invalid fixtures fail to compile.

**Step 5: Commit**

```bash
git add test/test_macro_compile_fail.c cmake/linux.cmake
git commit -m "test: add compile-fail coverage for typed macros"
```

### Task 5: Document the new contract

**Files:**
- Modify: `README.md`

**Step 1: Write the failing test**

Identify the undocumented behavior gap: the README currently says the library is C99-compatible and does not mention compile-time macro validation.

**Step 2: Run test to verify it fails**

Manual check: `grep -n "C99 Compatible\|type-safe\|compile-time" README.md`

Expected: README still advertises C99 and has no type-safe macro guidance.

**Step 3: Write minimal implementation**

Update documentation to:
- state the project now targets C11
- explain that descriptor macros validate member types at compile time
- clarify that string members must be `char[N]`, not `char *`
- note that object/array descriptor compatibility is still explicit

Include one example and one misuse example in prose.

**Step 4: Run test to verify it passes**

Run: `grep -n "C11\|compile-time\|char\[N\]" README.md`

Expected: README reflects the new contract.

**Step 5: Commit**

```bash
git add README.md
git commit -m "docs: document C11 type-safe macro requirements"
```

### Task 6: Final verification

**Files:**
- Review: `include/boring_json.h`
- Review: `cmake/linux.cmake`
- Review: `test/test_decode.c`
- Review: `test/test_encode.c`
- Review: `test/test_macro_compile_fail.c`
- Review: `README.md`

**Step 1: Run focused tests**

```bash
cmake -GNinja -DCONFIG_BORING_JSON_TESTING=ON -S . -B build
cmake --build build
./build/boring_json_test
cmake --build build --target boring_json_negative_tests
```

Expected: positive tests pass; negative compile tests succeed by observing compilation failures.

**Step 2: Run regression checks**

If available, also verify embedded integrations still configure:

```bash
grep -n "C_STANDARD\|c_std_11" CMakeLists.txt cmake/*.cmake
```

Expected: C11 requirement is visible and not Linux-only by accident.

**Step 3: Commit**

```bash
git add CMakeLists.txt cmake/linux.cmake include/boring_json.h test/test_macro_compile_fail.c README.md
git commit -m "test: verify C11 typed macro migration"
```
