#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsonEditor.h"
#include "jsonLexer.h"
// #include "jsonParser.h"
// #include "jsonListener.h"
#include "jsonParser.h"
#include "token.h"

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

// void enterValue(struct WalkerContext *ctx, json_val_t *token) {
// }

// void exitValue(struct WalkerContext *ctx, json_val_t *token) {
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

    json_obj_t obj = JSON_OBJECT;

    json_obj_set(&obj, "Name", "BBB");
    json_obj_set(&obj, "Age", 123);
    json_obj_set(&obj, "Age", 123.123);
    json_obj_set(&obj, "Bool", true);
    json_obj_set(&obj, "Null", JSON_NULL);
    json_obj_set(&obj, "Array", JSON_ARRAY);
    json_obj_set(&obj, "Object", JSON_OBJECT);
    json_obj_set(&obj, "Bool", JSON_EMPTY);

    // json_obj_remove(&obj, (json_val_t)JSON_STR("BBB"));

    json_obj_t innerObj = JSON_OBJECT;
    json_obj_set(&innerObj, "inner2AGE", 789);
    json_obj_set(&innerObj, "inner2Bool", true);
    json_obj_set(&obj, "inner move", &innerObj);

    innerObj = JSON_OBJECT;
    json_obj_set(&innerObj, "inner1AGE", 456);
    json_obj_set(&innerObj, "inner1Bool", false);
    json_obj_set(&obj, "inner copy", innerObj);
    json_clean_up(&innerObj);

    json_obj_set((json_obj_t *)__json_obj_get(obj, "Object"), "Test1", "OK");
    json_obj_set(&__json_obj_get(obj, "Object")->obj, "Test2", "OK");

    // json_obj_t obj3 = __obj_get_copy(obj, "Object").obj;
    // json_obj_set(&obj3, "Test13", "OK");
    // json_obj_set(&obj3, "Test23", "OK");
    // freeValue((json_val_t*)&obj3);
    // printf("Name: %s\n", __obj_get(obj, "Name").stringToken.text);
    // printf("Name: %s\n", __obj_get(obj, "Name").stringToken.text);

    printJson((json_val_t)obj);
    json_clean_up(&obj);
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

    struct json_lexer_context_t *lexer_ctx = initJsonLexer(memstream);
    jsonLexer(lexer_ctx);
    // printLexerToken(lexer_ctx);
    
    fclose(memstream);
    free(buffer);
    
    // struct ParserContext *parser_ctx = initJsonParser(lexer_ctx);
    // struct valueToken *value = jsonParser(parser_ctx);

    // freeJsonParser(parser_ctx);
    freeJsonLexer(lexer_ctx);

    // // objInsert(&parser_ctx.container->root.value.obj, "AAA", (struct valueToken) { .stringToken = { .type = T_STRING, .text = "BBB" } });
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