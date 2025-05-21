Include(FetchContent)

add_executable(sparkc 
    main.cpp 
    spark/frontend/Token.cpp 
    spark/common/StringRef.cpp
    spark/backend/rv/asm/Rv32Base.cpp
    printer/skr/SkrPrinter.cpp
    printer/rva/RvaPrinter.cpp
)
