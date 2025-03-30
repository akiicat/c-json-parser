#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// #include "jsonEditor.h"
// #include "jsonLexer.h"
// #include "jsonParser.h"
// #include "jsonListener.h"
// #include "jsonParser.h"
#include "json.h"

// void enterJson(struct WalkerContext *ctx, struct jsonToken *token) {
// }

// void exitJson(struct WalkerContext *ctx, struct jsonToken *token) {
// }

// void enterObj(struct WalkerContext *ctx, json_obj_t *token) {
// }

// void exitObj(struct WalkerContext *ctx, json_obj_t *token) {
// }

// void enterArr(struct WalkerContext *ctx, struct arrToken *token) {
// }

// void exitArr(struct WalkerContext *ctx, struct arrToken *token) {
// }

// void enterValue(struct WalkerContext *ctx, json_t *token) {
// }

// void exitValue(struct WalkerContext *ctx, json_t *token) {
// }

// void enterPair(struct WalkerContext *ctx, struct pairToken *token) {
// }

// void exitPair(struct WalkerContext *ctx, struct pairToken *token) {
//     if (token->value.next.type != ARR && token->value.next.type != OBJ) {
//         printf("%u:%u ", token->key.column, token->key.row);

//         for (int i = 0; i < ctx->pathLength; i++) {
//             if (ctx->path[i]->type == PAIR) {
//                 struct pairToken *pair = (struct pairToken *)ctx->path[i];
//                 printf(".%s", pair->key.text);
//             }
//         }

//         printf(" = %s\n", token->value.anyToken.text);
//     }
// }

void obj_test() {

    JSON_CURRENT_LOG_LEVEL = JSON_LOG_LEVEL_ERROR;

    union json_t obj = JSON_OBJECT;

    json_set(&obj, "Name", "BBB");
    json_set(&obj, "Age", 123);
    json_set(&obj, "Age", 123.123);
    json_set(&obj, "Bool", JSON_BOOL(true));
    json_set(&obj, "Null", JSON_NULL);
    json_set(&obj, "Object", JSON_OBJECT);

    json_set(&obj, "Delete1", "JSON_EMPTY");
    json_set(&obj, "Delete1", JSON_DELETE);
    json_set(&obj, "Delete2", "JSON_EMPTY");
    json_delete(&obj, "Delete2");

    json_set(&obj, "Array", JSON_ARRAY);
    json_append(json_getp(obj, "Array"), 123);
    json_append(json_getp(obj, "Array"), "INDEX = 1 DELETE LATER");
    json_append(json_getp(obj, "Array"), 123.456);
    json_append(json_getp(obj, "Array"), JSON_NUMBER("123.456"));
    json_append(json_getp(obj, "Array"), JSON_NULL);
    json_append(json_getp(obj, "Array"), JSON_TRUE);
    json_append(json_getp(obj, "Array"), JSON_FALSE);
    json_append(json_getp(obj, "Array"), JSON_BOOL(true));
    json_append(json_getp(obj, "Array"), JSON_BOOL(false));
    json_delete(json_getp(obj, "Array"), 1);
    json_delete(json_getp(obj, "Array"), json_length(json_get(obj, "Array")) - 1);

    union json_t arr_dup = json_dup(json_get(obj, "Array"));
    json_append(&arr_dup, "DUP");
    json_set(&obj, "Arr Dup", &arr_dup);

    union json_t innerObj = JSON_OBJECT;
    json_set(&innerObj, "inner2AGE", 789);
    json_set(&innerObj, "inner2Bool", true);
    json_set(&obj, "inner move", &innerObj);

    innerObj = JSON_OBJECT;
    json_set(&innerObj, "inner1AGE", 456);
    json_set(&innerObj, "inner1Bool", false);
    printf("%s:%d\n", __FUNCTION__, __LINE__);
    json_set(&obj, "inner copy", innerObj);
    printf("OK\n");
    json_clean(&innerObj);
    printf("OK\n");

    json_set(json_getp(obj, "Object"), "Test1", "OK1");
    json_set(json_getp(obj, "Object"), "Test1", "OK123");
    json_set(json_getp(obj, "Object Not Exist"), "Test1", "Fail");

    union json_t obj3 = json_dup(obj);
    json_set(&obj3, "Test1", "OK");
    json_clean(&obj3);
    json_set(&obj3, "Test2", "OK");

    printf("Name=%p type=%s value=%s\n",
        json_getp(obj3, "Test1"),
        (json_getp(obj3, "Test1") ? json_type2str(json_get(obj3, "Test1").type) : ""),
        (json_getp(obj3, "Test1") ? json_get(obj3, "Test1").tok.text : ""));
    printf("Name=%p type=%s value=%s\n", json_getp(obj3, "Test2"), 
        (json_getp(obj3, "Test2") ? json_type2str(json_get(obj3, "Test2").type) : ""),
        (json_getp(obj3, "Test2") ? json_get(obj3, "Test2").tok.text : ""));

    json_print(obj3);

    json_update(&obj3, "Test1XXXXXXXXXXXX");
    json_print(obj3);

    json_clean(&obj3);

    // json_print(obj);
    // json_pprint(obj, 4);
    // json_ppprint(obj, .indent = 8, .offset = 0);

    // char *s = json_dumps(obj, (struct json_config) { .indent = 4 });
    // printf("%s\n", s);
    // free(s);

    // FILE *f = fopen("test.json", "w");
    // json_dump(obj, f, (struct json_config) { .indent = 4 });
    // fclose(f);

    printf("-------------------\n");

    json_dump(obj, stderr, .indent = 4);

    json_clean(&obj);
}

