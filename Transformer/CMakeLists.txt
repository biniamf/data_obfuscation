set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
set(LLVM_LINK_COMPONENTS support)
set(LLVM_USED_LIBS clangTooling clangBasic clangAST)

add_clang_executable(Transformer
  Transformer.cpp
  Normalizer.cpp
  VariableDeclEncoder.cpp
  EncodingTransformationContext.cpp
  )

target_link_libraries(Transformer
  clangTooling
  clangBasic
  clangASTMatchers
  )

