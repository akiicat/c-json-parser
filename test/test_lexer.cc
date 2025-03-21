#include <cstddef>
#include <gtest/gtest.h>

#include "env.hh"
#include "json.h"

TEST(JsonLexerTest, LexerType2Str) {
    for (int i = JLT_MISSING; i <= JLT_LEXER_TOKEN_SIZE; i++) {
        EXPECT_STREQ(lexer_type_str[i], json_lexer_type2str((enum json_lexer_token_type_t)(i)));
    }
    EXPECT_EQ(NULL, json_lexer_type2str((enum json_lexer_token_type_t)(JLT_LEXER_TOKEN_SIZE+1)));
    EXPECT_EQ(NULL, json_lexer_type2str((enum json_lexer_token_type_t)(JLT_MISSING-1)));
}

TEST(JsonLexerTest, CreateLexer) {
    /* Arrange */
    const char *str = "test";

    /* Act */
    struct json_lexer_context_t *lexer = json_create_lexer(str);

    /* Assert */
    EXPECT_STREQ(str, lexer->from_string);
    EXPECT_EQ(strlen(str), lexer->from_string_len);
    EXPECT_EQ(0, lexer->offset);
    EXPECT_EQ(1, lexer->column);
    EXPECT_EQ(1, lexer->row);
    EXPECT_EQ(0, lexer->tokens.length);
    EXPECT_EQ(0, lexer->tokens.capacity);
    EXPECT_EQ(NULL, lexer->tokens.list);

    /* Clean */
    json_delete_lexer(lexer);
}

TEST(JsonLexerTest, ExecuteLexer) {
    /* Arrange */
    const char *json_str = "{\"key\":\"value\"}";
    struct json_lexer_context_t *lexer = json_create_lexer(json_str);

    /* Act */
    json_execute_lexer(lexer);

    /* Assert */
    struct json_lexer_token_t tokens[] = {
        { .type = JLT_LPAIR, .text = &json_str[0], .index = 0, .start = 0, .end = 1, .column = 1, .row = 1 },
        { .type = JLT_STRING, .text = &json_str[2], .index = 1, .start = 2, .end = 5, .column = 2, .row = 1 },
        { .type = JLT_COLON, .text = &json_str[6], .index = 2, .start = 6, .end = 7, .column = 7, .row = 1 },
        { .type = JLT_STRING, .text = &json_str[8], .index = 3, .start = 8, .end = 13, .column = 8, .row = 1 },
        { .type = JLT_RPAIR, .text = &json_str[14], .index = 4, .start = 14, .end = 15, .column = 15, .row = 1 },
    };
    int tokens_len = sizeof(tokens) / sizeof(struct json_lexer_token_t);

    EXPECT_LE(tokens_len, lexer->tokens.capacity);
    EXPECT_EQ(tokens_len, lexer->tokens.length);

    for (int i = 0; i < tokens_len; i++) {
        EXPECT_EQ(tokens[i].type, lexer->tokens.list[i].type) << "Case: " << i;
        EXPECT_EQ(tokens[i].text, lexer->tokens.list[i].text) << "Case: " << i; // only check its address
        EXPECT_EQ(tokens[i].index, lexer->tokens.list[i].index) << "Case: " << i;
        EXPECT_EQ(tokens[i].start, lexer->tokens.list[i].start) << "Case: " << i;
        EXPECT_EQ(tokens[i].end, lexer->tokens.list[i].end) << "Case: " << i;
        EXPECT_EQ(tokens[i].column, lexer->tokens.list[i].column) << "Case: " << i;
        EXPECT_EQ(tokens[i].row, lexer->tokens.list[i].row) << "Case: " << i;
    }

    /* Clean */
    json_delete_lexer(lexer);
}

