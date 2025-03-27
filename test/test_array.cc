#include <gtest/gtest.h>

#include "env.hh"
#include "json.h"
#include "json.hh"

TEST(JsonArrayTest, CreateArray) {
    /* Arrange */
    size_t capacity = 10;

    /* Act */
    union json_t j = json_create_arr(capacity);

    /* Assert */
    EXPECT_EQ(JT_ARRAY, j.type);
    EXPECT_EQ(0, json_length(j));
    EXPECT_LE(capacity, json_capacity(j));

    /* Clean */
    json_clean(&j);
}

TEST(JsonArrayTest, GetFromArray) {
    /* Arrange */
    union json_t j = JSON_ARRAY;
    json_append(&j, 1);
    json_append(&j, "2");

    /* Act */
    union json_t res = json_get(j, 1);

    /* Assert */
    EXPECT_EQ(JT_STRING, res.type);
    EXPECT_STREQ("2", res.tok.text);

    /* Clean */
    json_clean(&j);
}

TEST(JsonArrayTest, RemoveFromArray) {
    /* Arrange */
    union json_t j = JSON_ARRAY;
    json_append(&j, 1);
    json_append(&j, "2");

    /* Act */
    union json_t res = json_remove(&j, 1);
    union json_t res_not_exist = json_remove(&j, 100);

    /* Assert */
    EXPECT_EQ(JT_STRING, res.type);
    EXPECT_STREQ("2", res.tok.text);
    EXPECT_EQ(1, json_length(j));
    EXPECT_EQ(JT_MISSING, res_not_exist.type);

    /* Clean */
    json_clean(&j);
    json_clean(&res);
}

TEST(JsonArrayTest, DeleteFromArray) {
    /* Arrange */
    union json_t j = JSON_ARRAY;
    json_append(&j, 1);
    json_append(&j, "2");

    /* Act */
    json_delete(&j, 1);

    /* Assert */
    EXPECT_EQ(JT_ARRAY, j.type);
    EXPECT_EQ(1, json_length(j));
    EXPECT_EQ(JT_INT, json_get(j, 0).type);
    EXPECT_EQ(1, json_get(j, 0).i64);

    /* Clean */
    json_clean(&j);
}

TEST(JsonArrayTest, ArrayLength) {
    /* Arrange */
    union json_t j = JSON_ARRAY;
    json_append(&j, 1);
    json_append(&j, "2");

    /* Act */
    size_t res = json_length(j);

    /* Assert */
    EXPECT_EQ(2, res);

    /* Clean */
    json_clean(&j);
}

TEST(JsonArrayTest, ArrayCapacity) {
    /* Arrange */
    union json_t j = JSON_ARRAY;
    json_append(&j, 1);
    json_append(&j, "2");

    /* Act */
    size_t res = json_capacity(j);

    /* Assert */
    EXPECT_LE(2, res);

    /* Clean */
    json_clean(&j);
}

TEST(JsonArrayTest, JsonSet) {
    /* Arrange */
    union json_t j = JSON_ARRAY;
    json_append(&j, 1L);
    json_append(&j, "2");

    /* Act */
    json_set(&j, 0, JSON_STRING("100"));
    json_set(&j, 1L, 3L);

    /* Assert */
    EXPECT_EQ(JT_ARRAY, j.type);
    EXPECT_EQ(2, json_length(j));
    EXPECT_EQ(JT_STRING, json_get(j, 0).type);
    EXPECT_EQ(JT_INT, json_get(j, 1).type);
    EXPECT_STREQ("100", json_get(j, 0).text);
    EXPECT_EQ(3, json_get(j, 1).i64);

    /* Clean */
    json_clean(&j);
}

TEST(JsonArrayTest, SetString) {
    /* Arrange */
    union json_t j = JSON_ARRAY;
    json_append(&j, 1);
    json_append(&j, "2");

    /* Act */
    json_set_arr_str(&j, 0, "100");
    json_set_arr_str(&j, 1, "3");

    /* Assert */
    EXPECT_EQ(JT_ARRAY, j.type);
    EXPECT_EQ(2, json_length(j));
    EXPECT_EQ(JT_STRING, json_get(j, 0).type);
    EXPECT_EQ(JT_STRING, json_get(j, 1).type);
    EXPECT_STREQ("100", json_get(j, 0).text);
    EXPECT_STREQ("3", json_get(j, 1).text);

    /* Clean */
    json_clean(&j);
}

TEST(JsonArrayTest, SetBool) {
    /* Arrange */
    union json_t j = JSON_ARRAY;
    json_append(&j, 1);

    /* Act */
    json_set_arr_bool(&j, 0, true);

    /* Assert */
    EXPECT_EQ(JT_ARRAY, j.type);
    EXPECT_EQ(1, json_length(j));
    EXPECT_EQ(JT_BOOL, json_get(j, 0).type);
    EXPECT_TRUE(json_get(j, 0).boolean);

    /* Clean */
    json_clean(&j);
}

