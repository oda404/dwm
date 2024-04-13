/**
 * Copyright Alexandru Olaru
 * See LICENSE file for copyright and license details
 */

#include <ctype.h>
#include <ltils/string/numeric.h>
#include <stddef.h>
#include <string.h>

bool ltils_string_is_numeric10(const char* string)
{
    size_t len = strlen(string);

    for (size_t i = 0; i < len; ++i)
    {
        if (!isdigit(string[i]))
            return false;
    }

    return true;
}
