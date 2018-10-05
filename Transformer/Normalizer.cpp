/*
 * Normalizer.cpp
 *
 *  Created on: Dec 03, 2013
 *      Author: Biniam Fisseha Demissie
 */

#include <cstdio>
#include <string>
#include <sstream>

#include "clang/AST/Stmt.h"

#include "clang/AST/ASTContext.h"
#include "clang/AST/ASTConsumer.h"

#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/TextDiagnosticPrinter.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/FileManager.h"

#include "clang/Basic/Version.h"
#include "clang/Frontend/Utils.h"
#include "clang/Lex/HeaderSearch.h"
#include "clang/Lex/HeaderSearchOptions.h"
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

#include "Normalizer.h"

using namespace clang;
using namespace std;

class ConditionalOPVisitorEx: public RecursiveASTVisitor<ConditionalOPVisitorEx>
{
public:
	ConditionalOPVisitorEx(Rewriter &rewriter, SourceManager &sourceMgr) :
        theRewriter(rewriter), theSourceMgr(sourceMgr)
		{
			foundCondOP = false;
			foundCallExpr = false;
			expr = "";
			newVar = "";
		}

	bool foundCondOperator()  {
		return foundCondOP;
	}

	bool foundCallExpression()  {
		return foundCallExpr;
	}

	string getExpr() {
		return expr;
	}

	string getVarDecl() {
		return varDecl;
	}

    string getExprAsString(Expr *expression)
    {
    	LangOptions lopt;
    	SourceLocation begin(expression->getLocStart()), endToken(expression->getLocEnd());
    	SourceLocation end(clang::Lexer::getLocForEndOfToken(endToken, 0, theSourceMgr, lopt));

    	return string(theSourceMgr.getCharacterData(begin),	theSourceMgr.getCharacterData(end)-theSourceMgr.getCharacterData(begin));

    	/*
    	// Returns until the beginning of the last token
    	SourceRange exprRange = expression->getSourceRange();
    	string exprString;

    	int rangeSize = theRewriter.getRangeSize(exprRange);
    	if (rangeSize == -1)
    		return "";

    	SourceLocation startLoc = exprRange.getBegin();
    	const char *strStart = theSourceMgr.getCharacterData(startLoc);

    	exprString.assign(strStart, rangeSize);

    	return exprString;
    	*/
    }

	// This works ok for x = (y ? 10 : 20) + (z ? 30 : 40) ==> x = var_1231 + var_1222;
    // where the two vars are random and are result of if-then-else stmt
    bool VisitAbstractConditionalOperator(AbstractConditionalOperator *condOP) {
		foundCondOP = true;
		Expr *cond = condOP->getCond();
		Expr *truePart = condOP->getTrueExpr();
		Expr *falsePart = condOP->getFalseExpr();
		SourceLocation startLoc = condOP->getLocStart();
		SourceRange range = condOP->getSourceRange();

		char buf[128] = "";

		sprintf(buf, "%d", (rand() % 10000) + 1000);

		newVar = "var_";
		newVar += buf;
		expr += "\tif (" + getExprAsString(cond) + ") {\n\t\t";
		expr += newVar + " = " + getExprAsString(truePart) + ";\n\t}\n";
		expr += "\telse {\n\t\t";
		expr += newVar + " = " + getExprAsString(falsePart) + ";\n\t}\n\n";

		type = truePart->getType().getAsString();

		varDecl += "\t" + type + " " + newVar + ";\n";

		theRewriter.RemoveText(range);
		theRewriter.InsertText(startLoc, newVar, true, true);

		return true;
	}

    bool VisitCallExpr(CallExpr *CExpr) {
    	foundCallExpr;

      	return true;
    }

private:
	Rewriter &theRewriter;
	SourceManager &theSourceMgr;
	string newVar, varDecl, expr, type;
	bool foundCondOP, foundCallExpr;
};


class CallExprBinaryOperatorHandler: public RecursiveASTVisitor<CallExprBinaryOperatorHandler>
{
public:
	CallExprBinaryOperatorHandler(Rewriter &rewriter, SourceManager &sourceMgr) :
        theRewriter(rewriter), theSourceMgr(sourceMgr)
		{
			foundCondOP = false;
			newVarDeclared = false;
			expr = "";
			newVar = "";
		}

