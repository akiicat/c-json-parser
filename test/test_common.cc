#include <cstddef>
#include <gtest/gtest.h>

#include "env.hh"
#include "json.h"
#include "json.hh"

TEST(JsonCommonTest, JsonDupText) {
    /* Arrange */
    union json_t j = JSON_STRING("test");

    /* Act */
    union json_t res = json_dup(j);

    /* Assert */
    EXPECT_STREQ(j.tok.text, res.tok.text);

    /* Clean up */
    EXPECT_EQ(JT_STRING, j.type);
    free(res.tok.text);
}

TEST(JsonCommonTest, JsonDupRawNumber) {
    /* Arrange */
    union json_t j = JSON_NUMBER("123");

    /* Act */
    union json_t res = json_dup(j);

    /* Assert */
    EXPECT_EQ(JT_NUMBER, j.type);
    EXPECT_STREQ(j.tok.text, res.tok.text);

    /* Clean up */
    free(res.tok.text);
}

TEST(JsonCommonTest, JsonDupInt) {
    /* Arrange */
    union json_t j = JSON_INT(123);

    /* Act */
    union json_t res = json_dup(j);

    /* Assert */
    EXPECT_EQ(JT_INT, j.type);
    EXPECT_EQ(j.tok.i64, res.tok.i64);
}

TEST(JsonCommonTest, JsonDupUint) {
    /* Arrange */
    union json_t j = JSON_UINT(123);

    /* Act */
    union json_t res = json_dup(j);

    /* Assert */
    EXPECT_EQ(JT_UINT, j.type);
    EXPECT_EQ(j.tok.u64, res.tok.u64);
}

TEST(JsonCommonTest, JsonDupBool) {
    /* Arrange */
    union json_t j = JSON_BOOL(true);

    /* Act */
    union json_t res = json_dup(j);

    /* Assert */
    EXPECT_EQ(JT_BOOL, j.type);
    EXPECT_EQ(j.tok.boolean, res.tok.boolean);
}

TEST(JsonCommonTest, JsonDupNull) {
    /* Arrange */
    union json_t j = JSON_NULL;

    /* Act */
    union json_t res = json_dup(j);

    /* Assert */
    EXPECT_EQ(JT_NULL, j.type);
    EXPECT_EQ(j.type, res.type);
}

TEST(JsonCommonTest, JsonDupFloat) {
    /* Arrange */
    union json_t j = JSON_FLOAT(1.0);

    /* Act */
    union json_t res = json_dup(j);

    /* Assert */
    EXPECT_EQ(JT_FLOAT, j.type);
    EXPECT_EQ(j.tok.f64, res.tok.f64);
}

TEST(JsonCommonTest, JsonTerminalTokenLength) {
    /* Arrange */
    union json_t j = JSON_INT(123);

    /* Act */
    size_t res = json_length(j);

    /* Assert */
    EXPECT_EQ(JT_INT, j.type);
    EXPECT_EQ(123, j.tok.i64);
    EXPECT_EQ(0, res);
}

TEST(JsonCommonTest, JsonTerminalTokenCapacity) {
    /* Arrange */
    union json_t j = JSON_INT(123);

    /* Act */
    size_t res = json_capacity(j);

    /* Assert */
    EXPECT_EQ(JT_INT, j.type);
    EXPECT_EQ(123, j.tok.i64);
    EXPECT_EQ(0, res);
}

TEST(JsonCommonTest, JsonGetTerminalToken) {
    /* Arrange */
    union json_t j = JSON_INT(123);

    /* Act */
    union json_t *res_1 = __json_getp_from_obj(j, "A");
    union json_t *res_2 = __json_getp_from_arr(j, 1);

    /* Assert */
    EXPECT_EQ(JT_INT, j.type);
    EXPECT_EQ(123, j.tok.i64);
    EXPECT_EQ(NULL, res_1);
    EXPECT_EQ(NULL, res_2);
}

