#include "libblastrampoline_internal.h"
#include <ctype.h>

const char * env_lowercase(const char * env_name) {
    // Get environment value, if it's not set, return false
    char * env_value = getenv(env_name);
    if (env_value == NULL) {
        return NULL;
    }

    // If it is set, convert to lowercase.
    env_value = strdup(env_value);
    for (size_t idx=0; idx<strlen(env_value); ++idx) {
        env_value[idx] = tolower(env_value[idx]);
    }
    return env_value;
}


uint8_t env_lowercase_match(const char * env_name, const char * value) {
    const char * env_value = env_lowercase(env_name);
    if (env_value == NULL) {
        return 0;
    }

    int ret = strcmp(env_value, value) == 0;
    free((void *)env_value);
    return ret;
}

uint8_t env_lowercase_match_any(const char * env_name, uint32_t num_values, ...) {
    va_list args;
    va_start(args, num_values);

    // Get environment value
    const char * env_value = env_lowercase(env_name);
    if (env_value == NULL) {
        return 0;
    }

    // Search through our varargs for a match
    for (uint32_t idx=0; idx<num_values; idx++ ) {
        const char *value = va_arg(args, const char *);
        if (strcmp(env_value, value) == 0) {
            free((void *)env_value);
            return 1;
        }
    }
    free((void *)env_value);
    return 0;
}

// Check to see if `env_name` matches any "boolean"-like 
uint8_t env_match_bool(const char * env_name, uint8_t default_value) {
    if (env_lowercase_match_any(env_name, 3, "0", "false", "no")) {
        return 0;
    }
    if (env_lowercase_match_any(env_name, 3, "1", "true", "yes")) {
        return 1;
    }
    return default_value;
}