	bool foundCondOperator()  {
		return foundCondOP;
	}

	bool varDeclared()  {
		return newVarDeclared;
	}

	string getExpr() {
		return expr;
	}

	string getVarDecl() {
		return varDecl;
	}

    string getExprAsString(Expr *expression)
    {
    	LangOptions lopt;
    	SourceLocation begin(expression->getLocStart()), endToken(expression->getLocEnd());
    	SourceLocation end(clang::Lexer::getLocForEndOfToken(endToken, 0, theSourceMgr, lopt));

    	return string(theSourceMgr.getCharacterData(begin),	theSourceMgr.getCharacterData(end)-theSourceMgr.getCharacterData(begin));

    }

	// This works for foo(x=10+11) ==> foo(x);
    bool VisitBinaryOperator(BinaryOperator *bOP) {
		foundCondOP = true;
		SourceRange range = bOP->getSourceRange();
		SourceLocation startLoc = bOP->getLocStart();
		char buf[128] = "";

		sprintf(buf, "%d", (rand() % 10000) + 1000);

		newVar = "var_";
		newVar += buf;
		newVarDeclared = true;

		expr = getExprAsString(bOP->getLHS());
		expr += bOP->getOpcodeStr().str();
		expr += getExprAsString(bOP->getRHS());

		type = bOP->getRHS()->getType().getAsString();

		varDecl += type + " " + newVar + ";\n";

		expr = newVar + " = " + expr + ";";


		theRewriter.RemoveText(range);
		theRewriter.InsertText(startLoc, newVar, true, true);

		// Should stop after finding the first BOP
		// otherwise will end up duplicate
		return false;
	}

private:
	Rewriter &theRewriter;
	SourceManager &theSourceMgr;
	string newVar, varDecl, expr, type;
	bool foundCondOP, newVarDeclared;
};


class CallExprHanlder: public RecursiveASTVisitor<CallExprHanlder>
{
public:
	CallExprHanlder(Rewriter &rewriter, SourceManager &sourceMgr, SourceLocation sourceLoc) :
        theRewriter(rewriter), theSourceMgr(sourceMgr), theSourceLoc(sourceLoc)
		{
			foundCallExpr = false;
			newVarDeclared = false;
			expr = "";
			newVar = "";
		}

	bool foundCallExpression()  {
		return foundCallExpr;
	}

	bool varDeclared()  {
		return newVarDeclared;
	}

	string getExpr() {
		return expr;
	}

	string getVarDecl() {
		return varDecl;
	}

    string getExprAsString(Expr *expression)
    {
    	LangOptions lopt;
    	SourceLocation begin(expression->getLocStart()), endToken(expression->getLocEnd());
    	SourceLocation end(clang::Lexer::getLocForEndOfToken(endToken, 0, theSourceMgr, lopt));

    	return string(theSourceMgr.getCharacterData(begin),	theSourceMgr.getCharacterData(end)-theSourceMgr.getCharacterData(begin));

    }

    bool VisitCallExpr(CallExpr *callExpr) {
    	foundCallExpr = true;
    	unsigned int argCount = callExpr->getNumArgs();

    	for (unsigned int i=0; i<argCount; i++) {
    		CallExprBinaryOperatorHandler checkBOP(theRewriter, theSourceMgr);
    		checkBOP.TraverseStmt(callExpr->getArg(i));

    		if (checkBOP.foundCondOperator()) {
				string expr = checkBOP.getExpr();

				string toBeInserted = (checkBOP.varDeclared() ? checkBOP.getVarDecl() : "") + expr + "\n\n";

				theRewriter.InsertText(theSourceLoc, toBeInserted, true, true);
    		}
    	}

    	return true;
    }

private:
	Rewriter &theRewriter;
	SourceManager &theSourceMgr;
	string newVar, varDecl, expr, type;
	bool foundCallExpr, newVarDeclared;
	SourceLocation theSourceLoc;
};



