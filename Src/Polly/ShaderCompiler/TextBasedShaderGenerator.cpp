// Copyright (C) 2025 Cem Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE, or https://polly2d.org.

#include "Polly/ShaderCompiler/TextBasedShaderGenerator.hpp"

#include "Polly/Core/Casting.hpp"
#include "Polly/Defer.hpp"
#include "Polly/FileSystem.hpp"
#include "Polly/Format.hpp"
#include "Polly/ShaderCompiler/Ast.hpp"
#include "Polly/ShaderCompiler/CodeBlock.hpp"
#include "Polly/ShaderCompiler/CompileError.hpp"
#include "Polly/ShaderCompiler/Decl.hpp"
#include "Polly/ShaderCompiler/Expr.hpp"
#include "Polly/ShaderCompiler/SemaContext.hpp"
#include "Polly/ShaderCompiler/Stmt.hpp"
#include "Polly/ShaderCompiler/Type.hpp"
#include "Polly/ShaderCompiler/Writer.hpp"

namespace Polly::ShaderCompiler
{
TextBasedShaderGenerator::TextBasedShaderGenerator()
    : _uniformBufferAlignment(16)
    , _needsFloatLiteralSuffix(true)
{
}

TextBasedShaderGenerator::~TextBasedShaderGenerator() noexcept = default;

String TextBasedShaderGenerator::generate(
    const SemaContext&    context,
    const Ast&            ast,
    const FunctionDecl*   entryPoint,
    [[maybe_unused]] bool withDebugInfo)
{
    assume(not _ast);
    assume(ast.isVerified());

    _ast = std::addressof(ast);

    defer
    {
        _ast = nullptr;
    };

    const auto shaderName         = FileSystem::pathReplaceExtension(ast.filename().data(), {});
    const auto childrenToGenerate = gatherASTDeclarationsToGenerate(ast, entryPoint, context);

    if (childrenToGenerate.isEmpty())
    {
        throw ShaderCompileError::internal("Failed to gather children to generate.");
    }

    assume(entryPoint->isShader());

    _currentlyGeneratedShaderFunction = entryPoint;

    defer
    {
        _currentlyGeneratedShaderFunction = nullptr;
    };

    auto code = doGeneration(context, entryPoint, childrenToGenerate);
    code.trim({'\n'});

    if (not code.isEmpty())
    {
        code += '\n';
    }

    return code;
}

void TextBasedShaderGenerator::generateCodeBlock(
    Writer&            w,
    const CodeBlock*   codeBlock,
    const SemaContext& context)
{
    _tempVarNameGenStack.emplace(codeBlock);

    for (const auto& stmt : codeBlock->stmts())
    {
        generateStmt(w, stmt.get(), context);
        w << wnewline;
    }

    _tempVarNameGenStack.removeLast();
}

void TextBasedShaderGenerator::generateExpr(Writer& w, const Expr* expr, const SemaContext& context)
{
    if (const auto* parenExpr = as<ParenExpr>(expr))
    {
        w << '(';
        generateExpr(w, parenExpr->expr(), context);
        w << ')';
    }
    else if (const auto* intLit = as<IntLiteralExpr>(expr))
    {
        w << intLit->intValue();
    }
    else if (const auto* floatLit = as<FloatLiteralExpr>(expr))
    {
        w << floatLit->stringValue();
        if (_needsFloatLiteralSuffix)
            w << 'f';
    }
    else if (const auto* boolLit = as<BoolLiteralExpr>(expr))
    {
        w << boolLit->boolValue();
    }
    else if (const auto* scientificLit = as<ScientificIntLiteralExpr>(expr))
    {
        w << scientificLit->value();
    }
    else if (const auto* hexadecimalLit = as<HexadecimalIntLiteralExpr>(expr))
    {
        w << hexadecimalLit->value();
    }
    else if (const auto* symAccess = as<SymAccessExpr>(expr))
    {
        generateSymAccessExpr(w, symAccess, context);
    }
    else if (const auto* ternary = as<TernaryExpr>(expr))
    {
        generateTernaryExpr(w, ternary, context);
    }
    else if (const auto* unaryOp = as<UnaryOpExpr>(expr))
    {
        w << '-';
        generateExpr(w, unaryOp->expr(), context);
    }
    else if (const auto* functionCall = as<FunctionCallExpr>(expr))
    {
        generateFunctionCallExpr(w, functionCall, context);
    }
    else if (const auto* binOp = as<BinOpExpr>(expr))
    {
        generateBinOpExpr(w, binOp, context);
    }
    else if (const auto* sub = as<SubscriptExpr>(expr))
    {
        generateSubscriptExpr(w, sub, context);
    }
    else
    {
        w << "<< NotImplemented(" << typeid(*expr).name() << ") >> ";
    }
}

void TextBasedShaderGenerator::prepareExpr(Writer& w, const Expr* expr, const SemaContext& context)
{
    if (const auto* binOp = as<BinOpExpr>(expr))
    {
        prepareExpr(w, binOp->lhs(), context);
        prepareExpr(w, binOp->rhs(), context);
    }
    else if (const auto* unaryOp = as<UnaryOpExpr>(expr))
    {
        prepareExpr(w, unaryOp->expr(), context);
    }
    else if (const auto* ternaryOp = as<TernaryExpr>(expr))
    {
        prepareExpr(w, ternaryOp->conditionExpr(), context);
        prepareExpr(w, ternaryOp->trueExpr(), context);
        prepareExpr(w, ternaryOp->falseExpr(), context);
    }
    else if (const auto* subscript = as<SubscriptExpr>(expr))
    {
        prepareExpr(w, subscript->expr(), context);
        prepareExpr(w, subscript->indexExpr(), context);
    }
    else if (const auto* funcCall = as<FunctionCallExpr>(expr))
    {
        prepareExpr(w, funcCall->callee(), context);

        for (const auto& arg : funcCall->args())
        {
            prepareExpr(w, arg.get(), context);
        }
    }
}

void TextBasedShaderGenerator::generateStmt(Writer& w, const Statement* stmt, const SemaContext& context)
{
    if (const auto* varStmt = as<VarStmt>(stmt))
    {
        generateVarStmt(w, varStmt, context);
    }
    else if (const auto* ifStmt = as<IfStmt>(stmt))
    {
        generateIfStmt(w, ifStmt, context);
    }
    else if (const auto* returnStmt = as<ReturnStmt>(stmt))
    {
        generateReturnStmt(w, returnStmt, context);
    }
    else if (const auto* forStmt = as<ForStmt>(stmt))
    {
        generateForStmt(w, forStmt, context);
    }
    else if (const auto* compoundStmt = as<CompoundAssignment>(stmt))
    {
        generateCompoundStmt(w, compoundStmt, context);
    }
    else if (const auto* assignmentStmt = as<Assignment>(stmt))
    {
        generateAssignmentStmt(w, assignmentStmt, context);
    }
    else
    {
        w << "<< TODO(" << typeid(*stmt).name() << ") >>";
    }
}

void TextBasedShaderGenerator::generateIfStmt(Writer& w, const IfStmt* ifStmt, const SemaContext& context)
{
    {
        const auto* stmt = ifStmt;

        while (stmt)
        {
            if (stmt->conditionExpr())
            {
                prepareExpr(w, stmt->conditionExpr(), context);
            }

            stmt = stmt->next();
        }
    }

    const auto* stmt = ifStmt;

    while (stmt)
    {
        if (stmt != ifStmt)
        {
            w << " else ";
        }

        if (stmt->conditionExpr())
        {
            w << "if (";
            generateExpr(w, stmt->conditionExpr(), context);
            w << ") ";
        }

        w.openBrace();
        generateCodeBlock(w, stmt->body(), context);
        w.closeBrace();

        stmt = stmt->next();
    }
}

void TextBasedShaderGenerator::generateForStmt(Writer& w, const ForStmt* forStmt, const SemaContext& context)
{
    const auto  varName = forStmt->loopVariable()->name();
    const auto& range   = forStmt->range();
    const auto& type    = range.type();

    prepareExpr(w, range.start(), context);
    prepareExpr(w, range.end(), context);

    w << "for ( " << translateType(type, TypeNameContext::Normal) << ' ' << varName << " = ";
    generateExpr(w, range.start(), context);
    w << "; " << varName << " < ";
    generateExpr(w, range.end(), context);
    w << "; ++" << varName << ") ";

    w.openBrace();
    generateCodeBlock(w, forStmt->body(), context);
    w.closeBrace();
}

void TextBasedShaderGenerator::generateDecl(Writer& w, const Decl* decl, const SemaContext& context)
{
    if (const auto* function = as<FunctionDecl>(decl))
    {
        generateFunctionDecl(w, function, context);
    }
    else if (const auto* var = as<VarDecl>(decl))
    {
        generateGlobalVarDecl(w, var, context);
    }
    else
    {
        w << "<< NotImplemented(" << decl->name() << ") >>";
    }
}

void TextBasedShaderGenerator::generateVarStmt(
    Writer&                             w,
    [[maybe_unused]] const VarStmt*     varStmt,
    [[maybe_unused]] const SemaContext& context)
{
    w << "<< NotImplemented(" << typeid(*varStmt).name() << ") >>";
}

void TextBasedShaderGenerator::generateBinOpExpr(
    Writer&            w,
    const BinOpExpr*   binOp,
    const SemaContext& context)
{
    const auto* lhsExpr = binOp->lhs();
    const auto* rhsExpr = binOp->rhs();

    if (_isSwappingMatrixVectorMults)
    {
        const auto& lhs_type = lhsExpr->type();
        const auto& rhs_type = rhsExpr->type();

        if ((lhs_type->isMatrixType() and rhs_type->isMatrixType())
            or (lhs_type->isMatrixType() and rhs_type->isVectorType())
            or (lhs_type->isVectorType() and rhs_type->isMatrixType()))
        {
            std::swap(lhsExpr, rhsExpr);
        }
    }

    generateExpr(w, lhsExpr, context);

    const auto needSpaceBetweenOperands = binOp->binOpKind() != BinOpKind::MemberAccess;

    if (needSpaceBetweenOperands)
    {
        w << ' ';
    }

    switch (binOp->binOpKind())
    {
        case BinOpKind::Add: w << "+"; break;
        case BinOpKind::Subtract: w << "-"; break;
        case BinOpKind::Multiply: w << "*"; break;
        case BinOpKind::Divide: w << "/"; break;
        case BinOpKind::LogicalAnd: w << "&&"; break;
        case BinOpKind::LogicalOr: w << "||"; break;
        case BinOpKind::LessThan: w << "<"; break;
        case BinOpKind::LessThanOrEqual: w << "<="; break;
        case BinOpKind::GreaterThan: w << ">"; break;
        case BinOpKind::GreaterThanOrEqual: w << ">="; break;
        case BinOpKind::MemberAccess: w << "."; break;
        case BinOpKind::BitwiseXor: w << "^"; break;
        case BinOpKind::BitwiseAnd: w << "&"; break;
        case BinOpKind::Equal: w << "=="; break;
        case BinOpKind::NotEqual: w << "!="; break;
        case BinOpKind::RightShift: w << ">>"; break;
        case BinOpKind::BitwiseOr: w << "|"; break;
        case BinOpKind::LeftShift: w << "<<"; break;
        default: throw ShaderCompileError(binOp->location(), "Unhandled binary operation kind.");
    }

    if (needSpaceBetweenOperands)
    {
        w << ' ';
    }

    generateExpr(w, rhsExpr, context);
}

void TextBasedShaderGenerator::generateSubscriptExpr(
    Writer&              w,
    const SubscriptExpr* subscriptExpr,
    const SemaContext&   context)
{
    generateExpr(w, subscriptExpr->expr(), context);
    w << '[';
    generateExpr(w, subscriptExpr->indexExpr(), context);
    w << ']';
}

void TextBasedShaderGenerator::generateCompoundStmt(
    Writer&                   w,
    const CompoundAssignment* stmt,
    const SemaContext&        context)
{
    prepareExpr(w, stmt->lhs(), context);
    prepareExpr(w, stmt->rhs(), context);

    generateExpr(w, stmt->lhs(), context);

    switch (stmt->kind())
    {
        case CompoundAssignmentKind::Add: w << " += "; break;
        case CompoundAssignmentKind::Subtract: w << " -= "; break;
        case CompoundAssignmentKind::Multiply: w << " *= "; break;
        case CompoundAssignmentKind::Divide: w << " /= "; break;
    }

    generateExpr(w, stmt->rhs(), context);
    w << ';';
}

void TextBasedShaderGenerator::generateAssignmentStmt(
    Writer&            w,
    const Assignment*  stmt,
    const SemaContext& context)
{
    prepareExpr(w, stmt->lhs(), context);
    prepareExpr(w, stmt->rhs(), context);

    generateExpr(w, stmt->lhs(), context);
    w << " = ";
    generateExpr(w, stmt->rhs(), context);
    w << ';';
}

void TextBasedShaderGenerator::generateSymAccessExpr(
    Writer&                             w,
    const SymAccessExpr*                expr,
    [[maybe_unused]] const SemaContext& context)
{
    const auto& built_ins = context.builtInSymbols();
    const auto* symbol    = expr->symbol();
    const auto  name      = expr->name();

    if (as<FunctionParamDecl>(symbol))
    {
        w << name;
    }
    else if (_currentSymAccessOverride)
    {
        w << *_currentSymAccessOverride;
    }
    else if (built_ins.isSomeIntrinsicFunction(symbol))
    {
        w << name.lowerCased();
    }
    else
    {
        for (const auto& [built_in_type, built_in_type_name] : _builtInTypeDict)
        {
            if (built_in_type->typeName() == name)
            {
                w << built_in_type_name;
                return;
            }
        }

        w << name;
    }
}

void TextBasedShaderGenerator::generateTernaryExpr(
    Writer&            w,
    const TernaryExpr* expr,
    const SemaContext& context)
{
    w << "(";
    generateExpr(w, expr->conditionExpr(), context);
    w << " ? ";
    generateExpr(w, expr->trueExpr(), context);
    w << " : ";
    generateExpr(w, expr->falseExpr(), context);
    w << ")";
}

String TextBasedShaderGenerator::translateType(const Type* type, [[maybe_unused]] TypeNameContext context)
    const
{
    if (const auto it = _builtInTypeDict.find(type))
    {
        return *it;
    }

    return String(type->typeName());
}

String TextBasedShaderGenerator::translateArrayType(const ArrayType* type, StringView variableName) const
{
    return formatString(
        "{} {}[{}]",
        translateType(type->elementType(), TypeNameContext::Normal),
        variableName,
        type->size());
}

} // namespace Polly::ShaderCompiler
