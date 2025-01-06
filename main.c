
#include "jsonLexer.h"
#include "jsonParser.h"

int main(int argc, char *argv[]) {

    struct LexerContext lexer_ctx = {
        .tokenLength = 0,
        .tokenCapacity = 0,
        .tokenList = NULL,
        .currentChar = '\0',
        .offset = 0,
        .column = 1,
        .row = 0,
        .stream = NULL,
    };

    if (argc >= 2) {
        lexer_ctx.stream = fopen(argv[1], "r");
    } else {
        // redirect stdin to a tempfile, because fseek is not work on stdin.
        lexer_ctx.stream = tmpfile();

        int c;
        while ((c = fgetc(stdin)) != EOF)
            fputc(c, lexer_ctx.stream);

        fflush(lexer_ctx.stream);
        fseek(lexer_ctx.stream, 0, SEEK_SET);
    }

    jsonLexer(&lexer_ctx);

    printToken(&lexer_ctx);

    struct ParserContext parser_ctx = {
        .tokenIndex = 0,
        .tokenLength = lexer_ctx.tokenLength,
        .tokenList = lexer_ctx.tokenList,
    };

    jsonParser(&parser_ctx);

    printTree(&parser_ctx);

    fclose(lexer_ctx.stream);

    return 0;
}