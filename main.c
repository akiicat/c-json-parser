
#include "jsonLexer.h"
#include "jsonParser.h"
#include "jsonListener.h"
#include "token.h"

void enterJson(struct jsonWalkerContext *ctx, struct jsonToken *token) {
}

void exitJson(struct jsonWalkerContext *ctx, struct jsonToken *token) {
}

void enterObj(struct jsonWalkerContext *ctx, struct objToken *token) {
}

void exitObj(struct jsonWalkerContext *ctx, struct objToken *token) {
}

void enterArr(struct jsonWalkerContext *ctx, struct arrToken *token) {
}

void exitArr(struct jsonWalkerContext *ctx, struct arrToken *token) {
}

void enterValue(struct jsonWalkerContext *ctx, struct valueToken *token) {
}

void exitValue(struct jsonWalkerContext *ctx, struct valueToken *token) {
}

void enterPair(struct jsonWalkerContext *ctx, struct pairToken *token) {
}

void exitPair(struct jsonWalkerContext *ctx, struct pairToken *token) {
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

    FILE *stream = NULL;

    if (argc >= 2) {
        stream = fopen(argv[1], "r");
    } else {
        // redirect stdin to a tempfile, because fseek is not work on stdin.
        stream = tmpfile();

        int c;
        while ((c = fgetc(stdin)) != EOF)
            fputc(c, stream);

        fflush(stream);
        fseek(stream, 0, SEEK_SET);
    }

    struct LexerContext lexer_ctx = {
        .tokenLength = 0,
        .tokenCapacity = 0,
        .tokenList = NULL,
        .currentChar = '\0',
        .offset = 0,
        .column = 1,
        .row = 0,
        .stream = stream,
    };
    jsonLexer(&lexer_ctx);
    printToken(&lexer_ctx);

    struct ParserContext parser_ctx = {
        .tokenIndex = 0,
        .tokenLength = lexer_ctx.tokenLength,
        .tokenList = lexer_ctx.tokenList,
    };
    jsonParser(&parser_ctx);
    printTree(&parser_ctx);

    struct jsonWalkerContext walker_ctx =  {
        .entry = (struct BaseToken *)&parser_ctx.json,
        .listener = {
            // .enterJson = enterJson,
            // .exitJson = exitJson,
            .enterObj = enterObj,
            .exitObj = exitObj,
            .enterArr = enterArr,
            .exitArr = exitArr,
            .enterValue = enterValue,
            .exitValue = exitValue,
            .enterPair = enterPair,
            .exitPair = exitPair,
        },
    };
    jsonWalker(&walker_ctx);

    fclose(lexer_ctx.stream);

    return 0;
}