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
    EXPECT_STREQ(j.text, res.text);

    /* Clean up */
    EXPECT_EQ(JT_STRING, j.type);
    free(res.text);
}

TEST(JsonCommonTest, JsonDupRawNumber) {
    /* Arrange */
    union json_t j = JSON_NUMBER("123");

    /* Act */
    union json_t res = json_dup(j);

    /* Assert */
    EXPECT_EQ(JT_NUMBER, j.type);
    EXPECT_STREQ(j.text, res.text);

    /* Clean up */
    free(res.text);
}

TEST(JsonCommonTest, JsonDupInt) {
    /* Arrange */
    union json_t j = JSON_INT(123);

    /* Act */
    union json_t res = json_dup(j);

    /* Assert */
    EXPECT_EQ(JT_INT, j.type);
    EXPECT_EQ(j.i64, res.i64);
}

TEST(JsonCommonTest, JsonDupUint) {
    /* Arrange */
    union json_t j = JSON_UINT(123);

    /* Act */
    union json_t res = json_dup(j);

    /* Assert */
    EXPECT_EQ(JT_UINT, j.type);
    EXPECT_EQ(j.u64, res.u64);
}

TEST(JsonCommonTest, JsonDupBool) {
    /* Arrange */
    union json_t j = JSON_BOOL(true);

    /* Act */
    union json_t res = json_dup(j);

    /* Assert */
    EXPECT_EQ(JT_BOOL, j.type);
    EXPECT_EQ(j.boolean, res.boolean);
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
    EXPECT_EQ(j.f, res.f);
}

TEST(JsonCommonTest, JsonTerminalTokenLength) {
    /* Arrange */
    union json_t j = JSON_INT(123);

    /* Act */
    size_t res = json_length(j);

    /* Assert */
    EXPECT_EQ(JT_INT, j.type);
    EXPECT_EQ(123, j.i64);
    EXPECT_EQ(0, res);
}

TEST(JsonCommonTest, JsonTerminalTokenCapacity) {
    /* Arrange */
    union json_t j = JSON_INT(123);

    /* Act */
    size_t res = json_capacity(j);

    /* Assert */
    EXPECT_EQ(JT_INT, j.type);
    EXPECT_EQ(123, j.i64);
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
    EXPECT_EQ(123, j.i64);
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
    EXPECT_EQ(123, j.i64);
}

TEST(JsonCommonTest, JsonRemoveTerminalToken) {
    /* Arrange */
    union json_t j = JSON_INT(123);

    /* Act */
    union json_t res_1 = __json_remove_from_obj(&j, "A");
    union json_t res_2 = __json_remove_from_arr(&j, 1);

    /* Assert */
    EXPECT_EQ(JT_INT, j.type);
    EXPECT_EQ(123, j.i64);
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
    EXPECT_EQ(123, j.i64);
}

TEST(JsonCommonTest, JsonAppendTerminalToken) {
    /* Arrange */
    union json_t j = JSON_INT(123);

    /* Act */
    bool res = json_append(&j, 456);

    /* Assert */
    EXPECT_FALSE(res);
    EXPECT_EQ(JT_INT, j.type);
    EXPECT_EQ(123, j.i64);
}

TEST(JsonCommonTest, JsonCleanTerminalToken) {
    /* Arrange */
    union json_t j = JSON_INT(123);

    /* Act */
    json_clean(&j);

    /* Assert */
    EXPECT_EQ(JT_INT, j.type);
    EXPECT_EQ(123, j.i64);
}

TEST(JsonCommonTest, CleanDupText) {
    /* Arrange */
    union json_t j = json_dup(JSON_STRING("test"));

    /* Act */
    json_clean(&j);

    /* Assert */
    EXPECT_EQ(JT_STRING, j.type);
    EXPECT_EQ(NULL, j.text);
}

