/*
 * VariableDeclEncoder.h
 *
 *  Created on: Dec 17, 2013
 *      Author: Biniam Fisseha Demissie
 */

#ifndef VARIABLEDECLENCODER_H_
#define VARIABLEDECLENCODER_H_

#include <cstdio>
#include <string>
#include <sstream>

#include "clang/AST/Stmt.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/FileManager.h"

#include "clang/AST/StmtVisitor.h"
#include "clang/Basic/TargetOptions.h"
#include "clang/Basic/TargetInfo.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Parse/ParseAST.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Rewrite/Frontend/Rewriters.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/ADT/IntrusiveRefCntPtr.h"

#include "EncodingTransformationContext.h"

using namespace clang;
using namespace std;

class VariableDeclEncoderVisitor: public RecursiveASTVisitor<VariableDeclEncoderVisitor> {
public:
	VariableDeclEncoderVisitor(Rewriter &rewriter, EncodingTransformationContext &context, SourceManager &sourceMgr) :
        theRewriter(rewriter), theContext(context), theSourceMgr(sourceMgr){

}
	bool VisitVarDecl(VarDecl *variableDecl);
	virtual ~VariableDeclEncoderVisitor();

private:
    Rewriter &theRewriter;
    EncodingTransformationContext &theContext;
    SourceManager &theSourceMgr;
};

#endif /* VARIABLEDECLENCODER_H_ */