TEST(JsonCommonTest, JsonSetTerminalToken) {
    /* Arrange */
    union json_t j = JSON_INT(123);

    /* Act */
    bool res = json_set(&j, "A", 1);

    /* Assert */
    EXPECT_FALSE(res);
    EXPECT_EQ(JT_INT, j.type);
    EXPECT_EQ(123, j.tok.i64);
}

TEST(JsonCommonTest, JsonRemoveTerminalToken) {
    /* Arrange */
    union json_t j = JSON_INT(123);

    /* Act */
    union json_t res_1 = __json_remove_from_obj(&j, "A");
    union json_t res_2 = __json_remove_from_arr(&j, 1);

    /* Assert */
    EXPECT_EQ(JT_INT, j.type);
    EXPECT_EQ(123, j.tok.i64);
    EXPECT_EQ(JT_MISSING, res_1.type);
    EXPECT_EQ(JT_MISSING, res_2.type);
}

TEST(JsonCommonTest, JsonDeleteTerminalToken) {
    /* Arrange */
    union json_t j = JSON_INT(123);

    /* Act */
    __json_delete_from_obj(&j, "A");
    __json_delete_from_arr(&j, 1);

    /* Assert */
    EXPECT_EQ(JT_INT, j.type);
    EXPECT_EQ(123, j.tok.i64);
}

TEST(JsonCommonTest, JsonAppendTerminalToken) {
    /* Arrange */
    union json_t j = JSON_INT(123);

    /* Act */
    bool res = json_append(&j, 456);

    /* Assert */
    EXPECT_FALSE(res);
    EXPECT_EQ(JT_INT, j.type);
    EXPECT_EQ(123, j.tok.i64);
}

TEST(JsonCommonTest, JsonCleanTerminalToken) {
    /* Arrange */
    union json_t j = JSON_INT(123);

    /* Act */
    json_clean(&j);

    /* Assert */
    EXPECT_EQ(JT_INT, j.type);
    EXPECT_EQ(123, j.tok.i64);
}

TEST(JsonCommonTest, CleanDupText) {
    /* Arrange */
    union json_t j = json_dup(JSON_STRING("test"));

    /* Act */
    json_clean(&j);

    /* Assert */
    EXPECT_EQ(JT_STRING, j.type);
    EXPECT_EQ(NULL, j.tok.text);
}

TEST(JsonCommonTest, CleanDupRawNumber) {
    /* Arrange */
    union json_t j = json_dup(JSON_NUMBER("123"));

    /* Act */
    json_clean(&j);

    /* Assert */
    EXPECT_EQ(JT_NUMBER, j.type);
    EXPECT_EQ(NULL, j.tok.text);
}

TEST(JsonCommonTest, DumpsTerminalToken) {
    /* Arrange */
    union json_t j = JSON_INT(123);

    /* Act */
    char *res = json_dumps(j);

    /* Assert */
    EXPECT_STREQ("123", res);

    /* Clean */
    free(res);
}

TEST(JsonCommonTest, JsonDumpsTerminalToken) {
    /* Arrange */
    union json_t j = JSON_INT(123);

    /* Act */
    char *res = json_dumps(j, .indent = -1);

    /* Assert */
    EXPECT_STREQ("123", res);

    /* Clean */
    free(res);
}

TEST(JsonCommonTest, DumpToFile) {
    /* Arrange */
    FILE *tempOutput = tmpfile();
    EXPECT_TRUE(tempOutput != nullptr);

    union json_t j = JSON_OBJECT;
    json_set(&j, "A", JSON_INT(1));
    json_set(&j, "B", JSON_STRING("2"));

    /* Act */
    json_dump(j, tempOutput, .indent = 2);

    /* Assert */
    fseek(tempOutput, 0, SEEK_SET);

    char buffer[128] = {};
    size_t readBytes = fread(buffer, 1, sizeof(buffer) - 1, tempOutput);
    buffer[readBytes] = '\0';
    
    EXPECT_STREQ("{\n  \"A\": 1, \n  \"B\": \"2\"\n}", buffer);

    /* Clean */
    json_clean(&j);
    fclose(tempOutput);
}
