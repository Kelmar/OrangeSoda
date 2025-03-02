/*************************************************************************/
/*************************************************************************/

#include "osbc.h"

#include "lex.h"
#include "parse.h"
#include "declarer.h"
#include "resolver.h"
#include "codegen.h"

/*************************************************************************/

static std::string g_filename = "";

/*
 * Other options to consider:
 * - Compile type: program/library
 * - Entry point: default 'main' or '_start'
 * - No default libraries
 * - Output filename
 */

/*************************************************************************/

void ParseArgs(int argc, char **argv)
{
    std::vector<std::string> args;
    args.assign(argv + 1, argv + argc);

    // Reverse the vector so we can treat it like a stack.
    std::reverse(args.begin(), args.end());

    while (!args.empty())
    {
        std::string arg = args.back();
        args.pop_back();

        g_filename = arg;
    }
}

/*************************************************************************/

ast::PModuleNode ParseFile()
{
    PLexer lexer(new Lexer(g_filename));
    Parser parser(lexer);

    return parser.Execute();
}

/*************************************************************************/
/**
 * @brief Run various processes over the AST.
 */
void Process(ast::PModuleNode root)
{
    std::vector<ast::PNodeVisitor> visitors =
    {
        std::make_shared<Declarer>(), // Fill out symbol table with declarations.
        std::make_shared<Resolver>(), // Validate all symbols can be resolved.
    };

    // After the Declarer and Resolver stages the program should be completely validated.

    // TODO: Add things like optimization passes, etc here.
    //visitors.push_back(std::make_shared<ConstFolding>());
    //visitors.push_back(std::make_shared<BooleanShortCircuit>());

    // Last stage, generate the actual code.
    visitors.push_back(std::make_shared<CodeGen>());

    for (auto visitor : visitors)
        root->Accept(*visitor.get());
}

/*************************************************************************/

int main(int argc, char **argv)
{
    try
    {
        ParseArgs(argc, argv);

        auto root = ParseFile();

        Process(root);
    }
    catch (const err::compile_error &err)
    {
        fmt::println(stderr, "ERROR ({0}): {1}", err.lineNumber(), err.what());
        return -1;
    }
    catch (const std::exception &ex)
    {
        fmt::println(stderr, "EXCEPTION: {0}", ex.what());
        return -1;
    }

    return 0;
}

/*************************************************************************/