TEST(JsonArrayTest, SetNull) {
    /* Arrange */
    union json_t j = JSON_ARRAY;
    json_append(&j, 1);

    /* Act */
    json_set_arr_null(&j, 0, NULL);

    /* Assert */
    EXPECT_EQ(JT_ARRAY, j.type);
    EXPECT_EQ(1, json_length(j));
    EXPECT_EQ(JT_NULL, json_get(j, 0).type);

    /* Clean */
    json_clean(&j);
}

TEST(JsonArrayTest, SetI8) {
    /* Arrange */
    union json_t j = JSON_ARRAY;
    json_append(&j, 1);

    /* Act */
    json_set_arr_i8(&j, 0, 100);

    /* Assert */
    EXPECT_EQ(JT_ARRAY, j.type);
    EXPECT_EQ(1, json_length(j));
    EXPECT_EQ(JT_INT, json_get(j, 0).type);
    EXPECT_EQ(100, json_get(j, 0).i64);

    /* Clean */
    json_clean(&j);
}

TEST(JsonArrayTest, SetI16) {
    /* Arrange */
    union json_t j = JSON_ARRAY;
    json_append(&j, 1);

    /* Act */
    json_set_arr_i16(&j, 0, 100);

    /* Assert */
    EXPECT_EQ(JT_ARRAY, j.type);
    EXPECT_EQ(1, json_length(j));
    EXPECT_EQ(JT_INT, json_get(j, 0).type);
    EXPECT_EQ(100, json_get(j, 0).i64);

    /* Clean */
    json_clean(&j);
}

TEST(JsonArrayTest, SetI32) {
    /* Arrange */
    union json_t j = JSON_ARRAY;
    json_append(&j, 1);

    /* Act */
    json_set_arr_i32(&j, 0, 100);

    /* Assert */
    EXPECT_EQ(JT_ARRAY, j.type);
    EXPECT_EQ(1, json_length(j));
    EXPECT_EQ(JT_INT, json_get(j, 0).type);
    EXPECT_EQ(100, json_get(j, 0).i64);

    /* Clean */
    json_clean(&j);
}

TEST(JsonArrayTest, SetI64) {
    /* Arrange */
    union json_t j = JSON_ARRAY;
    json_append(&j, 1);

    /* Act */
    json_set_arr_i64(&j, 0, 100);

    /* Assert */
    EXPECT_EQ(JT_ARRAY, j.type);
    EXPECT_EQ(1, json_length(j));
    EXPECT_EQ(JT_INT, json_get(j, 0).type);
    EXPECT_EQ(100, json_get(j, 0).i64);

    /* Clean */
    json_clean(&j);
}

TEST(JsonArrayTest, SetU8) {
    /* Arrange */
    union json_t j = JSON_ARRAY;
    json_append(&j, 1);

    /* Act */
    json_set_arr_u8(&j, 0, 100);

    /* Assert */
    EXPECT_EQ(JT_ARRAY, j.type);
    EXPECT_EQ(1, json_length(j));
    EXPECT_EQ(JT_UINT, json_get(j, 0).type);
    EXPECT_EQ(100, json_get(j, 0).u64);

    /* Clean */
    json_clean(&j);
}

TEST(JsonArrayTest, SetU16) {
    /* Arrange */
    union json_t j = JSON_ARRAY;
    json_append(&j, 1);

    /* Act */
    json_set_arr_u16(&j, 0, 100);

    /* Assert */
    EXPECT_EQ(JT_ARRAY, j.type);
    EXPECT_EQ(1, json_length(j));
    EXPECT_EQ(JT_UINT, json_get(j, 0).type);
    EXPECT_EQ(100, json_get(j, 0).u64);

    /* Clean */
    json_clean(&j);
}

TEST(JsonArrayTest, SetU32) {
    /* Arrange */
    union json_t j = JSON_ARRAY;
    json_append(&j, 1);

    /* Act */
    json_set_arr_u32(&j, 0, 100);

    /* Assert */
    EXPECT_EQ(JT_ARRAY, j.type);
    EXPECT_EQ(1, json_length(j));
    EXPECT_EQ(JT_UINT, json_get(j, 0).type);
    EXPECT_EQ(100, json_get(j, 0).u64);

    /* Clean */
    json_clean(&j);
}

