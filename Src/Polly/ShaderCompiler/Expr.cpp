// Copyright (C) 2025 Cemalettin Dervis
// This file is part of Polly.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "Expr.hpp"

#include "../Core/Casting.hpp"
#include "BinOpTable.hpp"
#include "CodeBlock.hpp"
#include "CompileError.hpp"
#include "Decl.hpp"
#include "Polly/Algorithm.hpp"
#include "Polly/Format.hpp"
#include "Polly/LinalgOps.hpp"
#include "Polly/Maybe.hpp"
#include "Scope.hpp"
#include "SemaContext.hpp"
#include "Stmt.hpp"
#include "Type.hpp"
#include "TypeCache.hpp"

namespace Polly::ShaderCompiler
{
static StringView binOpKindDisplayString(BinOpKind kind)
{
    switch (kind)
    {
        case BinOpKind::Add: return "+";
        case BinOpKind::Subtract: return "-";
        case BinOpKind::Multiply: return "*";
        case BinOpKind::Divide: return "/";
        case BinOpKind::LogicalAnd: return "&&";
        case BinOpKind::LogicalOr: return "||";
        case BinOpKind::LessThan: return "<";
        case BinOpKind::LessThanOrEqual: return "<=";
        case BinOpKind::GreaterThan: return ">";
        case BinOpKind::GreaterThanOrEqual: return ">=";
        case BinOpKind::MemberAccess: return ".";
        case BinOpKind::BitwiseXor: return "^";
        case BinOpKind::BitwiseAnd: return "&";
        case BinOpKind::Equal: return "==";
        case BinOpKind::NotEqual: return "!=";
        case BinOpKind::RightShift: return ">>";
        case BinOpKind::BitwiseOr: return "|";
        case BinOpKind::LeftShift: return "<<";
    }

    throw ShaderCompileError::internal("invalid BinOpKind");
}

Expr::~Expr() noexcept = default;

void Expr::verify(SemaContext& context, Scope& scope)
{
    if (not _isVerified)
    {
        onVerify(context, scope);
        assumeWithMsg(_type, "Expression did not have a valid type after it was verified.");
        _isVerified = true;
    }
}

Any Expr::evaluateConstantValue([[maybe_unused]] SemaContext& context, [[maybe_unused]] Scope& scope) const
{
    return none;
}

bool Expr::isLiteral() const
{
    return false;
}

bool Expr::accessesSymbol(const Decl* symbol, [[maybe_unused]] bool transitive) const
{
    return _symbol == symbol;
}

const SourceLocation& Expr::location() const
{
    return _location;
}

const Type* Expr::type() const
{
    return _type;
}

const Decl* Expr::symbol() const
{
    return _symbol;
}

Expr::Expr(const SourceLocation& location)
    : _location(location)
    , _isVerified(false)
{
}

bool Expr::isVerified() const
{
    return _isVerified;
}

void Expr::setSymbol(const Decl* symbol)
{
    _symbol = symbol;
}

void Expr::setType(const Type* type)
{
    _type = type;
}

void IntLiteralExpr::onVerify([[maybe_unused]] SemaContext& context, [[maybe_unused]] Scope& scope)
{
}

IntLiteralExpr::IntLiteralExpr(const SourceLocation& location, int value)
    : Expr(location)
    , _value(value)
{
    setType(IntType::instance());
}

int IntLiteralExpr::intValue() const
{
    return _value;
}

Any IntLiteralExpr::evaluateConstantValue(
    [[maybe_unused]] SemaContext& context,
    [[maybe_unused]] Scope&       scope) const
{
    return _value;
}

bool IntLiteralExpr::isLiteral() const
{
    return true;
}

void BoolLiteralExpr::onVerify([[maybe_unused]] SemaContext& context, [[maybe_unused]] Scope& scope)
{
}

BoolLiteralExpr::BoolLiteralExpr(const SourceLocation& location, bool value)
    : Expr(location)
    , _value(value)
{
    setType(BoolType::instance());
}

bool BoolLiteralExpr::boolValue() const
{
    return _value;
}

Any BoolLiteralExpr::evaluateConstantValue(
    [[maybe_unused]] SemaContext& context,
    [[maybe_unused]] Scope&       scope) const
{
    return _value;
}

bool BoolLiteralExpr::isLiteral() const
{
    return true;
}

void FloatLiteralExpr::onVerify([[maybe_unused]] SemaContext& context, [[maybe_unused]] Scope& scope)
{
}

StringView FloatLiteralExpr::stringValue() const
{
    return _stringValue;
}

FloatLiteralExpr::FloatLiteralExpr(const SourceLocation& location, StringView stringValue, double value)
    : Expr(location)
    , _stringValue(stringValue)
    , _value(value)
{
    setType(FloatType::instance());
}

double FloatLiteralExpr::doubleValue() const
{
    return _value;
}

Any FloatLiteralExpr::evaluateConstantValue(
    [[maybe_unused]] SemaContext& context,
    [[maybe_unused]] Scope&       scope) const
{
    return _value;
}

bool FloatLiteralExpr::isLiteral() const
{
    return true;
}

void BinOpExpr::onVerify(SemaContext& context, Scope& scope)
{
    _lhs->verify(context, scope);

    if (is(BinOpKind::MemberAccess))
    {
        if (auto* symAccess = as<SymAccessExpr>(_rhs.get()))
        {
            symAccess->_ancestorExpr = _lhs.get();
        }
    }

    _rhs->verify(context, scope);

    const auto& lhsType = _lhs->type();
    const auto& rhsType = _rhs->type();
    const auto* symbol  = _rhs->symbol();

    if (is(BinOpKind::MemberAccess))
    {
        // The member we have accessed dictates our type.
        setType(rhsType);
        setSymbol(symbol);

        // If we're accessing an array's size, save its size as our evaluated constant value directly.
        if (context.builtInSymbols().isArraySizeMember(symbol))
        {
            _arraySizeAccessValue = as<ArrayType>(lhsType)->size();
        }
    }
    else
    {
        // The binary operation dictates our type.
        const auto maybeResultType =
            context.binaryOperationTable().binOpResultType(_binOpKind, lhsType, rhsType);

        if (not maybeResultType)
        {
            throw ShaderCompileError(
                location(),
                formatString(
                    "The operator '{}' is not defined between the types '{}' and '{}'.",
                    binOpKindDisplayString(_binOpKind),
                    lhsType->typeName(),
                    rhsType->typeName()));
        }

        setType(*maybeResultType);
    }
}

Any BinOpExpr::evaluateConstantValue(SemaContext& context, Scope& scope) const
{
    if (_arraySizeAccessValue)
    {
        return *_arraySizeAccessValue;
    }

    const auto lhs = _lhs->evaluateConstantValue(context, scope);
    const auto rhs = _rhs->evaluateConstantValue(context, scope);

    if (lhs and rhs)
    {
        if (lhs.type() != rhs.type())
        {
            return {};
        }

        if (lhs.type() == AnyType::Int)
        {
            const auto lhsInt = lhs.get<int>();
            const auto rhsInt = rhs.get<int>();

            switch (_binOpKind)
            {
                case BinOpKind::Add: return lhsInt + rhsInt;
                case BinOpKind::Subtract: return lhsInt - rhsInt;
                case BinOpKind::Multiply: return lhsInt * rhsInt;
                case BinOpKind::Divide: return lhsInt / rhsInt;
                case BinOpKind::LessThan: return lhsInt < rhsInt;
                case BinOpKind::LessThanOrEqual: return lhsInt <= rhsInt;
                case BinOpKind::GreaterThan: return lhsInt > rhsInt;
                case BinOpKind::GreaterThanOrEqual: return lhsInt >= rhsInt;
                case BinOpKind::BitwiseXor: return lhsInt xor rhsInt;
                case BinOpKind::BitwiseAnd: return lhsInt bitand rhsInt;
                case BinOpKind::Equal: return lhsInt == rhsInt;
                case BinOpKind::NotEqual: return lhsInt != rhsInt;
                case BinOpKind::RightShift: return lhsInt >> rhsInt;
                case BinOpKind::BitwiseOr: return lhsInt bitor rhsInt;
                case BinOpKind::LeftShift: return lhsInt << rhsInt;
                default: return {};
            }
        }

        if (lhs.type() == AnyType::Double)
        {
            const auto lhsFloat = lhs.get<double>();
            const auto rhsFloat = rhs.get<double>();

            switch (_binOpKind)
            {
                case BinOpKind::Add: return lhsFloat + rhsFloat;
                case BinOpKind::Subtract: return lhsFloat - rhsFloat;
                case BinOpKind::Multiply: return lhsFloat * rhsFloat;
                case BinOpKind::Divide: return lhsFloat / rhsFloat;
                case BinOpKind::LessThan: return lhsFloat < rhsFloat;
                case BinOpKind::LessThanOrEqual: return lhsFloat <= rhsFloat;
                case BinOpKind::GreaterThan: return lhsFloat > rhsFloat;
                case BinOpKind::GreaterThanOrEqual: return lhsFloat >= rhsFloat;
                case BinOpKind::Equal: return lhsFloat == rhsFloat;
                case BinOpKind::NotEqual: return lhsFloat != rhsFloat;
                default: return {};
            }
        }

        if (lhs.type() == AnyType::Vec2)
        {
            const auto a = lhs.get<Vec2>();
            const auto b = rhs.get<Vec2>();

            switch (_binOpKind)
            {
                case BinOpKind::Add: return a + b;
                case BinOpKind::Subtract: return a - b;
                case BinOpKind::Multiply: return a * b;
                case BinOpKind::Divide: return a / b;
                case BinOpKind::Equal: return a == b;
                case BinOpKind::NotEqual: return a != b;
                default: return {};
            }
        }

        if (lhs.type() == AnyType::Vec3)
        {
            const auto a = lhs.get<Vec3>();
            const auto b = rhs.get<Vec3>();

            switch (_binOpKind)
            {
                case BinOpKind::Add: return a + b;
                case BinOpKind::Subtract: return a - b;
                case BinOpKind::Multiply: return a * b;
                case BinOpKind::Divide: return a / b;
                case BinOpKind::Equal: return a == b;
                case BinOpKind::NotEqual: return a != b;
                default: return {};
            }
        }

        if (lhs.type() == AnyType::Vec4)
        {
            const auto a = lhs.get<Vec4>();
            const auto b = rhs.get<Vec4>();

            switch (_binOpKind)
            {
                case BinOpKind::Add: return a + b;
                case BinOpKind::Subtract: return a - b;
                case BinOpKind::Multiply: return a * b;
                case BinOpKind::Divide: return a / b;
                case BinOpKind::Equal: return a == b;
                case BinOpKind::NotEqual: return a != b;
                default: return {};
            }
        }
    }

    return {};
}

bool BinOpExpr::accessesSymbol(const Decl* symbol, bool transitive) const
{
    return _lhs->accessesSymbol(symbol, transitive) or _rhs->accessesSymbol(symbol, transitive);
}

BinOpExpr::BinOpExpr(const SourceLocation& location, BinOpKind kind, UniquePtr<Expr> lhs, UniquePtr<Expr> rhs)
    : Expr(location)
    , _binOpKind(kind)
    , _lhs(std::move(lhs))
    , _rhs(std::move(rhs))
{
}

BinOpKind BinOpExpr::binOpKind() const
{
    return _binOpKind;
}

const Expr* BinOpExpr::lhs() const
{
    return _lhs.get();
}

const Expr* BinOpExpr::rhs() const
{
    return _rhs.get();
}

bool BinOpExpr::is(BinOpKind kind) const
{
    return _binOpKind == kind;
}

void SubscriptExpr::onVerify(SemaContext& context, Scope& scope)
{
    _expr->verify(context, scope);

    setSymbol(_expr->symbol());
    assume(symbol());

    _indexExpr->verify(context, scope);

    const auto* indexType = _indexExpr->type();

    if (indexType != IntType::instance())
    {
        throw ShaderCompileError(
            _indexExpr->location(),
            formatString(
                "'{}' cannot be used to index into an array; expected '{}'.",
                indexType->typeName(),
                IntType::instance()->typeName()));
    }

    const auto* arrayType = as<ArrayType>(symbol()->type());

    if (not arrayType)
    {
        throw ShaderCompileError(
            _indexExpr->location(),
            formatString("Cannot index into non-array type '{}'.", symbol()->type()->typeName()));
    }

    const auto  arraySize   = arrayType->size();
    const auto* indexSymbol = _indexExpr->symbol();

    const auto indexExprConstantValue = _indexExpr->evaluateConstantValue(context, scope);
    auto       constantIndex          = Maybe<int>();

    if (indexExprConstantValue and indexExprConstantValue.type() == AnyType::Int)
    {
        const auto intIndex = indexExprConstantValue.get<int>();

        if (intIndex < 0)
        {
            throw ShaderCompileError(
                location(),
                formatString(
                    "You're attempting to access an array with size {} at index {}, which would be "
                    "out "
                    "of bounds.",
                    arraySize,
                    intIndex));
        }

        constantIndex = intIndex;
    }

    if (constantIndex and *constantIndex >= arraySize)
    {
        throw ShaderCompileError(
            location(),
            formatString("index (= {}) exceeds the array's size (= {})", *constantIndex, arraySize));
    }

    if (indexSymbol)
    {
        if (const auto* forLoopVar = as<ForLoopVariableDecl>(indexSymbol))
        {
            const auto& range    = forLoopVar->parentForStmt()->range();
            const auto  minValue = range.start()->evaluateConstantValue(context, scope);
            const auto  maxValue = range.end()->evaluateConstantValue(context, scope);

            if (minValue and minValue.get<int>() < 0)
            {
                throw ShaderCompileError(
                    location(),
                    formatString(
                        "The loop variable '{}' would access the array with size {} at index {}, which "
                        "would be out of bounds.",
                        forLoopVar->name(),
                        arraySize,
                        minValue.get<int>()));
            }

            if (maxValue and maxValue.get<int>() > arraySize)
            {
                throw ShaderCompileError(
                    location(),
                    formatString(
                        "The loop variable '{}' would access the array with size {} at index {}, which "
                        "would be out of bounds.",
                        forLoopVar->name(),
                        arraySize,
                        maxValue.get<int>() - 1));
            }
        }
    }

    setType(arrayType->elementType());
}

const Expr* SubscriptExpr::expr() const
{
    return _expr.get();
}

const Expr* SubscriptExpr::indexExpr() const
{
    return _indexExpr.get();
}

bool SubscriptExpr::accessesSymbol(const Decl* symbol, bool transitive) const
{
    return _expr->accessesSymbol(symbol, transitive) or _indexExpr->accessesSymbol(symbol, transitive);
}

SubscriptExpr::SubscriptExpr(const SourceLocation& location, UniquePtr<Expr> expr, UniquePtr<Expr> indexExpr)
    : Expr(location)
    , _expr(std::move(expr))
    , _indexExpr(std::move(indexExpr))
{
}

SymAccessExpr::SymAccessExpr(const SourceLocation& location, const Decl* symbol)
    : Expr(location)
    , _ancestorExpr()
{
    _identifier = symbol->name();
    setSymbol(symbol);
    setType(symbol->type());
}

static const Type* determineVectorSwizzlingType(StringView name)
{
    switch (name.size())
    {
        case 1: return FloatType::instance();
        case 2: return Vec2Type::instance();
        case 3: return Vec3Type::instance();
        case 4: return Vec4Type::instance();
        default: break;
    }

    return nullptr;
}

void SymAccessExpr::onVerify(SemaContext& context, Scope& scope)
{
    const auto& builtins           = context.builtInSymbols();
    const Type* overrideSymbolType = nullptr;

    if (_ancestorExpr)
    {
        // This is a member access. Search the symbol within the type (i.e. a member).
        const auto& ancestorType = _ancestorExpr->type();
        const auto* memberSymbol = ancestorType->findMemberSymbol(context, _identifier);

        if (memberSymbol == builtins.vectorSwizzlingSym.get())
        {
            _isVectorSwizzling = true;
            overrideSymbolType = determineVectorSwizzlingType(_identifier);

            if (not overrideSymbolType)
            {
                throw ShaderCompileError(
                    location(),
                    formatString("invalid vector swizzling '{}' (too many components)", _identifier));
            }
        }
        else if (memberSymbol == builtins.arraySizeMember.get())
        {
            overrideSymbolType = IntType::instance();
            _isArraySizeAccess = true;
        }

        if (not memberSymbol)
        {
            throw ShaderCompileError(
                location(),
                formatString("type '{}' has no member named '{}'", ancestorType->typeName(), _identifier));
        }

        setSymbol(memberSymbol);
    }
    else if (scope.currentContext() == ScopeContext::FunctionCall)
    {
        // We're looking up a symbol that represents a function call.
        // Because we support overloading, we have to look for the correct function
        // depending on the currently passed argument types.
        const auto& args              = scope.functionCallArguments();
        auto        wasFuncFoundAtAll = false;
        auto        allFuncsThatMatch = List<const FunctionDecl*>{};

        for (const auto& symbol : scope.findSymbols(_identifier, true))
        {
            const auto* function = as<FunctionDecl>(symbol);

            if (not function)
            {
                continue;
            }

            const auto acceptsImplicitlyCastArgs = builtins.acceptsImplicitlyCastArguments(function);

            wasFuncFoundAtAll = true;

            const auto params = function->parameters();

            if (params.size() != args.size())
            {
                continue;
            }

            const auto doParamTypesMatch =
                all(args,
                    [&](const auto& arg, u32 idx)
                    { return SemaContext::canAssign(params[idx]->type(), arg, acceptsImplicitlyCastArgs); });

            if (doParamTypesMatch)
            {
                // We've got a match.
                allFuncsThatMatch.add(function);
            }
        }

        const auto buildCallString = [&]
        {
            return formatString(
                "{}({})",
                _identifier,
                joinToStringBy(args, ", ", [](const Expr* arg) { return arg->type()->typeName(); }));
        };

        if (allFuncsThatMatch.isEmpty())
        {
            if (wasFuncFoundAtAll)
            {
                throw ShaderCompileError(
                    location(),
                    formatString(
                        "Unable to find a matching overload for function call '{}'",
                        buildCallString()));
            }

            throw ShaderCompileError(
                location(),
                formatString("Unable to find a function named '{}'.", _identifier));
        }

        if (allFuncsThatMatch.size() > 1)
        {
            throw ShaderCompileError(
                location(),
                formatString(
                    "This function call for '{}' is ambiguous (I found more than one suitable candidate).",
                    buildCallString()));
        }

        assume(allFuncsThatMatch.size() == 1);
        setSymbol(allFuncsThatMatch.first());
    }
    else
    {
        setSymbol(scope.findSymbol(_identifier));
    }

    if (not symbol())
    {
        // See if there's a similarly named symbol. If so, suggest it in the error message.
        if (const auto* similarSymbol = scope.findSymbolWithSimilarName(_identifier);
            similarSymbol and _identifier.size() > 2)
        {
            throw ShaderCompileError(
                location(),
                formatString(
                    "Unable to find a symbol named '{}' not found; did you mean '{}'?",
                    _identifier,
                    similarSymbol->name()));
        }

        throw ShaderCompileError(
            location(),
            formatString("Unable to find a symbol named '{}'.", _identifier));
    }

    if (overrideSymbolType)
    {
        setType(overrideSymbolType);
    }
    else
    {
        setType(symbol()->type());
    }
}

Any SymAccessExpr::evaluateConstantValue(SemaContext& context, Scope& scope) const
{
    if (const auto* variable = as<VarDecl>(symbol()))
    {
        return variable->expr()->evaluateConstantValue(context, scope);
    }

    return none;
}

bool SymAccessExpr::accessesSymbol(const Decl* symbol, bool /*transitive*/) const
{
    return this->symbol() == symbol;
}

Expr* SymAccessExpr::ancestorExpr() const
{
    return _ancestorExpr;
}

bool SymAccessExpr::isVectorSwizzling() const
{
    return _isVectorSwizzling;
}

bool SymAccessExpr::isArraySizeAccess() const
{
    return _isArraySizeAccess;
}

SymAccessExpr::SymAccessExpr(const SourceLocation& location, StringView identifier)
    : Expr(location)
    , _identifier(identifier)
    , _ancestorExpr(nullptr)
{
}

StringView SymAccessExpr::name() const
{
    return symbol() != nullptr ? symbol()->name() : _identifier;
}

StringView SymAccessExpr::identifier() const
{
    return _identifier;
}

void FunctionCallExpr::onVerify(SemaContext& context, Scope& scope)
{
    auto args = List<const Expr*>();
    args.reserve(_args.size());

    for (auto& arg : _args)
    {
        arg->verify(context, scope);
        args.emplace(arg.get());
    }

    scope.pushContext(ScopeContext::FunctionCall);
    scope.setFunctionCallArguments(std::move(args));

    _callee->verify(context, scope);
    setSymbol(_callee->symbol());

    scope.setFunctionCallArguments({});
    scope.popContext();

    setType(symbol()->type());

    if (const auto* calledFunction = as<FunctionDecl>(_callee->symbol()))
    {
        if (calledFunction->isShader())
        {
            throw ShaderCompileError(location(), "Calling a shader main function is not allowed.");
        }
    }
}

Span<UniquePtr<Expr>> FunctionCallExpr::args() const
{
    return _args;
}

bool FunctionCallExpr::accessesSymbol(const Decl* symbol, bool transitive) const
{
    if (_callee->accessesSymbol(symbol, transitive))
    {
        return true;
    }

    if (transitive)
    {
        if (const auto* symAccess = as<SymAccessExpr>(_callee.get()); symAccess and symAccess->isVerified())
        {
            if (const auto* func = as<FunctionDecl>(symAccess->symbol()))
            {
                if (func->body() and func->body()->accessesSymbol(symbol, transitive))
                {
                    return true;
                }
            }
        }
    }

    return containsWhere(
        _args,
        [&symbol, transitive](const auto& expr) { return expr->accessesSymbol(symbol, transitive); });
}

Any FunctionCallExpr::evaluateConstantValue(SemaContext& context, Scope& scope) const
{
    assume(isVerified());

    const auto& builtins = context.builtInSymbols();
    const auto* symbol   = this->callee()->symbol();

    const auto getArgConstantValues = [this, &context, &scope]
    {
        auto argValues = List<Any, 4>();
        argValues.reserve(_args.size());

        for (const auto& arg : _args)
        {
            auto value = arg->evaluateConstantValue(context, scope);
            if (not value)
            {
                argValues.clear();
                break;
            }

            argValues.add(std::move(value));
        }

        return argValues;
    };

    const auto expectAndGetFloat = [](const Any& value)
    {
        if (const auto f = value.tryGet<float>())
        {
            return *f;
        }

        if (const auto i = value.tryGet<int>())
        {
            return static_cast<float>(*i);
        }

        throw ShaderCompileError::internal("expected float argument");
    };

    const auto expectAndGetVec2 = [](const Any& value)
    {
        if (const auto vec = value.tryGet<Vec2>())
            return *vec;

        throw ShaderCompileError::internal(
            formatString("expected argument of type '{}'", Vec2Type::instance()->typeName()));
    };

    const auto expectAndGetVec3 = [](const Any& value)
    {
        if (const auto vec = value.tryGet<Vec3>())
        {
            return *vec;
        }

        throw ShaderCompileError::internal(
            formatString("expected argument of type '{}'", Vec3Type::instance()->typeName()));
    };

    if (builtins.isFloatCtor(symbol))
    {
        const auto values = getArgConstantValues();

        if (values.isEmpty())
        {
            return {};
        }

        return expectAndGetFloat(values.first());
    }

    if (builtins.isIntCtor(symbol))
    {
        throw ShaderCompileError::internal("[not implemented] implicit conversion to int");
    }

    if (builtins.isVec2Ctor(symbol))
    {
        const auto values = getArgConstantValues();

        if (values.isEmpty())
        {
            return {};
        }

        if (symbol == builtins.vec2Ctor_x_y.get())
        {
            const auto x = expectAndGetFloat(values[0]);
            const auto y = expectAndGetFloat(values[1]);

            return Vec2(x, y);
        }

        if (symbol == builtins.vec2Ctor_xy.get())
        {
            return Vec2(expectAndGetFloat(values.first()));
        }

        throw ShaderCompileError::internal("unknown Vector constructor call");
    }

    if (builtins.isVec4Ctor(symbol))
    {
        const auto values = getArgConstantValues();

        if (values.isEmpty())
        {
            return none;
        }

        if (symbol == builtins.vec4Ctor_x_y_z_w.get())
        {
            const auto x = expectAndGetFloat(values[0]);
            const auto y = expectAndGetFloat(values[1]);
            const auto z = expectAndGetFloat(values[2]);
            const auto w = expectAndGetFloat(values[3]);

            return Vec4(x, y, z, w);
        }

        if (symbol == builtins.vec4Ctor_xy_zw.get())
        {
            const auto xy = expectAndGetVec2(values[0]);
            const auto zw = expectAndGetVec2(values[1]);

            return Vec4(xy, zw);
        }

        if (symbol == builtins.vec4Ctor_xy_z_w.get())
        {
            const auto xy = expectAndGetVec2(values[0]);
            const auto z  = expectAndGetFloat(values[1]);
            const auto w  = expectAndGetFloat(values[2]);

            return Vec4(xy, z, w);
        }

        if (symbol == builtins.vec4Ctor_xyz_w.get())
        {
            const auto xyz = expectAndGetVec3(values[0]);
            const auto w   = expectAndGetFloat(values[1]);

            return Vec4(xyz, w);
        }

        throw ShaderCompileError::internal("Unknown Vec3 ctor call");
    }

    return none;
}

const Expr* FunctionCallExpr::callee() const
{
    return _callee.get();
}

FunctionCallExpr::FunctionCallExpr(
    const SourceLocation& location,
    UniquePtr<Expr>       callee,
    List<UniquePtr<Expr>> args)
    : Expr(location)
    , _callee(std::move(callee))
    , _args(std::move(args))
{
}

ScientificIntLiteralExpr::ScientificIntLiteralExpr(const SourceLocation& location, StringView value)
    : Expr(location)
    , _value(value)
{
    setType(FloatType::instance());
}

void ScientificIntLiteralExpr::onVerify([[maybe_unused]] SemaContext& context, [[maybe_unused]] Scope& scope)
{
}

StringView ScientificIntLiteralExpr::value() const
{
    return _value;
}

void HexadecimalIntLiteralExpr::onVerify([[maybe_unused]] SemaContext& context, [[maybe_unused]] Scope& scope)
{
    setType(IntType::instance());
}

HexadecimalIntLiteralExpr::HexadecimalIntLiteralExpr(const SourceLocation& location, StringView value)
    : Expr(location)
    , _value(value)
{
}

StringView HexadecimalIntLiteralExpr::value() const
{
    return _value;
}

RangeExpr::RangeExpr(const SourceLocation& location, UniquePtr<Expr> start, UniquePtr<Expr> end)
    : Expr(location)
    , _start(std::move(start))
    , _end(std::move(end))
{
}

void RangeExpr::onVerify(SemaContext& context, Scope& scope)
{
    _start->verify(context, scope);
    _end->verify(context, scope);

    if (_start->type() != _end->type())
    {
        throw ShaderCompileError(
            location(),
            formatString(
                "Type mismatch between range start and end ({} to {}).",
                _start->type()->typeName(),
                _end->type()->typeName()));
    }

    setType(_start->type());
}

const Expr* RangeExpr::start() const
{
    return _start.get();
}

const Expr* RangeExpr::end() const
{
    return _end.get();
}

bool RangeExpr::accessesSymbol(const Decl* symbol, bool transitive) const
{
    return _start->accessesSymbol(symbol, transitive) or _end->accessesSymbol(symbol, transitive);
}

UnaryOpExpr::UnaryOpExpr(const SourceLocation& location, UnaryOpKind kind, UniquePtr<Expr> expr)
    : Expr(location)
    , _kind(kind)
    , _expr(std::move(expr))
{
}

void UnaryOpExpr::onVerify(SemaContext& context, Scope& scope)
{
    _expr->verify(context, scope);
    setType(_expr->type());
    setSymbol(_expr->symbol());
}

UnaryOpKind UnaryOpExpr::unaryOpKind() const
{
    return _kind;
}

const Expr* UnaryOpExpr::expr() const
{
    return _expr.get();
}

Any UnaryOpExpr::evaluateConstantValue(SemaContext& context, Scope& scope) const
{
    const auto value = _expr->evaluateConstantValue(context, scope);

    if (not value)
    {
        return none;
    }

    if (value.type() == AnyType::Int)
    {
        if (_kind == UnaryOpKind::Negate)
        {
            return -value.get<int>();
        }
    }
    else if (value.type() == AnyType::Float)
    {
        if (_kind == UnaryOpKind::Negate)
        {
            return -value.get<float>();
        }
    }
    else if (value.type() == AnyType::Bool)
    {
        if (_kind == UnaryOpKind::LogicalNot)
        {
            return not value.get<bool>();
        }
    }

    return none;
}

bool UnaryOpExpr::accessesSymbol(const Decl* symbol, bool transitive) const
{
    return _expr->accessesSymbol(symbol, transitive);
}

ParenExpr::ParenExpr(const SourceLocation& location, UniquePtr<Expr> expr)
    : Expr(location)
    , _expr(std::move(expr))
{
}

void ParenExpr::onVerify(SemaContext& context, Scope& scope)
{
    _expr->verify(context, scope);
    setType(_expr->type());
    setSymbol(_expr->symbol());
}

const Expr* ParenExpr::expr() const
{
    return _expr.get();
}

Any ParenExpr::evaluateConstantValue(SemaContext& context, Scope& scope) const
{
    return _expr->evaluateConstantValue(context, scope);
}

bool ParenExpr::accessesSymbol(const Decl* symbol, bool transitive) const
{
    return _expr->accessesSymbol(symbol, transitive);
}

TernaryExpr::TernaryExpr(
    const SourceLocation& location,
    UniquePtr<Expr>       conditionExpr,
    UniquePtr<Expr>       trueExpr,
    UniquePtr<Expr>       falseExpr)
    : Expr(location)
    , _conditionExpr(std::move(conditionExpr))
    , _trueExpr(std::move(trueExpr))
    , _falseExpr(std::move(falseExpr))
{
}

void TernaryExpr::onVerify(SemaContext& context, Scope& scope)
{
    _conditionExpr->verify(context, scope);
    _trueExpr->verify(context, scope);
    _falseExpr->verify(context, scope);

    if (_trueExpr->type() != _falseExpr->type())
    {
        throw ShaderCompileError(
            location(),
            formatString(
                "Type mismatch between true-expression ('{}') and false-expression "
                "('{}'); both expressions must be of "
                "the same type.",
                _trueExpr->type()->typeName(),
                _falseExpr->type()->typeName()));
    }

    setType(_trueExpr->type());
}

const Expr* TernaryExpr::conditionExpr() const
{
    return _conditionExpr.get();
}

const Expr* TernaryExpr::trueExpr() const
{
    return _trueExpr.get();
}

const Expr* TernaryExpr::falseExpr() const
{
    return _falseExpr.get();
}

Any TernaryExpr::evaluateConstantValue(SemaContext& context, Scope& scope) const
{
    const auto conditionValue = _conditionExpr->evaluateConstantValue(context, scope);

    if (not conditionValue)
    {
        return none;
    }

    auto trueValue = _trueExpr->evaluateConstantValue(context, scope);

    if (not trueValue)
    {
        return none;
    }

    auto falseValue = _falseExpr->evaluateConstantValue(context, scope);

    if (not falseValue)
    {
        return none;
    }

    if (conditionValue.type() == AnyType::Bool)
    {
        assume(trueValue.type() == falseValue.type());
        return conditionValue.get<bool>() ? trueValue : falseValue;
    }

    return none;
}

bool TernaryExpr::accessesSymbol(const Decl* symbol, bool transitive) const
{
    return _conditionExpr->accessesSymbol(symbol, transitive)
           or _trueExpr->accessesSymbol(symbol, transitive)
           or _falseExpr->accessesSymbol(symbol, transitive);
}

ArrayExpr::ArrayExpr(const SourceLocation& location, const Type* type, UniquePtr<Expr> sizeExpr)
    : Expr(location)
    , _sizeExpr(std::move(sizeExpr))
{
    setType(type);
}

void ArrayExpr::onVerify(SemaContext& context, Scope& scope)
{
    _sizeExpr->verify(context, scope);

    const auto* elementType = type()->resolve(context, scope);

    auto intLitExpr = makeUnique<IntLiteralExpr>(
        _sizeExpr->location(),
        _sizeExpr->evaluateConstantValue(context, scope).get<int>());

    intLitExpr->verify(context, scope);

    const auto& arrType =
        context.typeCache().createArrayType(type()->location(), elementType, std::move(intLitExpr));

    setType(arrType->resolve(context, scope));
}

Any ArrayExpr::evaluateConstantValue(SemaContext& context, Scope& scope) const
{
    return _sizeExpr->evaluateConstantValue(context, scope);
}

bool ArrayExpr::accessesSymbol(const Decl* symbol, bool transitive) const
{
    return Expr::accessesSymbol(symbol, transitive);
}

const Expr* ArrayExpr::sizeExpr() const
{
    return _sizeExpr.get();
}
} // namespace Polly::ShaderCompiler
