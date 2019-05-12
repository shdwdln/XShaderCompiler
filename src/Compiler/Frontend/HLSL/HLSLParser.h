/*
 * HLSLParser.h
 * 
 * This file is part of the XShaderCompiler project (Copyright (c) 2014-2019 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef XSC_HLSL_PARSER_H
#define XSC_HLSL_PARSER_H


#include "SLParser.h"
#include "HLSLScanner.h"
#include "SymbolTable.h"
#include <map>


namespace Xsc
{


// Syntax parser class for the shading language HLSL.
class HLSLParser : public SLParser
{

    public:

        HLSLParser(Log* log = nullptr);

        ProgramPtr ParseSource(
            const SourceCodePtr&        source,
            const NameMangling&         nameMangling,
            const InputShaderVersion    versionIn,
            bool                        rowMajorAlignment   = false,
            bool                        enableWarnings      = false
        );

    private:

        ScannerPtr MakeScanner() override;

        // Returns true if the current token is a data type.
        bool IsDataType() const;

        // Returns true if the current token is a base data type (i.e. scalar, vector, matrix, or string type denoter).
        bool IsBaseDataType() const;

        // Returns true if the current token is a literal.
        bool IsLiteral() const;

        // Returns true if the current token is part of an arithmetic unary expression, i.e. either '-' or '+'.
        bool IsArithmeticUnaryExpr() const;

        // Returns true if the current token is a modifier of a type specifier.
        bool IsModifier() const;

        // Converts the specified expression to a type name expression if it is a left-hand-side of a cast expression.
        TypeSpecifierPtr MakeTypeSpecifierIfLhsOfCastExpr(const ExprPtr& expr);

        // Overrides the token accept function to process all directives before the actual parsing.
        TokenPtr AcceptIt() override;

        // Processes the specified directive (only '#line'-directive are allowed after pre-processing).
        void ProcessDirective(const std::string& ident);
        void ProcessDirectiveLine();
        void ProcessDirectivePragma();

        /* ----- Symbol table ----- */

        // Opens a new scope of the type name symbol table.
        void OpenScope();

        // Closes the current scope of the type name symbol table.
        void CloseScope();

        // Registers the specified identifier as type name, to detect cast expressions.
        void RegisterTypeName(const std::string& ident);

        // Returns true if the specified identifier is a valid type name within the current scope.
        bool IsRegisteredTypeName(const std::string& ident) const;

        // Makes a new alias declaration statement and registers it's identifier in the symbol table.
        AliasDeclStmtPtr MakeAndRegisterBuildinAlias(const DataType dataType, const std::string& ident);

        // Generates all pre defined type aliases (e.g. 'typedef int DWORD').
        void GeneratePreDefinedTypeAliases(Program& ast);

        // Creates a new var-decl statement with the current matrix pack alignment type modifier.
        TypeSpecifierPtr MakeTypeSpecifierWithPackAlignment();

        /* ----- Parsing ----- */

        ProgramPtr                      ParseProgram(const SourceCodePtr& source);

        CodeBlockPtr                    ParseCodeBlock() override;
        VarDeclStmtPtr                  ParseParameter() override;
        StmtPtr                         ParseLocalStmt() override;
        StmtPtr                         ParseForStmtInitializer() override;
        SwitchCasePtr                   ParseSwitchCase() override;
        VarDeclPtr                      ParseVarDecl(VarDeclStmt* declStmtRef, const TokenPtr& identTkn = nullptr) override;

        SamplerValuePtr                 ParseSamplerValue();
        AttributePtr                    ParseAttribute();
        RegisterPtr                     ParseRegister(bool parseColon = true);
        PackOffsetPtr                   ParsePackOffset(bool parseColon = true);
        TypeSpecifierPtr                ParseTypeSpecifier(bool parseVoidType = false);

        BufferDeclPtr                   ParseBufferDecl(BufferDeclStmt* declStmtRef, const TokenPtr& identTkn = nullptr);
        SamplerDeclPtr                  ParseSamplerDecl(SamplerDeclStmt* declStmtRef, const TokenPtr& identTkn = nullptr);
        StructDeclPtr                   ParseStructDecl(bool parseStructTkn = true, const TokenPtr& identTkn = nullptr);
        AliasDeclPtr                    ParseAliasDecl(TypeDenoterPtr typeDenoter);
        FunctionDeclPtr                 ParseFunctionDecl(BasicDeclStmt* declStmtRef, const TypeSpecifierPtr& returnType = nullptr, const TokenPtr& identTkn = nullptr);
        UniformBufferDeclPtr            ParseUniformBufferDecl();

        StmtPtr                         ParseGlobalStmt();
        StmtPtr                         ParseGlobalStmtPrimary();
        StmtPtr                         ParseGlobalStmtWithTypeSpecifier();
        StmtPtr                         ParseGlobalStmtWithSamplerTypeDenoter();
        StmtPtr                         ParseGlobalStmtWithBufferTypeDenoter();
        BasicDeclStmtPtr                ParseFunctionDeclStmt(const TypeSpecifierPtr& returnType = nullptr, const TokenPtr& identTkn = nullptr);
        BasicDeclStmtPtr                ParseUniformBufferDeclStmt();
        BufferDeclStmtPtr               ParseBufferDeclStmt(const BufferTypeDenoterPtr& typeDenoter = nullptr, const TokenPtr& identTkn = nullptr);
        SamplerDeclStmtPtr              ParseSamplerDeclStmt(const SamplerTypeDenoterPtr& typeDenoter = nullptr, const TokenPtr& identTkn = nullptr);
        VarDeclStmtPtr                  ParseVarDeclStmt();
        AliasDeclStmtPtr                ParseAliasDeclStmt();

        StmtPtr                         ParseStmt(bool allowAttributes = true);
        StmtPtr                         ParseStmtPrimary();
        StmtPtr                         ParseStmtWithStructDecl();
        StmtPtr                         ParseStmtWithIdent();

        ExprPtr                         ParsePrimaryExpr() override;
        ExprPtr                         ParsePrimaryExprPrefix();
        ExprPtr                         ParseExprWithSuffixOpt(ExprPtr expr);
        LiteralExprPtr                  ParseLiteralExpr();
        ExprPtr                         ParseTypeSpecifierOrCallExpr();
        ExprProxyPtr                    ParseTypeSpecifierExprProxy();
        UnaryExprPtr                    ParseUnaryExpr();
        UnaryExprPtr                    ParsePostUnaryExpr(const ExprPtr& expr);
        ExprPtr                         ParseExprWithBracketPrefix();
        IdentExprPtr                    ParseIdentExpr(const ExprPtr& expr = nullptr);
        AssignExprPtr                   ParseAssignExpr(const ExprPtr& expr);
        ExprPtr                         ParseIdentOrCallExpr(const ExprPtr& expr = nullptr);
        CallExprPtr                     ParseCallExpr(const IdentExprPtr& identExpr = nullptr, const TypeDenoterPtr& typeDenoter = nullptr);
        CallExprPtr                     ParseCallExprWithPrefixOpt(const ExprPtr& prefixExpr = nullptr, bool isStatic = false, const TokenPtr& identTkn = nullptr);
        CallExprPtr                     ParseCallExprAsTypeCtor(const TypeDenoterPtr& typeDenoter);

        std::vector<StmtPtr>            ParseGlobalStmtList();
        std::vector<VarDeclStmtPtr>     ParseAnnotationList();
        std::vector<RegisterPtr>        ParseRegisterList(bool parseFirstColon = true);
        std::vector<AttributePtr>       ParseAttributeList();
        std::vector<BufferDeclPtr>      ParseBufferDeclList(BufferDeclStmt* declStmtRef, const TokenPtr& identTkn = nullptr);
        std::vector<SamplerDeclPtr>     ParseSamplerDeclList(SamplerDeclStmt* declStmtRef, const TokenPtr& identTkn = nullptr);
        std::vector<SamplerValuePtr>    ParseSamplerValueList();
        std::vector<AliasDeclPtr>       ParseAliasDeclList(TypeDenoterPtr typeDenoter);

        std::string                     ParseIdentWithNamespaceOpt(IdentExprPtr& namespaceExpr, TokenPtr identTkn = nullptr, SourceArea* area = nullptr);

        TypeDenoterPtr                  ParseTypeDenoter(bool allowVoidType = true, StructDeclPtr* structDecl = nullptr);
        TypeDenoterPtr                  ParseTypeDenoterPrimary(StructDeclPtr* structDecl = nullptr);
        TypeDenoterPtr                  ParseTypeDenoterWithStructDeclOpt(StructDeclPtr& structDecl, bool allowVoidType = true);
        VoidTypeDenoterPtr              ParseVoidTypeDenoter();
        BaseTypeDenoterPtr              ParseBaseTypeDenoter();
        BaseTypeDenoterPtr              ParseBaseVectorTypeDenoter();
        BaseTypeDenoterPtr              ParseBaseMatrixTypeDenoter();
        BufferTypeDenoterPtr            ParseBufferTypeDenoter();
        SamplerTypeDenoterPtr           ParseSamplerTypeDenoter();
        StructTypeDenoterPtr            ParseStructTypeDenoter();
        StructTypeDenoterPtr            ParseStructTypeDenoterWithStructDeclOpt(StructDeclPtr& structDecl);
        AliasTypeDenoterPtr             ParseAliasTypeDenoter(std::string ident = "");

        void                            ParseAndIgnoreTechniquesAndNullStmts();
        void                            ParseAndIgnoreTechnique();

        void                            ParseVarDeclSemantic(VarDecl& varDecl, bool allowPackOffset = true);
        void                            ParseFunctionDeclSemantic(FunctionDecl& funcDecl);

        DataType                        ParseDataType(const std::string& keyword);
        PrimitiveType                   ParsePrimitiveType();
        InterpModifier                  ParseInterpModifier();
        TypeModifier                    ParseTypeModifier();
        StorageClass                    ParseStorageClass();
        UniformBufferType               ParseUniformBufferType();
        BufferType                      ParseBufferType();
        SamplerType                     ParseSamplerType();
        IndexedSemantic                 ParseSemantic(bool parseColon = true);

        std::string                     ParseSamplerStateTextureIdent();

        bool                            ParseModifiers(TypeSpecifier* typeSpecifier, bool allowPrimitiveType = false);

    private:

        using TypeNameSymbolTable = SymbolTable<bool>;

        // Symbol table for type name (i.e. structure and typedef identifiers) to detect cast expression, which are not context free.
        TypeNameSymbolTable typeNameSymbolTable_;

        // True, if semantics are parsed for D3D10+ shader.
        bool                useD3D10Semantics_      = true;

        // True, if Cg keyword extensions are enabled.
        bool                enableCgKeywords_       = false;

        // True, if matrix packing is globally set to row major.
        bool                rowMajorAlignment_      = false;

};


} // /namespace Xsc


#endif



// ================================================================================