TEST(JsonArrayTest, SetU64) {
    /* Arrange */
    union json_t j = JSON_ARRAY;
    json_append(&j, 1);

    /* Act */
    json_set_arr_u64(&j, 0, 100);

    /* Assert */
    EXPECT_EQ(JT_ARRAY, j.type);
    EXPECT_EQ(1, json_length(j));
    EXPECT_EQ(JT_UINT, json_get(j, 0).type);
    EXPECT_EQ(100, json_get(j, 0).u64);

    /* Clean */
    json_clean(&j);
    json_clean(&j);
}

TEST(JsonArrayTest, SetF32) {
    /* Arrange */
    union json_t j = JSON_ARRAY;
    json_append(&j, 1);

    /* Act */
    json_set_arr_f32(&j, 0, 100.0);

    /* Assert */
    EXPECT_EQ(JT_ARRAY, j.type);
    EXPECT_EQ(1, json_length(j));
    EXPECT_EQ(JT_FLOAT, json_get(j, 0).type);
    EXPECT_EQ(100.0, json_get(j, 0).f);

    /* Clean */
    json_clean(&j);
}

TEST(JsonArrayTest, SetF64) {
    /* Arrange */
    union json_t j = JSON_ARRAY;
    json_append(&j, 1);

    /* Act */
    json_set_arr_f64(&j, 0, 100.0);

    /* Assert */
    EXPECT_EQ(JT_ARRAY, j.type);
    EXPECT_EQ(1, json_length(j));
    EXPECT_EQ(JT_FLOAT, json_get(j, 0).type);
    EXPECT_EQ(100.0, json_get(j, 0).f);

    /* Clean */
    json_clean(&j);
}

TEST(JsonArrayTest, SetValue) {
    /* Arrange */
    union json_t j = JSON_ARRAY;
    json_append(&j, 1);

    /* Act */
    json_set_arr_value(&j, 0, JSON_STRING("100"));

    /* Assert */
    EXPECT_EQ(JT_ARRAY, j.type);
    EXPECT_EQ(1, json_length(j));
    EXPECT_EQ(JT_STRING, json_get(j, 0).type);
    EXPECT_STREQ("100", json_get(j, 0).text);

    /* Clean */
    json_clean(&j);
}

TEST(JsonArrayTest, SetValueP) {
    /* Arrange */
    union json_t j = JSON_ARRAY;
    json_append(&j, 1);
    json_append(&j, 2);

    union json_t jobj = JSON_OBJECT;
    json_set(&jobj, "A", 1);

    union json_t jarr = JSON_ARRAY;
    json_append(&jarr, "2");

    /* Act */
    json_set_arr_value_p(&j, 0, &jobj);
    json_set_arr_value_p(&j, 1, &jarr);

    /* Assert */
    EXPECT_EQ(JT_ARRAY, j.type);
    EXPECT_EQ(2, json_length(j));
    EXPECT_EQ(JT_OBJECT, json_get(j, 0).type);
    EXPECT_EQ(JT_ARRAY, json_get(j, 1).type);
    EXPECT_EQ(1, json_get(json_get(j, 0), "A").i64);
    EXPECT_STREQ("2", json_get(json_get(j, 1), 0).text);
    EXPECT_EQ(JT_OBJECT, jobj.type);
    EXPECT_EQ(JT_ARRAY, jarr.type);
    EXPECT_EQ(0, json_length(jobj));
    EXPECT_EQ(0, json_length(jarr));

    /* Clean */
    json_clean(&j);
}

TEST(JsonArrayTest, AppendString) {
    /* Arrange */
    union json_t j = JSON_ARRAY;

    /* Act */
    json_append_str(&j, "1");
    json_append_str(&j, "2");

    /* Assert */
    EXPECT_EQ(JT_ARRAY, j.type);
    EXPECT_EQ(2, json_length(j));
    EXPECT_EQ(JT_STRING, json_get(j, 0).type);
    EXPECT_EQ(JT_STRING, json_get(j, 1).type);
    EXPECT_STREQ("1", json_get(j, 0).text);
    EXPECT_STREQ("2", json_get(j, 1).text);

    /* Clean */
    json_clean(&j);
}

TEST(JsonArrayTest, AppendBool) {
    /* Arrange */
    union json_t j = JSON_ARRAY;

    /* Act */
    json_append_bool(&j, true);
    json_append_bool(&j, false);

    /* Assert */
    EXPECT_EQ(JT_ARRAY, j.type);
    EXPECT_EQ(2, json_length(j));
    EXPECT_EQ(JT_BOOL, json_get(j, 0).type);
    EXPECT_EQ(JT_BOOL, json_get(j, 1).type);
    EXPECT_TRUE(json_get(j, 0).boolean);
    EXPECT_FALSE(json_get(j, 1).boolean);

    /* Clean */
    json_clean(&j);
}