int main(int argc, char *argv[]) {

    if (argc != 2) {
        fprintf(stderr, "Usage: %s filename\n", argv[0]);
        return EXIT_FAILURE;
    }

    /* Open the file in binary mode */
    FILE *fp = fopen(argv[1], "rb");
    if (!fp) {
        perror("fopen");
        return EXIT_FAILURE;
    }

    /* Seek to the end to determine the file size */
    if (fseek(fp, 0, SEEK_END) != 0) {
        perror("fseek");
        fclose(fp);
        return EXIT_FAILURE;
    }
    long filesize = ftell(fp);
    if (filesize < 0) {
        perror("ftell");
        fclose(fp);
        return EXIT_FAILURE;
    }
    rewind(fp);  // Reset file position to the beginning

    /* Allocate a buffer for the file contents (+1 for a null terminator) */
    char *buffer = malloc(filesize+1);
    if (!buffer) {
        perror("malloc");
        fclose(fp);
        return EXIT_FAILURE;
    }

    /* Read the file into the buffer */
    size_t bytesRead = fread(buffer, 1, filesize, fp);
    if (bytesRead != (size_t)filesize) {
        fprintf(stderr, "Error reading file\n");
        free(buffer);
        fclose(fp);
        return EXIT_FAILURE;
    }
    buffer[filesize] = '\0';  // Null terminate the buffer

    fclose(fp);

    /* Now, open a memory stream on the buffer */
    FILE *memstream = fmemopen(buffer, filesize, "r");
    if (!memstream) {
        perror("fmemopen");
        free(buffer);
        return EXIT_FAILURE;
    }

    // FILE *stream = NULL;

    // if (argc >= 2) {
    //     stream = fopen(argv[1], "r");
    // } else {
    //     // redirect stdin to a tempfile, because fseek is not work on stdin.
    //     stream = tmpfile();

    //     int c;
    //     while ((c = fgetc(stdin)) != EOF)
    //         fputc(c, stream);

    //     fflush(stream);
    //     fseek(stream, 0, SEEK_SET);
    // }

    obj_test();

    struct json_lexer_context_t *lexer_ctx = json_create_lexer(buffer);
    json_execute_lexer(lexer_ctx);
    json_print_lexer(lexer_ctx);
    json_delete_lexer(lexer_ctx);
    
    fclose(memstream);
    free(buffer);

        // struct ParserContext *parser_ctx = initJsonParser(lexer_ctx);
    // struct valueToken *value = jsonParser(parser_ctx);

    // freeJsonParser(parser_ctx);


    // objInsert(&parser_ctx.container->root.value.obj, "AAA", (struct valueToken) { .stringToken = { .type = T_STRING, .text = "BBB" } });
    // printToken(&lexer_ctx);
    // printTree(&parser_ctx);

    // struct WalkerContext walker_ctx =  {
    //     .entry = (struct BaseToken *)&parser_ctx.container->root,
    //     .listener = {
    //         // .enterJson = enterJson,
    //         // .exitJson = exitJson,
    //         .enterObj = enterObj,
    //         .exitObj = exitObj,
    //         .enterArr = enterArr,
    //         .exitArr = exitArr,
    //         .enterValue = enterValue,
    //         .exitValue = exitValue,
    //         .enterPair = enterPair,
    //         .exitPair = exitPair,
    //     },
    // };
    // jsonWalker(&walker_ctx);

    // freeWalker(&walker_ctx);
    // freeParser(&parser_ctx);

    return 0;
}