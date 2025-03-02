#include <stdint.h>
#include <string.h>

#include "jsonEditor.h"
#include "token.h"

void _objInsertNoDup(struct objToken *obj, union valueToken key, union valueToken value) {
    struct pairToken pair = {
        .type = PAIR,
        .key = key.anyToken,
        .value = value,
    };

    insertPair(obj, pair);
}

int _objInsert(struct objToken *obj, union valueToken key, union valueToken value) {
    if (objFind(obj, key.anyToken.text)) {
        return 1;
    }

    _objInsertNoDup(obj, dupToken(key), dupToken(value));

    return 0;
}

int _objInsertKey(struct objToken *obj, const char *key, union valueToken value) {
    if (objFind(obj, key)) {
        return 1;
    }

    _objInsertNoDup(obj, JSON_STR(strdup(key)), dupToken(value));

    return 0;
}

int _objInsertStr(struct objToken *obj, const char *key, const char *value) {
    if (objFind(obj, key)) {
        return 1;
    }

    _objInsertNoDup(obj, JSON_STR(strdup(key)), (union valueToken) JSON_STR(strdup(value)));

    return 0;
}

int _objInsertBool(struct objToken *obj, const char *key, bool value) {
    return _objInsertKey(obj, key, (union valueToken) {
        .anyToken = {
            .type = (value ? T_TRUE : T_FALSE),
        }
    });
}

// int _objInsertI8(struct objToken *obj, const char *key, int8_t value) {
//     return _objInsert(obj, key, (union valueToken) {
//         .numberToken = {
//             .type = T_INT8,
//             .i8 = value,
//         }
//     });
// }

// int _objInsertI16(struct objToken *obj, const char *key, int16_t value) {
//     return _objInsert(obj, key, (union valueToken) {
//         .numberToken = {
//             .type = T_INT16,
//             .i16 = value,
//         }
//     });
// }

// int _objInsertI32(struct objToken *obj, const char *key, int32_t value) {
//     return _objInsert(obj, key, (union valueToken) {
//         .numberToken = {
//             .type = T_INT32,
//             .i32 = value,
//         }
//     });
// }

// int _objInsertI64(struct objToken *obj, const char *key, int64_t value) {
//     return _objInsert(obj, key, (union valueToken) {
//         .numberToken = {
//             .type = T_INT64,
//             .i64 = value,
//         }
//     });
// }

// int _objInsertU8(struct objToken *obj, const char *key, uint8_t value) {
//     return _objInsert(obj, key, (union valueToken) {
//         .numberToken = {
//             .type = T_UINT8,
//             .u8 = value,
//         }
//     });
// }

// int _objInsertU16(struct objToken *obj, const char *key, uint16_t value) {
//     return _objInsert(obj, key, (union valueToken) {
//         .numberToken = {
//             .type = T_UINT16,
//             .u16 = value,
//         }
//     });
// }

// int _objInsertU32(struct objToken *obj, const char *key, uint32_t value) {
//     return _objInsert(obj, key, (union valueToken) {
//         .numberToken = {
//             .type = T_UINT32,
//             .u32 = value,
//         }
//     });
// }

// int _objInsertU64(struct objToken *obj, const char *key, uint64_t value) {
//     return _objInsert(obj, key, (union valueToken) {
//         .numberToken = {
//             .type = T_UINT64,
//             .u64 = value,
//         }
//     });
// }