TEST(JsonArrayTest, AppendNull) {
    /* Arrange */
    union json_t j = JSON_ARRAY;

    /* Act */
    json_append_null(&j, NULL);
    json_append_null(&j, NULL);

    /* Assert */
    EXPECT_EQ(JT_ARRAY, j.type);
    EXPECT_EQ(2, json_length(j));
    EXPECT_EQ(JT_NULL, json_get(j, 0).type);
    EXPECT_EQ(JT_NULL, json_get(j, 1).type);

    /* Clean */
    json_clean(&j);
}

TEST(JsonArrayTest, AppendI8) {
    /* Arrange */
    union json_t j = JSON_ARRAY;

    /* Act */
    json_append_i8(&j, 1);
    json_append_i8(&j, -2);

    /* Assert */
    EXPECT_EQ(JT_ARRAY, j.type);
    EXPECT_EQ(2, json_length(j));
    EXPECT_EQ(JT_INT, json_get(j, 0).type);
    EXPECT_EQ(JT_INT, json_get(j, 1).type);
    EXPECT_EQ(1, json_get(j, 0).i64);
    EXPECT_EQ(-2, json_get(j, 1).i64);

    /* Clean */
    json_clean(&j);
}

TEST(JsonArrayTest, AppendI16) {
    /* Arrange */
    union json_t j = JSON_ARRAY;

    /* Act */
    json_append_i16(&j, 1);
    json_append_i16(&j, -2);

    /* Assert */
    EXPECT_EQ(JT_ARRAY, j.type);
    EXPECT_EQ(2, json_length(j));
    EXPECT_EQ(JT_INT, json_get(j, 0).type);
    EXPECT_EQ(JT_INT, json_get(j, 1).type);
    EXPECT_EQ(1, json_get(j, 0).i64);
    EXPECT_EQ(-2, json_get(j, 1).i64);

    /* Clean */
    json_clean(&j);
}

TEST(JsonArrayTest, AppendI32) {
    /* Arrange */
    union json_t j = JSON_ARRAY;

    /* Act */
    json_append_i32(&j, 1);
    json_append_i32(&j, -2);

    /* Assert */
    EXPECT_EQ(JT_ARRAY, j.type);
    EXPECT_EQ(2, json_length(j));
    EXPECT_EQ(JT_INT, json_get(j, 0).type);
    EXPECT_EQ(JT_INT, json_get(j, 1).type);
    EXPECT_EQ(1, json_get(j, 0).i64);
    EXPECT_EQ(-2, json_get(j, 1).i64);

    /* Clean */
    json_clean(&j);
}

TEST(JsonArrayTest, AppendI64) {
    /* Arrange */
    union json_t j = JSON_ARRAY;

    /* Act */
    json_append_i64(&j, 1);
    json_append_i64(&j, -2);

    /* Assert */
    EXPECT_EQ(JT_ARRAY, j.type);
    EXPECT_EQ(2, json_length(j));
    EXPECT_EQ(JT_INT, json_get(j, 0).type);
    EXPECT_EQ(JT_INT, json_get(j, 1).type);
    EXPECT_EQ(1, json_get(j, 0).i64);
    EXPECT_EQ(-2, json_get(j, 1).i64);

    /* Clean */
    json_clean(&j);
}

TEST(JsonArrayTest, AppendU8) {
    /* Arrange */
    union json_t j = JSON_ARRAY;

    /* Act */
    json_append_u8(&j, 1);
    json_append_u8(&j, 2);

    /* Assert */
    EXPECT_EQ(JT_ARRAY, j.type);
    EXPECT_EQ(2, json_length(j));
    EXPECT_EQ(JT_UINT, json_get(j, 0).type);
    EXPECT_EQ(JT_UINT, json_get(j, 1).type);
    EXPECT_EQ(1, json_get(j, 0).u64);
    EXPECT_EQ(2, json_get(j, 1).u64);

    /* Clean */
    json_clean(&j);
}

TEST(JsonArrayTest, AppendU16) {
    /* Arrange */
    union json_t j = JSON_ARRAY;

    /* Act */
    json_append_u16(&j, 1);
    json_append_u16(&j, 2);

    /* Assert */
    EXPECT_EQ(JT_ARRAY, j.type);
    EXPECT_EQ(2, json_length(j));
    EXPECT_EQ(JT_UINT, json_get(j, 0).type);
    EXPECT_EQ(JT_UINT, json_get(j, 1).type);
    EXPECT_EQ(1, json_get(j, 0).u64);
    EXPECT_EQ(2, json_get(j, 1).u64);

    /* Clean */
    json_clean(&j);
}

