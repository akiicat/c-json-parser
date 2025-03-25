#include <gtest/gtest.h>

#include "env.hh"

TEST(JsonUtilTest, JsonStrdup) {
    /* Arrange */
    const char *str1 = "test";

    /* Act */
    char *str2 = json_strdup(str1);

    /* Assert */
    EXPECT_STREQ(str1, str2);

    /* Clean up */
    free(str2);
}

TEST(JsonUtilTest, JsonStrndup) {
    /* Arrange */
    const char *str1 = "test";

    /* Act */
    char *str2 = json_strndup(str1, 3);

    /* Assert */
    EXPECT_STREQ("tes", str2);

    /* Clean up */
    free(str2);
}

TEST(JsonUtilTest, JsonType2Str) {
    for (int i = JLT_MISSING; i <= JLT_LEXER_TOKEN_SIZE; i++) {
        EXPECT_STREQ(lexer_type_str[i], json_lexer_type2str((enum json_lexer_token_type_t)(i))) << "Index: " << i;
    }
    EXPECT_EQ(NULL, json_lexer_type2str((enum json_lexer_token_type_t)(JLT_LEXER_TOKEN_SIZE+1)));
    EXPECT_EQ(NULL, json_lexer_type2str((enum json_lexer_token_type_t)(JLT_MISSING-1)));
}

TEST(JsonUtilTest, JsonDumps) {
    /* Arrange */
    union json_t obj = JSON_OBJECT;
    union json_t arr = JSON_ARRAY;

    /* Act */
    json_append(&arr, 123);
    json_set(&obj, "Array", arr);
    json_append(json_getp(obj, "Array"), 456);

    char *obj_s = json_dumps(obj, .indent = -1);
    char *arr_s = json_dumps(arr, .indent = -1);

    /* Assert */
    EXPECT_STREQ("{\"Array\": [123, 456]}", obj_s);
    EXPECT_STREQ("[123]", arr_s);

    /* Clean */
    free(obj_s);
    free(arr_s);
    json_clean(&obj);
    json_clean(&arr);    
}
