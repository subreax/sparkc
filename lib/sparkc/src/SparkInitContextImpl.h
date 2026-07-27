#pragma once
#include "sparkc/SparkInitContext.h"
#include "sparkc/common/IdentifierGen.h"
#include "sparkc/symbol/SymbolTable.h"
#include "sparkc/type/TypeTable.h"
#include "sparkc/backend/rv/asm/RvAssembler.h"

class SparkInitContextImpl : public SparkInitContext {
public:
    SparkInitContextImpl(
        IdentifierGen& idGen,
        SymbolTable& symbolTable,
        TypeTable& typeTable,
        RvAssembler& assembler
    )
        : SparkInitContext(symbolTable.getTypeFactory())
        , idGen(idGen)
        , symbolTable(symbolTable)
        , typeTable(typeTable)
        , assembler(assembler) { }

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