TEST(JsonArrayTest, AppendU32) {
    /* Arrange */
    union json_t j = JSON_ARRAY;

    /* Act */
    json_append_u32(&j, 1);
    json_append_u32(&j, 2);

    /* Assert */
    EXPECT_EQ(JT_ARRAY, j.type);
    EXPECT_EQ(2, json_length(j));
    EXPECT_EQ(JT_UINT, json_get(j, 0).type);
    EXPECT_EQ(JT_UINT, json_get(j, 1).type);
    EXPECT_EQ(1, json_get(j, 0).u64);
    EXPECT_EQ(2, json_get(j, 1).u64);

    /* Clean */
    json_clean(&j);
}

TEST(JsonArrayTest, AppendU64) {
    /* Arrange */
    union json_t j = JSON_ARRAY;

    /* Act */
    json_append_u64(&j, 1);
    json_append_u64(&j, 2);

    /* Assert */
    EXPECT_EQ(JT_ARRAY, j.type);
    EXPECT_EQ(2, json_length(j));
    EXPECT_EQ(JT_UINT, json_get(j, 0).type);
    EXPECT_EQ(JT_UINT, json_get(j, 1).type);
    EXPECT_EQ(1, json_get(j, 0).u64);
    EXPECT_EQ(2, json_get(j, 1).u64);

    /* Clean */
    json_clean(&j);
}

TEST(JsonArrayTest, AppendF32) {
    /* Arrange */
    union json_t j = JSON_ARRAY;

    /* Act */
    json_append_f32(&j, 1.0);
    json_append_f32(&j, -2.0);

    /* Assert */
    EXPECT_EQ(JT_ARRAY, j.type);
    EXPECT_EQ(2, json_length(j));
    EXPECT_EQ(JT_FLOAT, json_get(j, 0).type);
    EXPECT_EQ(JT_FLOAT, json_get(j, 1).type);
    EXPECT_EQ(1.0, json_get(j, 0).f);
    EXPECT_EQ(-2.0, json_get(j, 1).f);

    /* Clean */
    json_clean(&j);
}

TEST(JsonArrayTest, AppendF64) {
    /* Arrange */
    union json_t j = JSON_ARRAY;

    /* Act */
    json_append_f64(&j, 1.0);
    json_append_f64(&j, -2.0);

    /* Assert */
    EXPECT_EQ(JT_ARRAY, j.type);
    EXPECT_EQ(2, json_length(j));
    EXPECT_EQ(JT_FLOAT, json_get(j, 0).type);
    EXPECT_EQ(JT_FLOAT, json_get(j, 1).type);
    EXPECT_EQ(1.0, json_get(j, 0).f);
    EXPECT_EQ(-2.0, json_get(j, 1).f);

    /* Clean */
    json_clean(&j);
}

TEST(JsonArrayTest, AppendValue) {
    /* Arrange */
    union json_t j = JSON_ARRAY;

    union json_t jobj = JSON_OBJECT;
    json_set(&jobj, "A", 1);

    union json_t jarr = JSON_ARRAY;
    json_append(&jarr, "2");

    /* Act */
    json_append_value(&j, JSON_INT(1));
    json_append_value(&j, JSON_STRING("2"));
    json_append_value(&j, jobj);
    json_append_value(&j, jarr);

    /* Assert */
    EXPECT_EQ(JT_ARRAY, j.type);
    EXPECT_EQ(4, json_length(j));
    EXPECT_EQ(JT_INT, json_get(j, 0).type);
    EXPECT_EQ(JT_STRING, json_get(j, 1).type);
    EXPECT_EQ(JT_OBJECT, json_get(j, 2).type);
    EXPECT_EQ(JT_ARRAY, json_get(j, 3).type);
    EXPECT_EQ(1, json_get(j, 0).i64);
    EXPECT_STREQ("2", json_get(j, 1).text);
    EXPECT_EQ(1, json_get(json_get(j, 2), "A").i64);
    EXPECT_STREQ("2", json_get(json_get(j, 3), 0).text);

    /* Clean */
    json_clean(&j);
    json_clean(&jobj);
    json_clean(&jarr);
}

TEST(JsonArrayTest, AppendValueP) {
    /* Arrange */
    union json_t j = JSON_ARRAY;
    union json_t jobj = JSON_OBJECT;
    union json_t jarr = JSON_ARRAY;
    json_set(&jobj, "A", 1);
    json_append(&jarr, "2");

    /* Act */
    json_append_value_p(&j, &jobj);
    json_append_value_p(&j, &jarr);

    /* Assert */
    EXPECT_EQ(JT_ARRAY, j.type);
    EXPECT_EQ(2, json_length(j));
    EXPECT_EQ(JT_OBJECT, json_get(j, 0).type);
    EXPECT_EQ(JT_ARRAY, json_get(j, 1).type);
    EXPECT_EQ(1, json_get(json_get(j, 0), "A").i64);
    EXPECT_STREQ("2", json_get(json_get(j, 1), 0).text);

    /* Clean */
    json_clean(&j);
}