class GroupedVairableDeclNormalizer: public RecursiveASTVisitor<GroupedVairableDeclNormalizer>
{
public:
	GroupedVairableDeclNormalizer(Rewriter &rewriter, SourceManager &sourceMgr) :
            theRewriter(rewriter), theSourceMgr(sourceMgr)
			{}

    string getExprAsString(Expr *expression)
    {
    	LangOptions lopt;
    	SourceLocation begin(expression->getLocStart()), endToken(expression->getLocEnd());
    	SourceLocation end(clang::Lexer::getLocForEndOfToken(endToken, 0, theSourceMgr, lopt));

    	return string(theSourceMgr.getCharacterData(begin),	theSourceMgr.getCharacterData(end)-theSourceMgr.getCharacterData(begin));

    	/*
    	SourceRange exprRange = expression->getSourceRange();

    	int rangeSize = theRewriter.getRangeSize(exprRange);
    	if (rangeSize == -1)
    		return false;

    	SourceLocation startLoc = exprRange.getBegin();
    	const char *strStart = theSourceMgr.getCharacterData(startLoc);

    	exprString.assign(strStart, rangeSize);

    	return true;
    	*/
    }

    bool removeDecl(DeclStmt *DS)
    {
    	SourceRange range;
    	SourceLocation startLoc;
    	std::string strChange;

    	if (!DS->isSingleDecl()) {

    		range = DS->getSourceRange();
    		startLoc = DS->getLocStart();
			DeclGroupRef declGroupRef = DS->getDeclGroup();

			for (DeclGroupRef::iterator I=declGroupRef.begin(), E=declGroupRef.end(); I != E; ++I) {
				VarDecl *V = dyn_cast_or_null<VarDecl>(*I);

				if (V) {
					ValueDecl *valueDecl = dyn_cast_or_null<ValueDecl>(V);

					if (valueDecl) {
						string type = valueDecl->getType().getAsString();

						if (type.compare("int") == 0 || type.compare("int *") == 0 || type.compare("unsigned long") == 0) {

							if (V->hasInit()) {

								strChange += "\n" + type + " ";
								strChange += V->getNameAsString();
								strChange += ";\n";

								strChange += V->getNameAsString();
								strChange += " = ";

								string exprString;
								// Here, we don't care what the RHS Expr is
								exprString = getExprAsString(V->getInit());

								strChange += exprString;
								strChange += ";\n";
							}
							else {
								strChange += "\n" + type + " ";
								strChange += V->getNameAsString();
								strChange += ";";
							}
						}
					}
				}
			}
			if (strChange.length() != 0) {
				theRewriter.RemoveText(range);
				theRewriter.InsertText(startLoc, strChange, true, true);
			}
    	}
    	else {
    		VarDecl *V = dyn_cast_or_null<VarDecl>(DS->getSingleDecl());

    		if (V) {
    			range = DS->getSingleDecl()->getSourceRange();
    			startLoc = DS->getSingleDecl()->getLocStart();

				ValueDecl *valueDecl = dyn_cast_or_null<ValueDecl>(V);

				if (valueDecl) {
					string type = valueDecl->getType().getAsString();

					if (type.compare("int") == 0 || type.compare("int *") == 0 || type.compare("unsigned long") == 0) {

						// Only if it has initialization
						if (V->hasInit()) {
							theRewriter.RemoveText(range);

							string strInitChange;

							strChange += "\n" + type + " ";
							strChange += V->getNameAsString();
							strChange += ";\n";

							strChange += V->getNameAsString();
							strChange += " = ";

							string exprString;
							// Here, we don't care what the RHS Expr is
							exprString = getExprAsString(V->getInit());

							strChange += exprString;

							if (strChange.length() != 0) {
								theRewriter.RemoveText(range);
								theRewriter.InsertText(startLoc, strChange, true, true);
							}
						}
					}
				}
			}

    	}
    	return true;
    }