TEST(JsonLexerTest, ExecuteLexerSingleToken) {
    struct testcase_t {
        const char *str;
        struct json_lexer_token_t token;
    };

    /* Arrange */
    struct testcase_t testcases[] = {
        { .str = "{", .token = { .type = JLT_LPAIR, .text = "{", .index = 0, .start = 0, .end = 1, .column = 1, .row = 1 } },
        { .str = "}", .token = { .type = JLT_RPAIR, .text = "}", .index = 0, .start = 0, .end = 1, .column = 1, .row = 1 } },
        { .str = "[", .token = { .type = JLT_LARRAY, .text = "[", .index = 0, .start = 0, .end = 1, .column = 1, .row = 1 } },
        { .str = "]", .token = { .type = JLT_RARRAY, .text = "]", .index = 0, .start = 0, .end = 1, .column = 1, .row = 1 } },
        { .str = ":", .token = { .type = JLT_COLON, .text = ":", .index = 0, .start = 0, .end = 1, .column = 1, .row = 1 } },
        { .str = ",", .token = { .type = JLT_COMMA, .text = ",", .index = 0, .start = 0, .end = 1, .column = 1, .row = 1 } },
        { .str = "true", .token = { .type = JLT_TRUE, .text = "true", .index = 0, .start = 0, .end = 4, .column = 1, .row = 1 } },
        { .str = "false", .token = { .type = JLT_FALSE, .text = "false", .index = 0, .start = 0, .end = 5, .column = 1, .row = 1 } },
        { .str = "null", .token = { .type = JLT_NULL, .text = "null", .index = 0, .start = 0, .end = 4, .column = 1, .row = 1 } },
        { .str = " \t\r\n 123", .token = { .type = JLT_NUMBER, .text = "123", .index = 0, .start = 5, .end = 8, .column = 2, .row = 2 } },
        { .str = "0", .token = { .type = JLT_NUMBER, .text = "0", .index = 0, .start = 0, .end = 1, .column = 1, .row = 1 } },
        { .str = "123", .token = { .type = JLT_NUMBER, .text = "123", .index = 0, .start = 0, .end = 3, .column = 1, .row = 1 } },
        { .str = "-123", .token = { .type = JLT_NUMBER, .text = "-123", .index = 0, .start = 0, .end = 4, .column = 1, .row = 1 } },
        { .str = "+123", .token = { .type = JLT_NUMBER, .text = "+123", .index = 0, .start = 0, .end = 4, .column = 1, .row = 1 } },
        { .str = ".123456", .token = { .type = JLT_NUMBER, .text = ".123456", .index = 0, .start = 0, .end = 7, .column = 1, .row = 1 } },
        { .str = "123.456", .token = { .type = JLT_NUMBER, .text = "123.456", .index = 0, .start = 0, .end = 7, .column = 1, .row = 1 } },
        { .str = "-123.456", .token = { .type = JLT_NUMBER, .text = "-123.456", .index = 0, .start = 0, .end = 8, .column = 1, .row = 1 } },
        { .str = "123e456", .token = { .type = JLT_NUMBER, .text = "123e456", .index = 0, .start = 0, .end = 7, .column = 1, .row = 1 } },
        { .str = "-123e456", .token = { .type = JLT_NUMBER, .text = "-123e456", .index = 0, .start = 0, .end = 8, .column = 1, .row = 1 } },
        { .str = "123E456", .token = { .type = JLT_NUMBER, .text = "123E456", .index = 0, .start = 0, .end = 7, .column = 1, .row = 1 } },
        { .str = "-123E456", .token = { .type = JLT_NUMBER, .text = "-123E456", .index = 0, .start = 0, .end = 8, .column = 1, .row = 1 } },
        { .str = "123e+456", .token = { .type = JLT_NUMBER, .text = "123e+456", .index = 0, .start = 0, .end = 8, .column = 1, .row = 1 } },
        { .str = "-123e+456", .token = { .type = JLT_NUMBER, .text = "-123e+456", .index = 0, .start = 0, .end = 9, .column = 1, .row = 1 } },
        { .str = "123e-456", .token = { .type = JLT_NUMBER, .text = "123e-456", .index = 0, .start = 0, .end = 8, .column = 1, .row = 1 } },
        { .str = "-123e-456", .token = { .type = JLT_NUMBER, .text = "-123e-456", .index = 0, .start = 0, .end = 9, .column = 1, .row = 1 } },
        { .str = "\"test\"", .token = { .type = JLT_STRING, .text = "test", .index = 0, .start = 1, .end = 5, .column = 1, .row = 1 } },
        { .str = "\"a\"", .token = { .type = JLT_STRING, .text = "a", .index = 0, .start = 1, .end = 2, .column = 1, .row = 1 } },
        // { .str = "\"\"", .token = { .type = JLT_STRING, .text = "", .index = 0, .start = 1, .end = 1, .column = 1, .row = 1 } },
        { .str = "\"te" "\\" "\n" "st\"", .token = { .type = JLT_STRING, .text = "te\\\nst", .index = 0, .start = 1, .end = 7, .column = 1, .row = 1 } },
    };

    int testcases_len = sizeof(testcases) / sizeof(struct testcase_t);

    for (int i = 0; i < testcases_len; i++) {
        struct json_lexer_context_t *lexer = json_create_lexer(testcases[i].str);
        
        /* Act */
        json_execute_lexer(lexer);
        json_print_lexer(lexer);

        /* Assert */
        EXPECT_EQ(1, lexer->tokens.length) << "Case: " << i;
        EXPECT_LE(1, lexer->tokens.capacity) << "Case: " << i;
        EXPECT_EQ(testcases[i].token.type, lexer->tokens.list[0].type) << "Case: " << i;
        EXPECT_EQ(testcases[i].token.index, lexer->tokens.list[0].index) << "Case: " << i;
        EXPECT_EQ(testcases[i].token.start, lexer->tokens.list[0].start) << "Case: " << i;
        EXPECT_EQ(testcases[i].token.end, lexer->tokens.list[0].end) << "Case: " << i;
        EXPECT_EQ(testcases[i].token.column, lexer->tokens.list[0].column) << "Case: " << i;
        EXPECT_EQ(testcases[i].token.row, lexer->tokens.list[0].row) << "Case: " << i;

        size_t expect_length = testcases[i].token.end - testcases[i].token.start;
        char *expect_prefix_str = json_strndup(&testcases[i].str[testcases[i].token.start], expect_length);

        size_t actual_length = lexer->tokens.list[0].end - lexer->tokens.list[0].start;
        char *actual_prefix_str = json_strndup(lexer->tokens.list[0].text, actual_length);
        
        EXPECT_STREQ(expect_prefix_str, actual_prefix_str) << "Case: " << i;

        /* Clean */
        json_delete_lexer(lexer);
        free(expect_prefix_str);
        free(actual_prefix_str);
    }
}

