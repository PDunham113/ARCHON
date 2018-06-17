# C Style Guide

## General Rules

### Naming conventions
-   Functions and variables should follow `under_score`.
-   Functions and variables from a module should be prefixed with that module's
    three-letter-prefex (e.g. `mdl_under_score`).
-   Private variables and function arguments should be prefixed with a `_`
    (e.g. `_arg`).
-   Typedefs should be postfixed with a `_t` (e.g. `my_type_t`).
-   Global constants (defines, enum values, const variables, etc.) should
    follow `ALL_CAPS`. Constants from modules should be prefixed accordingly.

### Syntax conventions
-   Do this:
    ```c
    if (cond) {

    }
    ```
    Not these:
    ```c
    if(cond) {

    }

    if (cond)
    {

    }
    ```
-   Avoid preprocessor directives unless absolutely necessary. Enums are a Good
    Thing. Use them instead.
-   Typdef your structs and enums.

### Other conventions
-   If you're not sure if you should document something, document it.
-   Don't use floats unless you have to.
-   Stick to fixed-width types, and conserve space.

## Header Files
Header files will feature the following format, at minimum:
```c
/** @file file_name.h
 *  @brief One line module description.
 *
 *  In-depth module description. This can span multiple lines and *should* be
 *  formatted.
 *
 *  @author The Author
 *  @bug One line bug description, followed by detail.
 */

#ifndef FILE_NAME_H
#define FILE_NAME_H

#include <things>

#include "my_things"


/* -- VARIABLES & DEFINITIONS -- */


/* -- CONFIGURATION -- */


/* -- PUBLIC FUNCTIONS -- */

/** @brief One line function description.
 *
 *  In-depth module description. This can span multiple lines and *should* be
 *  formatted.
 *
 *  @param <param_name> One line param description
 *  @returns One line return description (Default to Void.)
 */
void foo(uint16_t _arg);

/* -- PRIVATE FUNCTIONS -- */

#endif /* FILE_NAME_H */
```
*Unused section headers may be omitted.*

Header files should *completely* document the implemented interface, including
function prototypes, typedefs, and public variables. Doxygen-style doc comments
are preferred.

## Source Files
Source files will feature the following format, at minimum:
```c
/** @file UART.c
 *  @brief Driver for ATMEGA328P UART.
 *
 *  This contains the implementation for the interface described in `UART.h`.
 *
 *  @author Patrick Dunham
 *  @bug No known bugs.
 */

#include "UART.h"


/* -- VARIABLES -- */


/* -- PUBLIC FUNCTIONS -- */

void foo(uint16_t _arg) {
  // Do things
	if (_arg) {
    bar = do + things;
  }
}


/* -- PRIVATE FUNCTIONS -- */


```
*Unused section headers may be omitted.*

Doc comments are not necessary, as they should be present in the header file.
