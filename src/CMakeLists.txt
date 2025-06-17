Include(FetchContent)

add_executable(sparkc 
    main.cpp 
    spark/frontend/lexer/Token.cpp 
    spark/common/StringRef.cpp
    spark/common/Constant.cpp
    spark/backend/rv/asm/Rv32Base.cpp
    spark/skr/value/SkrValue.cpp
    printer/skr/SkrPrinter.cpp
    printer/rva/RvaPrinter.cpp
    printer/ast/AstMermaidPrinter.cpp
)
