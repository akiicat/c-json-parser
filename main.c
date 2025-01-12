
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

void enterValue(struct WalkerContext *ctx, struct valueToken *token) {
}

void exitValue(struct WalkerContext *ctx, struct valueToken *token) {
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
        .container = initTokenContainer(),
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
        .container = lexer_ctx.container,
    };
    jsonParser(&parser_ctx);
    printTree(&parser_ctx);

    struct WalkerContext walker_ctx =  {
        .entry = (struct BaseToken *)&parser_ctx.container->root,
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
    freeTokenContainer(lexer_ctx.container);

    return 0;
}