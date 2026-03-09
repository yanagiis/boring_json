# Typed Object/Array Macros Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Add typed object/array macro variants that can enforce object-member and array-element compatibility at compile time.

**Architecture:** Keep the existing object/array macros for backward compatibility and add `_TYPED` variants that take an explicit member or element type. Reuse the existing C11 `_Generic` assertion helpers so the new API can reject object and array mismatches in static initializers and extend the compile-fail harness to cover the new variants.

**Tech Stack:** C11, CMake, Unity tests, `_Generic`, compile-fail harness

---

### Task 1: Add failing typed-macro scenarios

**Files:**
- Modify: `test/test_macro_compile_fail.c`
- Modify: `cmake/linux.cmake`

**Step 1: Write the failing test**

Add new `TEST_*` scenarios that call not-yet-implemented `_TYPED` object/array macros with mismatched object member types and array element types.

**Step 2: Run test to verify it fails**

Run: `cmake -S . -B build -DCONFIG_BORING_JSON_TESTING=ON && cmake --build build --target boring_json_negative_tests`

Expected: FAIL because the new typed macros are not implemented yet or the harness does not know their expected diagnostic tokens.

**Step 3: Write minimal implementation**

Implement only the typed scenarios and harness entries needed to express the new failures.

**Step 4: Run test to verify it passes**

Run the same negative target again after header support lands.

**Step 5: Commit**

```bash
git add test/test_macro_compile_fail.c cmake/linux.cmake include/boring_json.h
git commit -m "feat: add typed object and array macros"
```

### Task 2: Implement typed object/array macro helpers

**Files:**
- Modify: `include/boring_json.h`

**Step 1: Write the failing test**

Use the new negative scenarios from Task 1 as the red test.

**Step 2: Run test to verify it fails**

Run: `cmake --build build --target boring_json_negative_tests`

Expected: FAIL for the typed mismatch cases.

**Step 3: Write minimal implementation**

Add helper macros for:
- exact object member type checks
- exact array element type checks
- optional compile-time capacity-vs-storage checks for typed arrays

Then add `_TYPED` variants for:
- `BO_JSON_VALUE_STRUCT_OBJECT`
- `BO_JSON_VALUE_STRUCT_ARRAY`
- `BO_JSON_OBJECT_ATTR_OBJECT`
- `BO_JSON_OBJECT_ATTR_OBJECT_OR_NULL`
- `BO_JSON_OBJECT_ATTR_ARRAY`
- `BO_JSON_OBJECT_ATTR_ARRAY_OR_NULL`

**Step 4: Run test to verify it passes**

Run: `cmake --build build --target boring_json_negative_tests && cmake --build build && ./build/boring_json_test`

Expected: negative compile tests pass, runtime suite stays green.

**Step 5: Commit**

```bash
git add include/boring_json.h test/test_macro_compile_fail.c cmake/linux.cmake
git commit -m "feat: enforce typed object and array descriptors"
```

### Task 3: Document the typed API

**Files:**
- Modify: `README.md`

**Step 1: Write the failing test**

Manual gap: README does not mention typed object/array variants.

**Step 2: Run test to verify it fails**

Run: `grep -n "_TYPED\|OBJECT_TYPED\|ARRAY_TYPED" README.md`

Expected: no matches.

**Step 3: Write minimal implementation**

Add a short note describing when to use the new typed variants and what extra guarantees they provide.

**Step 4: Run test to verify it passes**

Run: `grep -n "_TYPED\|OBJECT_TYPED\|ARRAY_TYPED" README.md`

Expected: matches describing the typed API.

**Step 5: Commit**

```bash
git add README.md
git commit -m "docs: describe typed object and array macros"
```
