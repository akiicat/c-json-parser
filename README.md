# Union JSON

Union JSON is a lightweight and extendable JSON library written in C. It provides a unified interface for creating, manipulating, and interacting with JSON data—streamlining your workflow without the headache of managing dynamic memory. The library is also designed with C/C++ compatibility in mind, ensuring a smooth experience across projects.

## Table of Contents

- [Features](#features)
- [Getting Started](#getting-started)
  - [Installation](#installation)
- [Basic Usage](#basic-usage)
  - [Initializing and Printing JSON Values](#initializing-and-printing-json-values)
- [Advanced Usage](#advanced-usage)
  - [Working with Nested Structures](#working-with-nested-structures)
  - [Error Handling](#error-handling)
- [Importing Data](#importing-data)
- [Exporting Data](#exporting-data)
- [Memory Management](#memory-management)
- [Special Considerations](#special-considerations)
- [License](#license)

## Features

- **Intuitive API:** Easily create, update, delete, and print JSON values.
- **Unified Data Type:** A single union type (`union json_t`) is used to handle strings, numbers, booleans, arrays, and objects.
- **No Dynamic Memory for Initialization:** Initialization macros (prefixed with `JSON_`) do not allocate dynamic memory, reducing cleanup overhead.
- **Flexible and Extendable:** Easily adapt and extend the library for both simple and complex JSON manipulation tasks.
- **C/C++ Compatibility:** Designed to work seamlessly in both C and C++ environments.

---

## Getting Started

### Installation

Simply include the header file in your project:

```c
#include "json.h"
```

Ensure that you have the source code of Union JSON in your project directory or installed in your include path.

## Basic Usage

Union JSON uses a unified data type (`union json_t`) for all JSON values. Here are some basic examples to get you started:

### Initializing and Printing JSON Values

Union JSON uses the union type `union json_t` to represent all JSON values. This unified approach allows you to handle strings, integers, arrays, objects, and more without juggling multiple types.

```c
#include "json.h"

int main() {
    // Create a JSON string and print it
    union json_t hello = JSON_STRING("Hello World");
    json_print(hello); // Output: "Hello World"

    // Create and print an integer value
    union json_t number = JSON_INT(10);
    json_print(number); // Output: 10

    // Create and print an empty array and object
    union json_t array = JSON_ARRAY;
    json_print(array); // Output: []

    union json_t object = JSON_OBJECT;
    json_print(object); // Output: {}

    return 0;
}
```

You can reassign initialization macros to variables if the existing value is also an initializer.

```c
union json_t hello;
hello = JSON_STRING("Hello World");
hello = JSON_INT(10);
```

You can also pass them to functions like `json_print`.

```c
json_print(JSON_STRING("Hello World")); // Output: "Hello World"
```

This example shows how the `JSON_` macros work. The macros are designed to simplify value creation, allowing you to use them directly with functions like `json_print()`. This approach reduces the risk of memory leaks when dealing with simple, static JSON data, as it eliminates the need for manual memory cleanup during initialization

### Checking JSON Data Types

Use helper functions to check the type of a JSON value:

```c
union json_t j = JSON_STRING("Hello World");
printf("Is string: %s\n", json_is_string(j)); // Expected: true
printf("Is array: %s\n", json_is_array(JSON_ARRAY)); // Expected: true
printf("Is object: %s\n", json_is_object(JSON_OBJECT)); // Expected: true
```

By using type-checking functions, you ensure that the JSON data you're working with is of the expected type before performing further operations. This is especially useful when dealing with dynamic or user-provided data.

### Constructing JSON Objects

This example demonstrates how to build a JSON object by adding, updating, and removing key-value pairs:

```c
#include "json.h"

int main() {
    // Create an empty JSON object
    union json_t j = JSON_OBJECT;
    json_print(j); // Outputs: {}

    // Add key-value pairs
    json_set(&j, "name", "hello");
    json_set(&j, "age", 1);
    json_set(&j, "level", 3);
    json_print(j); // Outputs: {"name":"hello", "age":1, "level":3}

    // Update existing values
    json_set(&j, "name", "world");
    json_set(&j, "age", "unknown");
    json_set(&j, "level", 5);
    json_print(j); // Outputs: {"name":"world", "age":"unknown", "level":5}

    // Specify JSON type explicitly
    json_set(&j, "name", JSON_STRING("hello world"));
    json_set(&j, "age", JSON_UINT(2));
    json_print(j); // Outputs: {"name":"hello world", "age":2, "level":5}

    // Delete a key-value pair
    json_set(&j, "age", JSON_DELETE); // same as `json_delete(&j, "age")`;
    json_print(j); // Outputs: {"name":"hello world", "level":5}

    // Remove a key but preserve its value
    union json_t name = json_remove(&j, "name");
    json_print(name); // Outputs: "hello world"
    json_clean(&name); // Clean up the removed value
    json_print(j); // Outputs: {"level":5}

    // Final cleanup
    json_clean(&j);
    json_print(j); // Outputs: {}

    return 0;
}
```

You can delete a key from an object by either using` JSON_DELETE` or `JSON_MISSING` with `json_set()`, or by calling `json_delete()`. To preserve the value, use `json_remove()`, but keep in mind that you'll need to manually clean it up later.

The cleanup function `json_clean()` is crucial for preventing memory leaks when working with modified data.

### Working with JSON Arrays

This example demonstrates how to build a JSON object by adding, updating, and removing key-value pairs:

```c
#include "json.h"

int main() {
    // Create an empty JSON array
    union json_t j = JSON_ARRAY;
    json_print(j); // Outputs: []

    // Append elements
    json_append(&j, "apple");
    json_append(&j, "banana");
    json_append(&j, "orange");
    json_append(&j, "peach");
    json_print(j); // Outputs: ["apple", "banana", "orange", "peach"]

    // Update existing values
    json_set(&j, 3, "kiwi");

    // Delete an element (by index)
    json_set(&j, 1, JSON_DELETE); // same as `json_delete(&j, 1)`;
    json_print(j); // Outputs: ["apple", "orange", "kiwi"]

    // Remove an element while preserving its value
    union json_t peach = json_remove(&j, -1);
    json_print(peach); // Outputs: "kiwi"
    json_clean(&peach); // Clean up the removed value
    json_print(j); // Outputs: ["apple", "orange"]

    // Final cleanup
    json_clean(&j);
    json_print(j); // Outputs: []

    return 0;
}
```

Arrays work similarly to objects but use integer indices. Use `json_append()` to add elements, and `json_set()` to update an existing value. Use `json_delete()` or `json_remove()` to remove elements.

### Accessing Values

Retrieve values using the `json_get()` function; it is read-only. If you want to modify the value, use `json_getp()` to get a pointer, and then use `json_update()` to update it.

1. If the returned JSON type is an integer, float, boolean, or null, you can update it directly.
2. However, if the returned JSON type is a string, raw number, object, or array, you must call `json_update()`; otherwise, it may cause a memory leak.

Note: Always use `json_update()` with the return value of `json_getp()` to update the data safely. Alternatively, you can use `json_set()` to perform the update.

**`json_update()` is currently being implemented**

```c
union json_t json_get(union json_t j, const char *key);
union json_t json_get(union json_t j, long int i);
union json_t *json_getp(union json_t j, const char *key);
union json_t *json_getp(union json_t j, long int i);
```

Once you have a pointer to a nested value, you can directly access its fields. For example:

```c
union json_t j = JSON_OBJECT;
json_set(&j, "name", "hello");
json_set(&j, "age", 1);

union json_t *jp = json_get(j, "age");
if (!jp)
    exit(1);
jp->i64 = 2;  // Updates the integer value by rule 1

jp = json_get(j, "name");
if (!jp)
    exit(1);
json_update(jp, JSON_STRING("world"))  // Update to Raw Number by rule 2
```

*Reminder*: Avoid directly modifying values such as strings or numbers that use dynamically allocated memory. Instead, use `json_set()` or `json_update()` to update these types safely.

---

## Advance Usage

### Working with Nested Structures

Here, we show three methods to create a nested object: In-Place, Copying, and Moving.

#### Method 1: In-Place

Create an empty nested object and populate it using `json_get()`:

```c
#include "json.h"

int main() {
    union json_t j = JSON_OBJECT;

    json_set(&j, "level", JSON_OBJECT);
    json_set(json_get(j, "level"), "lucky", 1000);
    json_set(json_get(j, "level"), "speed", 3.14);
    json_print(j); // Outputs: {"level":{"lucky":1000, "speed":3.140000}}

    return 0;
}
```

#### Method 2: Copy JSON Tree

We use `json_set()` with the third argument to pass by value—this performs a deep copy into the parent object.

**Note:** You can build the nested object separately in a helper function for reuse in multiple places. However, you’ll need to clean it up after calling the helper function.

```c
#include "json.h"

union json_t get_level() {
    union json_t level = JSON_OBJECT;
    json_set(&level, "lucky", 1000);
    json_set(&level, "speed", 3.14);
    return level;
}

int main() {
    union json_t j = JSON_OBJECT;
    union json_t level = get_level();

    json_print(j);     // Outputs: {}
    json_print(level); // Outputs: {"lucky":1000, "speed":3.140000}

    // Copy the nested JSON tree
    json_set(&j, "level", level);
    json_print(j);     // Outputs: {"level":{"lucky":1000, "speed":3.140000}}

    // Clean up
    json_clean(j);
    json_clean(level);
    return 0;
}
```

#### Method 3: Move JSON Tree

Calling `json_set()` with a pointer transfers ownership of the nested object.

```c
#include "json.h"

union json_t get_level() {
    union json_t level = JSON_OBJECT;
    json_set(&level, "lucky", 1000);
    json_set(&level, "speed", 3.14);
    return level;
}

int main() {
    union json_t j = JSON_OBJECT;
    union json_t level = get_level();

    json_print(j);     // Outputs: {}
    json_print(level); // Outputs: {"lucky":1000, "speed":3.14}

    // Move the nested JSON tree into the parent object
    json_set(&j, "level", &level);
    json_print(j);     // Outputs: {"level":{"lucky":1000, "speed":3.14}}
    json_print(level); // Outputs: {}

    // Final cleanup (cleaning level is optional)
    json_clean(j);
    return 0;
}
```

### Error Handling

It's important to verify that your operations succeed before proceeding. The following example demonstrates how to safely access and update a JSON key:

```c
#include "json.h"

int main() {
    union json_t j = JSON_OBJECT;
    json_set(&j, "name", "hello");

    // Retrieve a pointer to the value stored under "name".
    union json_t *name_ptr = json_getp(j, "name");
    if (!name_ptr) {
        // If the key "name" doesn't exist, log an error and exit.
        fprintf(stderr, "Error: Key 'name' not found.\n");
        return 1;
    }

    // Safely update the value using json_update.
    json_update(name_ptr, JSON_STRING("world"));
    json_print(j); // Output: {"name":"world"}

    // Clean up the JSON object to free allocated resources.
    json_clean(&j);
    return 0;
}
```

## Importing Data

Union JSON can load JSON data from various sources: strings, file pointers, or file paths.

### From a String: `json_string()`

```c
#include "json.h"

int main() {
    const char *data = "[\"apple\", \"banana\", \"orange\", \"peach\"]";
    union json_t j = json_string(data);
    json_print(j); // Outputs: ["apple", "banana", "orange", "peach"]
    return 0;
}
```

### From a File Pointer: `json_load()`

```c
#include "json.h"

int main() {
    FILE *f = fopen("data.json", "r");
    union json_t j = json_load(f);
    json_print(j); // Outputs the JSON data from the file
    return 0;
}
```

### From a File Path: `json_file()`

```c
#include "json.h"

int main() {
    union json_t j = json_file("/tmp/data.json");
    json_print(j); // Outputs the JSON data from the specified file
    return 0;
}
```

## Exporting Data

### To String

```c
FILE *f = fopen("data.json", "w");
char *str = json_dumps(j, .indent = 4);
free(str);
```

### To File Pointer

```c
FILE *f = fopen("data.json", "w");
json_dump(j, f2, .indent = 4);
```

---

## Memory Management

Understanding memory management is critical when working with dynamic data in C.

A key benefit of the `JSON_` prefixed macros is that they don’t allocate dynamic memory for initialization. However, when you modify JSON data dynamically using functions such as `json_set()` and `json_append()`, you must:

Use `json_clean()` to free any dynamically allocated memory.

Utilize `json_update()` for safely updating complex data types (like strings or objects) to avoid memory leaks.

---

## Special Considerations:

This section highlights potential pitfalls when using Union JSON in different programming contexts. By following these recommendations, you can ensure consistent behavior in both C and C++ projects.

### C vs C++

In C, booleans from <stdbool.h> are treated as integers. To avoid type issues, always use the dedicated macros:

```c
bool flag = true;
json_set(&j, "active", JSON_BOOL(flag));
```

### Direct Access vs. Pointers

Use `json_get()` for read-only access and `json_getp()` when you need to modify nested data. When modifying complex types, ensure that you use `json_update()` to avoid memory leaks.

---

## License

Happy coding with Union JSON!

