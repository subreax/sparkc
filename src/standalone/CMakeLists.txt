set(SPARKC_SRC
    ../spark/frontend/lexer/Token.cpp 
    ../spark/common/StringRef.cpp
    ../spark/common/Constant.cpp
    ../spark/backend/rv/asm/Rv32Base.cpp
    ../spark/skr/value/SkrValue.cpp
    ../spark/skr/optimizer/SkrCfgOverloadings.cpp
    ../spark/SparkCompiler.cpp
)

add_executable(sparkc 
    ${SPARKC_SRC}
    main.cpp 
    printer/skr/SkrPrinter.cpp
    printer/rva/RvaPrinter.cpp
)