    // TODO: function call with expression argument in expression
    // e.g. x = foo(1 + 2 + 3) + 1; => x = foo(var) + 1 where var = 1+2+3
    bool VisitCallExpr_unused(CallExpr *callExpr) {

    	unsigned int argCount = callExpr->getNumArgs();

    	for (unsigned int i=0; i<argCount; i++) {
    		CallExprBinaryOperatorHandler checkBOP(theRewriter, theSourceMgr);
    		checkBOP.TraverseStmt(callExpr->getArg(i));

    		if (checkBOP.foundCondOperator()) {
				SourceLocation startLoc = callExpr->getLocStart();
				string expr = checkBOP.getExpr();

				string toBeInserted = (checkBOP.varDeclared() ? checkBOP.getVarDecl() : "") + expr + "\n";

				theRewriter.InsertText(startLoc, toBeInserted, true, true);
    		}
    	}

    	return true;
    }


    // Handle grouped variable declarations in the body of a function
	bool VisitFunctionDecl_unused(FunctionDecl *function) {

		// we are interested if it has body only
		if (function->hasBody()) {
			Stmt *body = function->getBody();

			for (StmtIterator I=body->child_begin(), E=body->child_end(); I != E; ++I) {
				DeclStmt *DS = dyn_cast_or_null<DeclStmt>(*I);

				// Both single and group declarations
				if (DS) {

					removeDecl(DS);
				}
			}
		}
		return true;
	}

	bool VisitBinaryOperator(BinaryOperator *bOP) {
		string toBeInserted;
		string expr;
		string varDecl;
		SourceLocation startLoc;

		if (bOP->isAssignmentOp()) {
			ConditionalOPVisitorEx condOPCheck(theRewriter, theSourceMgr);
			CallExprHanlder callExprCheck(theRewriter, theSourceMgr, bOP->getLocStart());

			condOPCheck.TraverseStmt(bOP->getRHS());
			callExprCheck.TraverseStmt(bOP->getRHS());

			startLoc = bOP->getLocStart();
			if (condOPCheck.foundCondOperator()) {
				varDecl = condOPCheck.getVarDecl();
				expr = condOPCheck.getExpr();
			}

			if (callExprCheck.foundCallExpression()) {
				varDecl += callExprCheck.getVarDecl();
				expr += callExprCheck.getExpr();

			}
			toBeInserted = varDecl + expr;

			if (toBeInserted.length() !=0 )
				theRewriter.InsertText(startLoc, toBeInserted, true, true);
		}
		return true;
	}

	bool VisitCompoundStmt(CompoundStmt *stmt) {
		for (Stmt::child_iterator I = stmt->child_begin(), E = stmt->child_end(); I != E; ++I) {
			if (Stmt *child = *I) {
				if (DeclStmt *DS = dyn_cast_or_null<DeclStmt>(child)) {
					if (DS) {
						// we have group declarations, transform one per line
						removeDecl(DS);
					}
				}
			}
		}
		return true;
	}

	bool fixForStmt(const DeclStmt *DS, string &strNewDecl, string &strChange) {

		DeclGroupRef declGroupRef = DS->getDeclGroup();
		for (DeclGroupRef::iterator I=declGroupRef.begin(), E=declGroupRef.end(); I != E; ++I) {
		    VarDecl *V = dyn_cast_or_null<VarDecl>(*I);

		    if (V) {
				ValueDecl *valueDecl = dyn_cast_or_null<ValueDecl>(V);

				if (valueDecl) {
					string type = valueDecl->getType().getAsString();
					// New VarDecls
					strNewDecl += type + " ";
					strNewDecl += V->getNameAsString();
					// FIXME: Default indent size should be declared class level
					strNewDecl += ";\n\t";			// assume TAB = default indent

					// Separate multiple declarations with comma
					if (strChange.length() != 0)
						strChange += ", ";

					strChange += V->getNameAsString();
					if (V->hasInit()) {
						strChange += "=";

						string exprString;
						// Here, we don't care what the RHS Expr is
						exprString = getExprAsString(V->getInit());

						strChange += exprString;
					}


				}
		    }
		}
		return true;
	}

