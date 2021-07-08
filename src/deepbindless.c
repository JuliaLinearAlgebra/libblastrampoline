#include "libblastrampoline_internal.h"

/*
 * Users can force an RTLD_DEEPBIND-capable system to avoid using RTLD_DEEPBIND
 * by setting `LBT_USE_RTLD_DEEPBIND=0` in their environment.  This function
 * returns `0x01` if it will use `RTLD_DEEPBIND` when loading a library, and
 * `0x00` otherwise.
 */
#if defined(LBT_DEEPBINDLESS) || !defined(RTLD_DEEPBIND)
uint8_t use_deepbind = 0x00;
#else
uint8_t use_deepbind = 0x01;
#endif
LBT_DLLEXPORT const uint8_t lbt_get_use_deepbind() {
    return use_deepbind;
}


int lsame_idx = -1;
const void *old_lsame32 = NULL, *old_lsame64 = NULL;
void push_fake_lsame() {
    // Find `lsame_` in our symbol list (if we haven't done so before)
    if (lsame_idx == -1) {
        lsame_idx = find_symbol_idx("lsame_");
        if (lsame_idx == -1) {
            // This is fatal as it signifies a configuration error in our trampoline symbol list
            fprintf(stderr, "Error: Unable to find lsame_ in our symbol list?!\n");
            exit(1);
        }
    }
    
    // Save old values of `lsame_` and `lsame_64_` to our swap location
    old_lsame32 = (*exported_func32_addrs[lsame_idx]);
    old_lsame64 = (*exported_func64_addrs[lsame_idx]);

    // Insert our "fake" lsame in so that we always have a half-functional copy
    (*exported_func32_addrs[lsame_idx]) = &fake_lsame;
    (*exported_func64_addrs[lsame_idx]) = &fake_lsame;
}

void pop_fake_lsame() {
    if (lsame_idx == -1) {
        // Did you call `pop_fake_lsame()` without calling `push_fake_lsame()` first?!
        fprintf(stderr, "pop_fake_lsame() called with invalid `lsame_idx`!\n");
        exit(1);
    }

    (*exported_func32_addrs[lsame_idx]) = old_lsame32;
    (*exported_func64_addrs[lsame_idx]) = old_lsame64;

    old_lsame32 = NULL;
    old_lsame64 = NULL;
}


/* `lsame_` implementation taken from `http://www.netlib.org/clapack/cblas/lsame.c`*/
int fake_lsame(char * ca, char * cb) {
    /* Local variables */
    static int inta, intb, zcode;

    if (*(unsigned char *)ca == *(unsigned char *)cb) {
	    return 1;
    }

    zcode = 'Z';
    inta = *(unsigned char *)ca;
    intb = *(unsigned char *)cb;

    if (zcode == 90 || zcode == 122) {
        if (inta >= 97 && inta <= 122) {
            inta += -32;
        }
        if (intb >= 97 && intb <= 122) {
            intb += -32;
        }
    } else if (zcode == 233 || zcode == 169) {
        if ((inta >= 129 && inta <= 137) || (inta >= 145 && inta <= 153) || (inta >= 162 && inta <= 169)) {
            inta += 64;
        }
        if ((intb >= 129 && intb <= 137) || (intb >= 145 && intb <= 153) || (intb >= 162 && intb <= 169)) {
            intb += 64;
        }
    } else if (zcode == 218 || zcode == 250) {
        if (inta >= 225 && inta <= 250) {
            inta += -32;
        }
        if (intb >= 225 && intb <= 250) {
            intb += -32;
        }
    }
    return inta == intb;
}
