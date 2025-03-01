#include <stdio.h>
#include <stdlib.h>

#include "jsonEditor.h"
#include "jsonLexer.h"
#include "jsonParser.h"
#include "jsonListener.h"
#include "token.h"

void enterJson(struct WalkerContext *ctx, struct jsonToken *token) {
}

void exitJson(struct WalkerContext *ctx, struct jsonToken *token) {
}

void enterObj(struct WalkerContext *ctx, struct objToken *token) {
}

void exitObj(struct WalkerContext *ctx, struct objToken *token) {
}

void enterArr(struct WalkerContext *ctx, struct arrToken *token) {
}

void exitArr(struct WalkerContext *ctx, struct arrToken *token) {
}

void enterValue(struct WalkerContext *ctx, union valueToken *token) {
}

void exitValue(struct WalkerContext *ctx, union valueToken *token) {
}

void enterPair(struct WalkerContext *ctx, struct pairToken *token) {
}

void exitPair(struct WalkerContext *ctx, struct pairToken *token) {
    if (token->value.next.type != ARR && token->value.next.type != OBJ) {
        printf("%u:%u ", token->key.column, token->key.row);

        for (int i = 0; i < ctx->pathLength; i++) {
            if (ctx->path[i]->type == PAIR) {
                struct pairToken *pair = (struct pairToken *)ctx->path[i];
                printf(".%s", pair->key.text);
            }
        }

        printf(" = %s\n", token->value.anyToken.text);
    }
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

    struct objToken *obj = createObj();
    objInsert(obj, "Name", (union valueToken) { .stringToken = { .type = T_STRING, .text = "BBB" } });
    objInsert(obj, "Age", (union valueToken) { .stringToken = { .type = T_NUMBER, .text = "1.23" } });
    objInsert(obj, "Bool", (union valueToken) { .stringToken = { .type = T_TRUE } });

    struct objToken *innerObj = createObj();
    objInsert(innerObj, "innerAGE", (union valueToken) { .stringToken = { .type = T_NUMBER, .text = "1.23" } });
    objInsert(innerObj, "innerBool", (union valueToken) { .stringToken = { .type = T_TRUE } });

    objInsert(obj, "inner", (union valueToken)*innerObj);
    freeObj(innerObj);

    printJson((union valueToken)*obj);

    freeObj(obj);

    struct LexerContext *lexer_ctx = initJsonLexer(memstream);
    jsonLexer(lexer_ctx);
    printLexerToken(lexer_ctx);
    fclose(memstream);
    free(buffer);

    // struct ParserContext *parser_ctx = initJsonParser(lexer_ctx);
    // struct valueToken *value = jsonParser(parser_ctx);
    // printTree(parser_ctx);
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