	// Handle variable declarations in for() statement
	bool VisitForStmt(ForStmt *forStmt) {
		SourceLocation insertLoc = forStmt->getLocStart();
		Stmt *forInit = forStmt->getInit();
		const DeclStmt *forCondDecl = forStmt->getConditionVariableDeclStmt();
		string strNewDecl, strChange;

		// Check if it has DeclStmt in the for initialization
		DeclStmt *DS = dyn_cast_or_null<DeclStmt>(forInit);

		// Handle both single and group declarations
		if (DS) {
			fixForStmt(DS, strNewDecl, strChange);
			// Apply changes
			SourceRange range = forInit->getSourceRange();
			theRewriter.ReplaceText(range, strChange + ";");
		}

		// e.g. for (...; int x = foo(); ...)
		if (forCondDecl) {
			strChange.clear();
			fixForStmt(forCondDecl, strNewDecl, strChange);
			// Apply changes
			SourceRange range = forCondDecl->getSourceRange();
			theRewriter.ReplaceText(range, "(" + strChange + ")");	// Parentheses to avoid warnings
		}

		if (strNewDecl.length() != 0)
			theRewriter.InsertTextBefore(insertLoc, strNewDecl);
		return true;
	}

private:
	Rewriter &theRewriter;
	SourceManager &theSourceMgr;
};

class NormalaizationConsumer: public ASTConsumer {
public:
	NormalaizationConsumer(Rewriter &rewriter, SourceManager &sourceMgr) :
			Normalizer(rewriter, sourceMgr),
			theRewriter(rewriter)
			{
				headerInserted = false;
			}

    virtual bool HandleTopLevelDecl(DeclGroupRef group) {

//    	if (!headerInserted) {
//			DeclGroupRef::iterator first = group.begin();
//			SourceLocation sLoc = (*first)->getLocStart();
//
//			theRewriter.InsertText(sLoc, "#include \"encoded.h\"\n", true, true);
//			headerInserted = true;
//    	}
    	// Normalize the translation-unit first
    	for (DeclGroupRef::iterator b = group.begin(), e = group.end(); b != e; ++b) {
    		Normalizer.TraverseDecl(*b);
    	}

        return true;
    }

private:
    GroupedVairableDeclNormalizer Normalizer;
    Rewriter &theRewriter;
    bool headerInserted;
};


void Normalizer::NormalizerMain() {

    CompilerInstance theCompInst;
	theCompInst.getLangOpts().CPlusPlus = 1;
    theCompInst.createDiagnostics(NULL, false);

    srand(time(NULL));
    // Initialize target info with the default triple for our platform.
    // IntrusiveRefCntPtr<TargetOptions*> TO ;
    llvm::IntrusiveRefCntPtr<TargetOptions> TO(new TargetOptions());
    TO->Triple = llvm::sys::getDefaultTargetTriple();
    TargetInfo *TI = TargetInfo::CreateTargetInfo(theCompInst.getDiagnostics(), TO.getPtr());
    theCompInst.setTarget(TI);

    theCompInst.createFileManager();
    FileManager &fileMgr = theCompInst.getFileManager();
    theCompInst.createSourceManager(fileMgr);
    SourceManager &sourceMgr = theCompInst.getSourceManager();
    theCompInst.createPreprocessor();
    theCompInst.createASTContext();

    // A Rewriter helps us manage the code rewriting task.
    Rewriter theRewriter;
    theRewriter.setSourceMgr(sourceMgr, theCompInst.getLangOpts());

    // Set the main file handled by the source manager to the input file.
    const FileEntry *fileIn = fileMgr.getFile(this->file);
    sourceMgr.createMainFileID(fileIn);
    theCompInst.getDiagnosticClient().BeginSourceFile(theCompInst.getLangOpts(), &theCompInst.getPreprocessor());

    NormalaizationConsumer theNormalizationConsumer(theRewriter, sourceMgr);

    // Parse the file to AST, registering our consumer as the AST consumer.
    ParseAST(theCompInst.getPreprocessor(), &theNormalizationConsumer, theCompInst.getASTContext());

#ifdef TEST
    const RewriteBuffer *rewriteBuf = theRewriter.getRewriteBufferFor(sourceMgr.getMainFileID());
    llvm::errs() << string(rewriteBuf->begin(), rewriteBuf->end());
#endif

    if (theRewriter.buffer_begin() != theRewriter.buffer_end())
    	theRewriter.overwriteChangedFiles();

}