TEST(JsonArrayTest, AppendTerminalToken) {
    /* Arrange */
    union json_t j = JSON_ARRAY;

    /* Act */
    json_append(&j, 1);
    json_append(&j, "2");
    json_append(&j, 3.0);
    json_append(&j, (void*)NULL);
    json_append(&j, JSON_NULL);
    json_append(&j, JSON_TRUE);
    json_append(&j, JSON_FALSE);
    json_append(&j, JSON_BOOL(true));
    json_append(&j, JSON_INT(-1));
    json_append(&j, JSON_UINT(1));
    json_append(&j, JSON_STRING("2"));
    json_append(&j, JSON_NUMBER("1.0"));

    /* Assert */
    EXPECT_EQ(JT_ARRAY, j.type);
    EXPECT_EQ(12, json_length(j));
    EXPECT_EQ(JT_INT, json_get(j, 0).type);
    EXPECT_EQ(JT_STRING, json_get(j, 1).type);
    EXPECT_EQ(JT_FLOAT, json_get(j, 2).type);
    EXPECT_EQ(JT_NULL, json_get(j, 3).type);
    EXPECT_EQ(JT_NULL, json_get(j, 4).type);
    EXPECT_EQ(JT_BOOL, json_get(j, 5).type);
    EXPECT_EQ(JT_BOOL, json_get(j, 6).type);
    EXPECT_EQ(JT_BOOL, json_get(j, 7).type);
    EXPECT_EQ(JT_INT, json_get(j, 8).type);
    EXPECT_EQ(JT_UINT, json_get(j, 9).type);
    EXPECT_EQ(JT_STRING, json_get(j, 10).type);
    EXPECT_EQ(JT_NUMBER, json_get(j, 11).type);
    EXPECT_EQ(1, json_get(j, 0).i64);
    EXPECT_STREQ("2", json_get(j, 1).text);
    EXPECT_EQ(3.0, json_get(j, 2).f);
    EXPECT_TRUE(json_get(j, 5).boolean);
    EXPECT_FALSE(json_get(j, 6).boolean);
    EXPECT_TRUE(json_get(j, 7).boolean);
    EXPECT_EQ(-1, json_get(j, 8).i64);
    EXPECT_EQ(1, json_get(j, 9).u64);
    EXPECT_STREQ("2", json_get(j, 10).text);
    EXPECT_STREQ("1.0", json_get(j, 11).text);

    /* Clean */
    json_clean(&j);
}

TEST(JsonArrayTest, AppendReturnValue) {
    /* Arrange */
    union json_t jarr = JSON_ARRAY;
    union json_t jobj = JSON_OBJECT;

    /* Act */
    bool res_1 = json_append(&jarr, 1);
    bool res_2 = json_append(&jarr, { .type = JT_MISSING });
    bool res_3 = json_append(&jobj, 3);

    /* Assert */
    EXPECT_TRUE(res_1);
    EXPECT_FALSE(res_2);
    EXPECT_FALSE(res_3);
    EXPECT_EQ(JT_ARRAY, jarr.type);
    EXPECT_EQ(1, json_length(jarr));
    EXPECT_EQ(JT_INT, json_get(jarr, 0).type);
    EXPECT_EQ(1, json_get(jarr, 0).i64);
    EXPECT_EQ(NULL, json_getp(jarr, 1));
    EXPECT_EQ(NULL, json_getp(jobj, 3));
    EXPECT_EQ(JT_MISSING, json_get(jarr, 1).type);
    EXPECT_EQ(JT_MISSING, json_get(jobj, 3).type);

    /* Clean */
    json_clean(&jarr);
    json_clean(&jobj);
}

TEST(JsonArrayTest, AppendNestedArray) {
    /* Arrange */
    union json_t j = JSON_ARRAY;
    union json_t jarr = JSON_ARRAY;
    json_append(&jarr, 1);
    json_append(&jarr, 2);

    /* Act */
    json_append(&j, jarr);
    json_append(&j, 3);

    /* Assert */
    EXPECT_EQ(JT_ARRAY, j.type);
    EXPECT_EQ(2, json_length(j));
    EXPECT_EQ(JT_ARRAY, json_get(j, 0).type);
    EXPECT_EQ(2, json_length(json_get(j, 0)));
    EXPECT_EQ(JT_INT, json_get(json_get(j, 0), 0).type);
    EXPECT_EQ(JT_INT, json_get(json_get(j, 0), 1).type);
    EXPECT_EQ(1, json_get(json_get(j, 0), 0).i64);
    EXPECT_EQ(2, json_get(json_get(j, 0), 1).i64);
    EXPECT_EQ(3, json_get(j, 1).i64);

    /* Clean */
    json_clean(&j);
    json_clean(&jarr);
}