TEST(JsonCommonTest, CleanDupRawNumber) {
    /* Arrange */
    union json_t j = json_dup(JSON_NUMBER("123"));

    /* Act */
    json_clean(&j);

    /* Assert */
    EXPECT_EQ(JT_NUMBER, j.type);
    EXPECT_EQ(NULL, j.text);
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

TEST(JsonCommonTest, JsonUpdate) {
    /* Arrange */
    union json_t j1 = JSON_OBJECT;
    json_set(&j1, "A", JSON_INT(1));
    json_set(&j1, "B", JSON_STRING("2"));

    union json_t j2 = JSON_OBJECT;
    json_set(&j2, "A", "3");
    json_set(&j2, "C", 4);

    /* Act */
    json_update(&j1, j2);

    /* Assert */
    EXPECT_EQ(JT_OBJECT, j1.type);
    EXPECT_EQ(2, json_length(j1));
    EXPECT_EQ(JT_STRING, json_get(j1, "A").type);
    EXPECT_STREQ("3", json_get(j1, "A").text);
    EXPECT_EQ(JT_INT, json_get(j1, "C").type);
    EXPECT_EQ(4, json_get(j1, "C").i64);
    EXPECT_EQ(NULL, json_getp(j1, "B"));

    /* Clean */
    json_clean(&j1);
    json_clean(&j2);
}

TEST(JsonCommonTest, JsonUpdateNull) {
    /* Arrange */
    union json_t j1 = JSON_OBJECT;
    json_set(&j1, "A", JSON_INT(1));
    json_set(&j1, "B", JSON_STRING("2"));

    /* Act */
    json_update_null(&j1, nullptr);

    /* Assert */
    EXPECT_EQ(JT_NULL, j1.type);

    /* Clean */
    json_clean(&j1);
}

TEST(JsonCommonTest, JsonUpdateBool) {
    /* Arrange */
    union json_t j1 = JSON_OBJECT;
    json_set(&j1, "A", JSON_INT(1));
    json_set(&j1, "B", JSON_STRING("2"));

    /* Act */
    json_update_bool(&j1, true);

    /* Assert */
    EXPECT_EQ(JT_BOOL, j1.type);
    EXPECT_TRUE(j1.boolean);

    /* Clean */
    json_clean(&j1);
}

TEST(JsonCommonTest, JsonUpdateI8) {
    /* Arrange */
    union json_t j1 = JSON_OBJECT;
    json_set(&j1, "A", JSON_INT(1));
    json_set(&j1, "B", JSON_STRING("2"));

    /* Act */
    json_update_i8(&j1, 2);

    /* Assert */
    EXPECT_EQ(JT_INT, j1.type);
    EXPECT_EQ(2, j1.i64);

    /* Clean */
    json_clean(&j1);
}

TEST(JsonCommonTest, JsonUpdateI16) {
    /* Arrange */
    union json_t j1 = JSON_OBJECT;
    json_set(&j1, "A", JSON_INT(1));
    json_set(&j1, "B", JSON_STRING("2"));

    /* Act */
    json_update_i16(&j1, 2);

    /* Assert */
    EXPECT_EQ(JT_INT, j1.type);
    EXPECT_EQ(2, j1.i64);

    /* Clean */
    json_clean(&j1);
}

TEST(JsonCommonTest, JsonUpdateI32) {
    /* Arrange */
    union json_t j1 = JSON_OBJECT;
    json_set(&j1, "A", JSON_INT(1));
    json_set(&j1, "B", JSON_STRING("2"));

    /* Act */
    json_update_i32(&j1, 2);

    /* Assert */
    EXPECT_EQ(JT_INT, j1.type);
    EXPECT_EQ(2, j1.i64);

    /* Clean */
    json_clean(&j1);
}

TEST(JsonCommonTest, JsonUpdateI64) {
    /* Arrange */
    union json_t j1 = JSON_OBJECT;
    json_set(&j1, "A", JSON_INT(1));
    json_set(&j1, "B", JSON_STRING("2"));

    /* Act */
    json_update_i64(&j1, 2);

    /* Assert */
    EXPECT_EQ(JT_INT, j1.type);
    EXPECT_EQ(2, j1.i64);

    /* Clean */
    json_clean(&j1);
}

TEST(JsonCommonTest, JsonUpdateU8) {
    /* Arrange */
    union json_t j1 = JSON_OBJECT;
    json_set(&j1, "A", JSON_INT(1));
    json_set(&j1, "B", JSON_STRING("2"));

    /* Act */
    json_update_u8(&j1, 2);

    /* Assert */
    EXPECT_EQ(JT_UINT, j1.type);
    EXPECT_EQ(2, j1.u64);

    /* Clean */
    json_clean(&j1);
}

TEST(JsonCommonTest, JsonUpdateU16) {
    /* Arrange */
    union json_t j1 = JSON_OBJECT;
    json_set(&j1, "A", JSON_INT(1));
    json_set(&j1, "B", JSON_STRING("2"));

    /* Act */
    json_update_u16(&j1, 2);

    /* Assert */
    EXPECT_EQ(JT_UINT, j1.type);
    EXPECT_EQ(2, j1.u64);

    /* Clean */
    json_clean(&j1);
}

TEST(JsonCommonTest, JsonUpdateU32) {
    /* Arrange */
    union json_t j1 = JSON_OBJECT;
    json_set(&j1, "A", JSON_INT(1));
    json_set(&j1, "B", JSON_STRING("2"));

    /* Act */
    json_update_u32(&j1, 2);

    /* Assert */
    EXPECT_EQ(JT_UINT, j1.type);
    EXPECT_EQ(2, j1.u64);

    /* Clean */
    json_clean(&j1);
}

TEST(JsonCommonTest, JsonUpdateU64) {
    /* Arrange */
    union json_t j1 = JSON_OBJECT;
    json_set(&j1, "A", JSON_INT(1));
    json_set(&j1, "B", JSON_STRING("2"));

    /* Act */
    json_update_u64(&j1, 2);

    /* Assert */
    EXPECT_EQ(JT_UINT, j1.type);
    EXPECT_EQ(2, j1.u64);

    /* Clean */
    json_clean(&j1);
}

TEST(JsonCommonTest, JsonUpdateF32) {
    /* Arrange */
    union json_t j1 = JSON_OBJECT;
    json_set(&j1, "A", JSON_INT(1));
    json_set(&j1, "B", JSON_STRING("2"));

    /* Act */
    json_update_f32(&j1, 3.14);

    /* Assert */
    EXPECT_EQ(JT_FLOAT, j1.type);
    EXPECT_FLOAT_EQ(3.14, j1.f);

    /* Clean */
    json_clean(&j1);
}

TEST(JsonCommonTest, JsonUpdateF64) {
    /* Arrange */
    union json_t j1 = JSON_OBJECT;
    json_set(&j1, "A", JSON_INT(1));
    json_set(&j1, "B", JSON_STRING("2"));

    /* Act */
    json_update_f64(&j1, 3.14);

    /* Assert */
    EXPECT_EQ(JT_FLOAT, j1.type);
    EXPECT_DOUBLE_EQ(3.14, j1.f);

    /* Clean */
    json_clean(&j1);
}

TEST(JsonCommonTest, JsonUpdateValue) {
    /* Arrange */
    union json_t j1 = JSON_OBJECT;
    json_set(&j1, "A", JSON_INT(1));
    json_set(&j1, "B", JSON_STRING("2"));

    union json_t j2 = JSON_OBJECT;
    json_set(&j2, "A", "3");
    json_set(&j2, "C", 4);

    /* Act */
    json_update_value(&j1, j2);

    /* Assert */
    EXPECT_EQ(JT_OBJECT, j1.type);
    EXPECT_EQ(2, json_length(j1));
    EXPECT_EQ(JT_STRING, json_get(j1, "A").type);
    EXPECT_STREQ("3", json_get(j1, "A").text);
    EXPECT_EQ(JT_INT, json_get(j1, "C").type);
    EXPECT_EQ(4, json_get(j1, "C").i64);
    EXPECT_EQ(NULL, json_getp(j1, "B"));

    /* Clean */
    json_clean(&j1);
    json_clean(&j2);
}

TEST(JsonCommonTest, JsonUpdateValueP) {
    /* Arrange */
    union json_t j1 = JSON_OBJECT;
    json_set(&j1, "A", JSON_INT(1));
    json_set(&j1, "B", JSON_STRING("2"));

    union json_t j2 = JSON_OBJECT;
    json_set(&j2, "A", "3");
    json_set(&j2, "C", 4);

    /* Act */
    json_update_value_p(&j1, &j2);

    /* Assert */
    EXPECT_EQ(JT_OBJECT, j1.type);
    EXPECT_EQ(2, json_length(j1));
    EXPECT_EQ(JT_STRING, json_get(j1, "A").type);
    EXPECT_STREQ("3", json_get(j1, "A").text);
    EXPECT_EQ(JT_INT, json_get(j1, "C").type);
    EXPECT_EQ(4, json_get(j1, "C").i64);
    EXPECT_EQ(NULL, json_getp(j1, "B"));
    EXPECT_EQ(JT_OBJECT, j2.type);
    EXPECT_EQ(0, json_length(j2));

    /* Clean */
    json_clean(&j1);
    json_clean(&j2);
}
