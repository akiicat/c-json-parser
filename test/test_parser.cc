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
    EXPECT_EQ(JT_NUMBER, json_get(j, "A")->type);
    EXPECT_EQ(JT_STRING, json_get(j, "B")->type);
    EXPECT_STREQ("1", json_get(j, "A")->m_text);
    EXPECT_STREQ("2", json_get(j, "B")->m_text);

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
    EXPECT_EQ(JT_NUMBER, json_get(j, "A")->type);
    EXPECT_EQ(JT_STRING, json_get(j, "B")->type);
    EXPECT_STREQ("1", json_get(j, "A")->m_text);
    EXPECT_STREQ("2", json_get(j, "B")->m_text);

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
    EXPECT_EQ(JT_NUMBER, json_get(j, "A")->type);
    EXPECT_EQ(JT_STRING, json_get(j, "B")->type);
    EXPECT_STREQ("1", json_get(j, "A")->m_text);
    EXPECT_STREQ("2", json_get(j, "B")->m_text);

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