TEST(JsonArrayTest, DeleteNestedArray) {
    /* Arrange */
    union json_t j = JSON_ARRAY;
    union json_t jarr = JSON_ARRAY;
    json_append(&jarr, 1);
    json_append(&jarr, 2);
    json_append(&j, jarr);
    json_append(&j, 3);

    /* Act */
    json_delete(&j, 0);

    /* Assert */
    EXPECT_EQ(JT_ARRAY, j.type);
    EXPECT_EQ(1, json_length(j));
    EXPECT_EQ(JT_INT, json_get(j, 0).type);
    EXPECT_EQ(3, json_get(j, 0).i64);

    /* Clean */
    json_clean(&j);
    json_clean(&jarr);
}

TEST(JsonArrayTest, AppendNestedObject) {
    /* Arrange */
    union json_t j = JSON_ARRAY;
    union json_t jobj = JSON_OBJECT;
    json_set(&jobj, "A", 1);
    json_set(&jobj, "B", 2);

    /* Act */
    json_append(&j, jobj);
    json_append(&j, 3);

    /* Assert */
    EXPECT_EQ(JT_ARRAY, j.type);
    EXPECT_EQ(2, json_length(j));
    EXPECT_EQ(JT_OBJECT, json_get(j, 0).type);
    EXPECT_EQ(2, json_length(json_get(j, 0)));
    EXPECT_EQ(JT_INT, json_get(json_get(j, 0), "A").type);
    EXPECT_EQ(JT_INT, json_get(json_get(j, 0), "B").type);
    EXPECT_EQ(1, json_get(json_get(j, 0), "A").i64);
    EXPECT_EQ(2, json_get(json_get(j, 0), "B").i64);
    EXPECT_EQ(3, json_get(j, 1).i64);

    /* Clean */
    json_clean(&j);
    json_clean(&jobj);
}

TEST(JsonArrayTest, DeleteNestedObject) {
    /* Arrange */
    union json_t j = JSON_ARRAY;
    union json_t jobj = JSON_OBJECT;
    json_set(&jobj, "A", 1);
    json_set(&jobj, "B", 2);
    json_append(&j, jobj);
    json_append(&j, 3);

    /* Act */
    json_delete(&j, 0);

    /* Assert */
    EXPECT_EQ(JT_ARRAY, j.type);
    EXPECT_EQ(1, json_length(j));
    EXPECT_EQ(JT_INT, json_get(j, 0).type);
    EXPECT_EQ(3, json_get(j, 0).i64);

    /* Clean */
    json_clean(&j);
    json_clean(&jobj);
}

TEST(JsonArrayTest, JsonDupArray) {
    /* Arrange */
    union json_t j = JSON_ARRAY;
    json_append(&j, 1);
    json_append(&j, "2");

    /* Act */
    union json_t res = json_dup(j);

    /* Assert */
    EXPECT_EQ(JT_ARRAY, j.type);
    EXPECT_EQ(2, json_length(j));
    EXPECT_EQ(JT_INT, json_get(j, 0).type);
    EXPECT_EQ(JT_STRING, json_get(j, 1).type);
    EXPECT_EQ(1, json_get(j, 0).i64);
    EXPECT_STREQ("2", json_get(j, 1).text);
    EXPECT_EQ(JT_ARRAY, res.type);
    EXPECT_EQ(2, json_length(res));
    EXPECT_EQ(JT_INT, json_get(res, 0).type);
    EXPECT_EQ(JT_STRING, json_get(res, 1).type);
    EXPECT_EQ(1, json_get(res, 0).i64);
    EXPECT_STREQ("2", json_get(res, 1).text);

    /* Clean */
    json_clean(&j);
    json_clean(&res);
}

TEST(JsonArrayTest, CleanArray) {
    /* Arrange */
    union json_t j = JSON_ARRAY;
    json_append(&j, 1);
    json_append(&j, "2");

    /* Act */
    json_clean(&j);

    /* Assert */
    EXPECT_EQ(JT_ARRAY, j.type);
    EXPECT_EQ(0, json_length(j));
}

TEST(JsonArrayTest, CleanDupArray) {
    /* Arrange */
    union json_t j = JSON_ARRAY;
    json_append(&j, 1);
    json_append(&j, "2");

    /* Act */
    union json_t res = json_dup(j);
    json_clean(&j);

    /* Assert */
    EXPECT_EQ(JT_ARRAY, res.type);
    EXPECT_EQ(2, json_length(res));
    EXPECT_EQ(JT_INT, json_get(res, 0).type);
    EXPECT_EQ(JT_STRING, json_get(res, 1).type);

    /* Clean */
    json_clean(&res);
}

