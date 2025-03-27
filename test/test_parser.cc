#include <gtest/gtest.h>

#include "env.hh"
#include "json.h"

TEST(JsonParserTest, LoadJsonFilePointer) {
    /* Arrange */
    FILE *fp = tmpfile();
    ASSERT_TRUE(fp != nullptr);

    const char *data = "{ \"A\" : 1, \"B\" : \"2\" }";
    ASSERT_EQ(strlen(data), fwrite(data, sizeof(char), strlen(data), fp));

    rewind(fp);
    
    /* Act */
    union json_t j = json_load(fp);

    /* Assert */
    EXPECT_EQ(JT_OBJECT, j.type);
    EXPECT_EQ(2, json_length(j));
    EXPECT_EQ(JT_NUMBER, json_get(j, "A").type);
    EXPECT_EQ(JT_STRING, json_get(j, "B").type);
    EXPECT_STREQ("1", json_get(j, "A").text);
    EXPECT_STREQ("2", json_get(j, "B").text);

    /* Clean */
    json_clean(&j);
    fclose(fp);
}

TEST(JsonParserTest, LoadJsonString) {
    /* Arrange */
    const char *data = "{ \"A\" : 1, \"B\" : \"2\" }";

    /* Act */
    union json_t j = json_string(data);

    /* Assert */
    EXPECT_EQ(JT_OBJECT, j.type);
    EXPECT_EQ(2, json_length(j));
    EXPECT_EQ(JT_NUMBER, json_get(j, "A").type);
    EXPECT_EQ(JT_STRING, json_get(j, "B").type);
    EXPECT_STREQ("1", json_get(j, "A").text);
    EXPECT_STREQ("2", json_get(j, "B").text);

    /* Clean */
    json_clean(&j);
}

TEST(JsonParserTest, LoadJsonFile) {
    /* Arrange */
    FILE *fp;
    const char *filename = "/tmp/test.json";

    fp = fopen(filename, "w");
    ASSERT_TRUE(fp != nullptr);

    const char *data = "{ \"A\" : 1, \"B\" : \"2\" }";
    ASSERT_EQ(strlen(data), fwrite(data, sizeof(char), strlen(data), fp));
    fclose(fp);  // Close the file after writing

    /* Act */
    union json_t j = json_file(filename);

    /* Assert */
    EXPECT_EQ(JT_OBJECT, j.type);
    EXPECT_EQ(2, json_length(j));
    EXPECT_EQ(JT_NUMBER, json_get(j, "A").type);
    EXPECT_EQ(JT_STRING, json_get(j, "B").type);
    EXPECT_STREQ("1", json_get(j, "A").text);
    EXPECT_STREQ("2", json_get(j, "B").text);

    /* Clean */
    json_clean(&j);
    ASSERT_EQ(0, remove(filename));
}

TEST(JsonParserTest, FileNotExist) {
    /* Arrange */
    const char *filename = "/tmp/test.json";

    /* Act */
    union json_t j = json_file(filename);

    /* Assert */
    EXPECT_EQ(JT_MISSING, j.type);

    /* Clean */
    json_clean(&j);
}

TEST(JsonParserTest, FilePointerIsNull) {
    /* Arrange */
    FILE *fp = nullptr;

    /* Act */
    union json_t j = json_load(fp);

    /* Assert */
    EXPECT_EQ(JT_MISSING, j.type);

    /* Clean */
    json_clean(&j);
}

TEST(JsonParserTest, ParseNestedArray) {
    /* Arrange */
    const char *data = "[ true, false, null, [ \"1\", \"2\" ], [ ], { } ]";

    /* Act */
    union json_t j = json_string(data);

    /* Assert */
    EXPECT_EQ(JT_ARRAY, j.type);
    EXPECT_EQ(6, json_length(j));
    EXPECT_EQ(JT_BOOL, json_get(j, 0).type);
    EXPECT_EQ(JT_BOOL, json_get(j, 1).type);
    EXPECT_EQ(JT_NULL, json_get(j, 2).type);
    EXPECT_EQ(JT_ARRAY, json_get(j, 3).type);
    EXPECT_EQ(JT_ARRAY, json_get(j, 4).type);
    EXPECT_EQ(JT_OBJECT, json_get(j, 5).type);
    EXPECT_TRUE(json_get(j, 0).boolean);
    EXPECT_FALSE(json_get(j, 1).boolean);
    EXPECT_STREQ("1", json_get(json_get(j, 3), 0).text);
    EXPECT_STREQ("2", json_get(json_get(j, 3), 1).text);
    EXPECT_EQ(2, json_length(json_get(j, 3)));
    EXPECT_EQ(0, json_length(json_get(j, 4)));
    EXPECT_EQ(0, json_length(json_get(j, 5)));

    /* Clean */
    json_clean(&j);
}

TEST(JsonParserTest, ParseNestedObject) {
    /* Arrange */
    const char *data = "{ \"A\" : 1, \"B\" : { \"C\" : 2, \"D\" : \"3\" }, \"E\" : { }, \"F\" : [ ] }";

    /* Act */
    union json_t j = json_string(data);

    /* Assert */
    EXPECT_EQ(JT_OBJECT, j.type);
    EXPECT_EQ(4, json_length(j));
    EXPECT_EQ(JT_NUMBER, json_get(j, "A").type);
    EXPECT_EQ(JT_OBJECT, json_get(j, "B").type);
    EXPECT_EQ(JT_OBJECT, json_get(j, "E").type);
    EXPECT_EQ(JT_ARRAY, json_get(j, "F").type);
    EXPECT_STREQ("1", json_get(j, "A").text);
    EXPECT_STREQ("2", json_get(json_get(j, "B"), "C").text);
    EXPECT_STREQ("3", json_get(json_get(j, "B"), "D").text);
    EXPECT_EQ(2, json_length(json_get(j, "B")));
    EXPECT_EQ(0, json_length(json_get(j, "E")));
    EXPECT_EQ(0, json_length(json_get(j, "F")));

    /* Clean */
    json_clean(&j);
}

