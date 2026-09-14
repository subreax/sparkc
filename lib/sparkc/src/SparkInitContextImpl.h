#pragma once
#include "sparkc/SparkInitContext.h"
#include "SparkBuildContext.h"

class SparkInitContextImpl : public SparkInitContext {
public:
    SparkInitContextImpl(SparkBuildContext& ctx)
        : SparkInitContext(ctx.symTable.getTypeFactory())
        , idGen(ctx.idGen)
        , symbolTable(ctx.symTable)
        , typeTable(ctx.typeTable)
        , assembler(ctx.assembler) { }

    void bindFunction(
        void* ptr,
        const char* name,
        SymbolType* type,
        std::initializer_list<SymbolType*> params
    ) {
        auto nameRef = idGen.copy(StringRef::cstr(name));
        symbolTable.declareFunc(nameRef, type, std::move(params));
        assembler.addExternalLabel(nameRef, ptr);
    }

    void addStruct(
        const char* tag,
        std::initializer_list<SparkExtStructField> fields
    ) {
        std::vector<StructField> vecFields;
        for (auto& field : fields) {
            vecFields.emplace_back(
                idGen.copy(StringRef::cstr(field.name)),
                field.type
            );
        }
        typeTable.declare(idGen.copy(StringRef::cstr(tag)), vecFields);
    }

private:
    IdentifierGen& idGen;
    SymbolTable& symbolTable;
    TypeTable& typeTable;
    RvAssembler& assembler;
};