TEST(JsonArrayTest, DumpsArray) {
    /* Arrange */
    union json_t j = JSON_ARRAY;
    json_append(&j, JSON_INT(1));
    json_append(&j, JSON_STRING("2"));
    json_append(&j, JSON_NUMBER("3.0"));

    /* Act */
    char *res = json_dumps(j, .indent = 2);

    /* Assert */
    EXPECT_STREQ("[\n  1, \n  \"2\", \n  3.0\n]", res);

    /* Clean */
    free(res);
    json_clean(&j);
}

TEST(JsonArrayTest, DumpsDupNestedArray) {
    /* Arrange */
    union json_t j = JSON_ARRAY;
    json_append(&j, JSON_INT(1));
    json_append(&j, JSON_ARRAY);
    json_append(json_getp(j, 1), JSON_STRING("2"));

    /* Act */
    union json_t res = json_dup(j);
    char *res_str = json_dumps(res, .indent = 2);

    /* Assert */
    EXPECT_STREQ("[\n  1, \n  [\n    \"2\"\n  ]\n]", res_str);

    /* Clean */
    free(res_str);
    json_clean(&j);
    json_clean(&res);
}

TEST(JsonArrayTest, DumpsDupNestedObject) {
    /* Arrange */
    union json_t j = JSON_ARRAY;
    json_append(&j, JSON_INT(1));
    json_append(&j, JSON_OBJECT);
    json_set(json_getp(j, 1), "A", JSON_STRING("2"));

    /* Act */
    union json_t res = json_dup(j);
    char *res_str = json_dumps(res, .indent = 2);

    /* Assert */
    EXPECT_STREQ("[\n  1, \n  {\n    \"A\": \"2\"\n  }\n]", res_str);

    /* Clean */
    free(res_str);
    json_clean(&j);
    json_clean(&res);
}

TEST(JsonArrayTest, JsonConcat) {
    /* Arrange */
    union json_t j1 = JSON_ARRAY;
    union json_t j2 = JSON_ARRAY;
    json_append(&j1, 1);
    json_append(&j2, 2);

    /* Act */
    json_concat(&j1, j2);

    /* Assert */
    EXPECT_EQ(JT_ARRAY, j1.type);
    EXPECT_EQ(2, json_length(j1));
    EXPECT_EQ(JT_INT, json_get(j1, 0).type);
    EXPECT_EQ(JT_INT, json_get(j1, 1).type);
    EXPECT_EQ(1, json_get(j1, 0).i64);
    EXPECT_EQ(2, json_get(j1, 1).i64);

    /* Clean */
    json_clean(&j1);
    json_clean(&j2);
}

TEST(JsonArrayTest, GetLastestElement) {
    /* Arrange */
    union json_t j = JSON_ARRAY;
    json_append(&j, 1);
    json_append(&j, "2");

    /* Act */
    union json_t res_1 = json_get(j, -1);
    union json_t res_2 = json_get(j, -2);
    union json_t res_3 = json_get(j, -100);

    /* Assert */
    EXPECT_EQ(JT_ARRAY, j.type);
    EXPECT_EQ(2, json_length(j));
    EXPECT_EQ(JT_STRING, res_1.type);
    EXPECT_STREQ("2", res_1.text);
    EXPECT_EQ(JT_INT, res_2.type);
    EXPECT_EQ(1, res_2.i64);
    EXPECT_EQ(JT_MISSING, res_3.type);

    /* Clean */
    json_clean(&j);
}

TEST(JsonArrayTest, RemoveLastestElement) {
    /* Arrange */
    union json_t j = JSON_ARRAY;
    json_append(&j, 1);
    json_append(&j, "2");

    /* Act */
    union json_t res = json_remove(&j, -1);
    union json_t res_not_exist = json_remove(&j, -100);

    /* Assert */
    EXPECT_EQ(JT_ARRAY, j.type);
    EXPECT_EQ(1, json_length(j));
    EXPECT_EQ(JT_STRING, res.type);
    EXPECT_STREQ("2", res.text);
    EXPECT_EQ(JT_MISSING, res_not_exist.type);

    /* Clean */
    json_clean(&j);
    json_clean(&res);
}

TEST(JsonArrayTest, DeleteLastestElement) {
    /* Arrange */
    union json_t j = JSON_ARRAY;
    json_append(&j, 1);
    json_append(&j, "2");

    /* Act */
    json_delete(&j, -1);

    /* Assert */
    EXPECT_EQ(JT_ARRAY, j.type);
    EXPECT_EQ(1, json_length(j));

    /* Clean */
    json_clean(&j);
}
