/*
 * Transformer.cpp
 *
 *  Created on: Dec 03, 2013
 *      Author: Biniam Fisseha Demissie
 */

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
#include "Normalizer.h"
#include "VariableDeclEncoder.h"

using namespace clang;
using namespace std;



class ExprBuilder {

public:
	ExprBuilder(){}

	void add(string expr) {
		this->exprList.push_back(expr);
	}

	void addDeclRef(DeclRefExpr *DR) {
		this->declRefList.push_back(DR);
	}

	void addBOP(BinaryOperator *bOP) {
		this->binaryOPList.push_back(bOP);
	}

	bool findBOP(BinaryOperator *bOP) {
		for (vector<BinaryOperator*>::iterator I=this->binaryOPList.begin(); I != this->binaryOPList.end(); ++I) {
			if (*I == bOP)
				return true;
		}
		return false;
	}

	void addIntLiteral(IntegerLiteral *IT)	{
		this->intRefList.push_back(IT);
	}

	bool findDeclRef(DeclRefExpr *DR) {

		for (vector<DeclRefExpr*>::iterator I=this->declRefList.begin(); I != this->declRefList.end(); ++I) {
			if (*I == DR)
				return true;
		}
		return false;
	}

	bool findIntLiteral(IntegerLiteral *IT) {

		for (vector<IntegerLiteral*>::iterator I=this->intRefList.begin(); I != this->intRefList.end(); ++I) {
			if (*I == IT)
				return true;
		}
		return false;
	}

	string getExprString() {
		string expr;

		for (vector<string>::iterator I=this->exprList.begin(); I != this->exprList.end(); ++I) {
			expr += *I;
		}
		return expr;
	}

	void clear() {
		this->exprList.clear();
	}

private:
	vector<string> exprList;
	vector<DeclRefExpr*> declRefList;
	vector<IntegerLiteral*> intRefList;
	vector<BinaryOperator*> binaryOPList;
};

class DeclRefVisitor: public RecursiveASTVisitor<DeclRefVisitor>
{
public:
	DeclRefVisitor(EncodingTransformationContext &context, Rewriter &rewriter, ExprBuilder &exprBuilder, bool inExpr) :
            theContext(context), theRewriter(rewriter), theBuilder(exprBuilder), inExpression(inExpr)
		{
			foundDeclRef = false;
		}

	bool foundDeclReference() {
		return foundDeclRef;
	}

	vector<DeclRefExpr*> getVarDeclList() {
		return varDeclList;
	}

	void clear() {
		varDeclList.clear();
		foundDeclRef = false;
	}

	bool VisitDeclRefExpr(DeclRefExpr *declRef) {
		VarDecl *VD = dyn_cast_or_null<VarDecl>(declRef->getDecl());

		if (VD && theContext.search(VD) && !theBuilder.findDeclRef(declRef)) {
			// An encoded variable in use, DECODE
			string expr;
			SourceLocation startLoc = declRef->getLocStart();
			SourceRange range = declRef->getSourceRange();

			expr = "(" + VD->getType().getAsString() + ")Encoded::DECODE(";
			expr += VD->getNameAsString();
			expr += ")";

			foundDeclRef = true;

			theRewriter.RemoveText(range);
			if (inExpression) {
				varDeclList.push_back(declRef);
//				theBuilder.add(expr);
			}
			else {
				theRewriter.InsertText(startLoc, expr, true, true);
			}

			theBuilder.addDeclRef(declRef);
		}
	    return true;
	}

private:
	EncodingTransformationContext &theContext;
	Rewriter &theRewriter;
	ExprBuilder &theBuilder;
	bool inExpression, foundDeclRef;
	vector<DeclRefExpr*> varDeclList;
};

class ConditionalOPVisitor: public RecursiveASTVisitor<ConditionalOPVisitor>
{
public:
	ConditionalOPVisitor(Rewriter &rewriter, EncodingTransformationContext &context, SourceManager &sourceMgr, ExprBuilder &expressionBuidler) :
        theRewriter(rewriter), theContext(context), theSourceMgr(sourceMgr), exprBuilder(expressionBuidler)
		{
			foundCOP = false;
		}


	// Replace all occurrences of 'find' with 'replace' in inStr
	string replaceAll(string inStr, string find,string replace)
	{
		string alphanum("_0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");

		size_t found=0, found2=0;

		found = inStr.find(find);
		while (1) {
		  if (found != string::npos) {
			  found--;
			  if (found != string::npos) {

				  found2 = alphanum.find(inStr.at(found));

				  //
				  if (found2 != string::npos) {
					  found = found + find.length() + 1; // skip
				  }
				  else {
					  // TODO: check also the 'after' part
					  // e.g. day_at_mon1(__mon1__) find 'mon1'
					  inStr.replace(found+1, find.length(), replace);
					  found = found + replace.length() + 1;
				  }
			  }
			  else {
				  inStr.replace(found+1, find.length(), replace);
				  found = found + replace.length() + 1;

			  }
		  }
		  else {
			  break;
		  }
		  found = inStr.find(find, found);
		}
		return inStr;
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

    bool foundCOperator() {
    	return foundCOP;
    }

	bool VisitAbstractConditionalOperator(AbstractConditionalOperator *condOP) {

		Expr *cond = condOP->getCond();
		Expr *truePart = condOP->getTrueExpr();
		Expr *falsePart = condOP->getFalseExpr();

		foundCOP = true;

		string expr;
		DeclRefVisitor declRefVistor(theContext, theRewriter, exprBuilder, true);

		declRefVistor.TraverseStmt(cond);
		if (!declRefVistor.foundDeclReference()) {
			expr = getExprAsString(cond);
		} else {
			expr = getExprAsString(cond);

			vector<DeclRefExpr*> verDecl = declRefVistor.getVarDeclList();

			expr = getExprAsString(cond);
			for (vector<DeclRefExpr*>::iterator I=verDecl.begin(); I != verDecl.end(); ++I) {
				VarDecl *VD = dyn_cast_or_null<VarDecl>((*I)->getDecl());
				expr = replaceAll(expr, VD->getNameAsString(), "(" + VD->getType().getAsString() + ")Encoded::DECODE(" + VD->getNameAsString() + ")");
				exprBuilder.addDeclRef(*I);
			}
		}
		exprBuilder.add(expr);

		exprBuilder.add(" ? ");

		declRefVistor.clear();

		// TRUE PART
		declRefVistor.TraverseStmt(truePart);
		if (!declRefVistor.foundDeclReference()) {
			expr = getExprAsString(truePart);
		} else {
			expr = getExprAsString(truePart);

			vector<DeclRefExpr*> verDecl = declRefVistor.getVarDeclList();

			expr = getExprAsString(truePart);
			for (vector<DeclRefExpr*>::iterator I=verDecl.begin(); I != verDecl.end(); ++I) {
				VarDecl *VD = dyn_cast_or_null<VarDecl>((*I)->getDecl());
				expr = replaceAll(expr, VD->getNameAsString(), "(" + VD->getType().getAsString() + ")Encoded::DECODE(" + VD->getNameAsString() + ")");
				exprBuilder.addDeclRef(*I);
			}
		}

		exprBuilder.add(expr);

		exprBuilder.add(" : ");

		declRefVistor.clear();

		// FALSE PART
		declRefVistor.TraverseStmt(falsePart);
		if (!declRefVistor.foundDeclReference()) {
			expr = getExprAsString(falsePart);
		} else {
			expr = getExprAsString(falsePart);

			vector<DeclRefExpr*> verDecl = declRefVistor.getVarDeclList();

			expr = getExprAsString(falsePart);
			for (vector<DeclRefExpr*>::iterator I=verDecl.begin(); I != verDecl.end(); ++I) {
				VarDecl *VD = dyn_cast_or_null<VarDecl>((*I)->getDecl());
				expr = replaceAll(expr, VD->getNameAsString(), "(" + VD->getType().getAsString() + ")Encoded::DECODE(" + VD->getNameAsString() + ")");
				exprBuilder.addDeclRef(*I);
			}
		}

		return false;
	}

	Rewriter &theRewriter;
	EncodingTransformationContext &theContext;
	SourceManager &theSourceMgr;
	ExprBuilder &exprBuilder;
	bool foundCOP;
};

class DecoderVisitor: public RecursiveASTVisitor<DecoderVisitor>
{
public:
	DecoderVisitor(Rewriter &rewriter, EncodingTransformationContext &context, SourceManager &sourceMgr, ExprBuilder &expressionBuidler) :
        theRewriter(rewriter), theContext(context), theSourceMgr(sourceMgr), exprBuilder(expressionBuidler)
		{
			foundBO = false; // a check if we handled, otherwise it was not supported
		}

	unsigned getLineNo(Expr *expr)
	{
		SourceLocation StmtStartLoc = expr->getLocStart();
		FileID FID;


		pair<FileID,unsigned> V = theSourceMgr.getDecomposedLoc(StmtStartLoc);
		FID = V.first;

		unsigned lineNo = theSourceMgr.getLineNumber(FID,  V.second);

		return lineNo;
	}

	// Replace all occurrences of 'find' with 'replace' in inStr
	string replaceAll(string inStr, string find,string replace)
	{
		string alphanum("_0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");

		size_t found=0, found2=0;

		found = inStr.find(find);
		while (1) {
		  if (found != string::npos) {
			  found--;
			  if (found != string::npos) {

				  found2 = alphanum.find(inStr.at(found));

				  //
				  if (found2 != string::npos) {
					  found = found + find.length() + 1; // skip
				  }
				  else {
					  // TODO: check also the 'after' part
					  // e.g. day_at_mon1(__mon1__) find 'mon1'
					  inStr.replace(found+1, find.length(), replace);
					  found = found + replace.length() + 1;
				  }
			  }
			  else {
				  inStr.replace(found+1, find.length(), replace);
				  found = found + replace.length() + 1;

			  }
		  }
		  else {
			  break;
		  }
		  found = inStr.find(find, found);
		}
		return inStr;
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

	bool VisitDeclRefExpr(DeclRefExpr *declRef) {
		VarDecl *VD = cast<VarDecl>(declRef->getDecl());
		if (theContext.search(VD)) {
			// nothing to be done, handle RHS
		} else {
			//
		}

        return true;
    }

	bool foundBOperator() {
		return foundBO;
	}

	bool VisitBinaryOperator(BinaryOperator *bOP) {

    	Expr *RHSExpr, *LHSExpr;
    	string unaryOperatorLHS, unaryOperatorRHS;
    	bool isLHSUOPPostfix=false;
    	bool isRHSUOPPostfix=false;
    	bool parenLExpr = false;
    	bool parenRExpr = false;

    	if (bOP->isAdditiveOp() || bOP->getOpcode() == BO_Mul) {
			// We are in the decoded domain
			foundBO = true;

			ParenExpr *pExpr = dyn_cast_or_null<ParenExpr>(bOP->getRHS());
			if (pExpr) {
				parenRExpr = true;
			}

			pExpr = dyn_cast_or_null<ParenExpr>(bOP->getLHS());
			if (pExpr) {
				parenLExpr = true;
			}

			// uOP in RHS: a + (-b)
			if (isa<UnaryOperator>(bOP->getRHS()->IgnoreParenImpCasts())) {
				UnaryOperator *uOP = dyn_cast_or_null<UnaryOperator>(bOP->getRHS()->IgnoreParenImpCasts());
				unaryOperatorRHS = uOP->getOpcodeStr(uOP->getOpcode()).str();
				RHSExpr = uOP->getSubExpr();
				isRHSUOPPostfix = uOP->isPostfix(); // otherwise it is a prefix

				while (isa<UnaryOperator>(RHSExpr)) {
					uOP = dyn_cast_or_null<UnaryOperator>(RHSExpr->IgnoreParenImpCasts());
					RHSExpr = uOP->getSubExpr();
					unaryOperatorRHS += " " + uOP->getOpcodeStr(uOP->getOpcode()).str();
				}
			}
			else {
				RHSExpr = bOP->getRHS();
				unaryOperatorRHS = "";
			}

			// uOP in LHS: (-a) + b
			if (isa<UnaryOperator>(bOP->getLHS()->IgnoreParenImpCasts())) {
				UnaryOperator *uOP = dyn_cast_or_null<UnaryOperator>(bOP->getLHS()->IgnoreParenImpCasts());
				unaryOperatorLHS = uOP->getOpcodeStr(uOP->getOpcode()).str();
				LHSExpr = uOP->getSubExpr();
				isLHSUOPPostfix = uOP->isPostfix();

				while (isa<UnaryOperator>(LHSExpr)) {
					uOP = dyn_cast_or_null<UnaryOperator>(LHSExpr->IgnoreParenImpCasts());
					LHSExpr = uOP->getSubExpr();
					unaryOperatorLHS += " " + uOP->getOpcodeStr(uOP->getOpcode()).str();
				}
			}
			else {
				LHSExpr = bOP->getLHS();
				unaryOperatorLHS = "";
			}

			// LHS
			if (isa<DeclRefExpr>(LHSExpr->IgnoreParenImpCasts())) {
				DeclRefExpr *DR = cast<DeclRefExpr>(LHSExpr->IgnoreParenImpCasts());
				VarDecl *VD = cast<VarDecl>(DR->getDecl());

				// If the variable IS in the encoded domain
				if (theContext.search(VD)) {
					if (!exprBuilder.findDeclRef(DR)) {

						string strChange;

						strChange = "(" + VD->getType().getAsString() + ")Encoded::DECODE(";
						if (isLHSUOPPostfix) {
							strChange += VD->getDeclName().getAsString() + unaryOperatorLHS;
						}
						else {
							strChange += unaryOperatorLHS + VD->getDeclName().getAsString();
						}
						strChange += ")";

						if (parenLExpr) {
							strChange = "(" + strChange + ")";
						}

						exprBuilder.add(strChange + " " + bOP->getOpcodeStr().str() + " ");
						exprBuilder.addDeclRef(DR);
					}
				}
				else
				{
					// If the variable isn't in the encoded domain, leave it
					if (!exprBuilder.findDeclRef(DR)) {
						string strChange;

						if (isLHSUOPPostfix) {
							strChange += VD->getDeclName().getAsString() + unaryOperatorLHS;
						}
						else {
							strChange += unaryOperatorLHS + VD->getDeclName().getAsString();
						}

						if (parenLExpr) {
							strChange = "(" + strChange + ")";
						}

						exprBuilder.add(strChange + " " + bOP->getOpcodeStr().str() + " ");
						exprBuilder.addDeclRef(DR);
					}
				}
			}
			else if (isa<IntegerLiteral>(LHSExpr->IgnoreParenImpCasts())) {
				IntegerLiteral *IL = cast<IntegerLiteral>(LHSExpr->IgnoreParenImpCasts());

				// If it is plain integer, nothing to do
				if (!exprBuilder.findIntLiteral(IL)) {
					string strChange;
					char buf[128] = {0};

					sprintf(buf, "%ld",  IL->getValue().getSExtValue());

					if (isLHSUOPPostfix) {
						 strChange = buf + unaryOperatorLHS;
					}
					else {
						strChange = unaryOperatorLHS + buf;
					}

					//if (unaryOperatorLHS.length() >= 2) {
					//	llvm::outs() << "error: there is an un-assignable expression at line " << getLineNo(LHSExpr) << "[" + strChange + "]\n";
					//}

					if (parenLExpr) {
						strChange = "(" + strChange + ")";
					}

					exprBuilder.add(strChange + " " + bOP->getOpcodeStr().str() + " ");
					exprBuilder.addIntLiteral(IL);
				}
			}
			else {
				DecoderVisitor lhsEncoder(theRewriter, theContext, theSourceMgr, exprBuilder);
				if (parenLExpr) {
					exprBuilder.add("(");
				}
				lhsEncoder.TraverseStmt(LHSExpr);

	//			if (!lhsEncoder.foundBOperator()) {
	//				string expr;
	//
	//				expr = getExprAsString(LHSExpr);
	//
	//				// FIXME: does this include the BOP?
	//				exprBuilder.add(expr);
	//			}
				if (!lhsEncoder.foundBOperator()) {

					DeclRefVisitor declRefVistor(theContext, theRewriter, exprBuilder, true);
					string expr;

					if (!isLHSUOPPostfix) {
						exprBuilder.add(unaryOperatorLHS);
					}

					declRefVistor.TraverseStmt(LHSExpr);

					if (!declRefVistor.foundDeclReference()) {
						expr = lhsEncoder.getExprAsString(LHSExpr);
					}
					else {
						ConditionalOPVisitor condOPVisitor(theRewriter, theContext, theSourceMgr, exprBuilder);

						condOPVisitor.TraverseStmt(LHSExpr);

						if (!condOPVisitor.foundCOperator()) {
							expr = lhsEncoder.getExprAsString(LHSExpr);

							vector<DeclRefExpr*> verDecl = declRefVistor.getVarDeclList();

							expr = lhsEncoder.getExprAsString(LHSExpr);
							for (vector<DeclRefExpr*>::iterator I=verDecl.begin(); I != verDecl.end(); ++I) {
								VarDecl *VD = dyn_cast_or_null<VarDecl>((*I)->getDecl());
								expr = lhsEncoder.replaceAll(expr, VD->getNameAsString(), "(" + VD->getType().getAsString() + ")Encoded::DECODE(" + VD->getNameAsString() + ")");
							}
						}
					}

					if (isLHSUOPPostfix) {
						exprBuilder.add(unaryOperatorLHS);
					}

					exprBuilder.add(expr);
				}

				if (parenLExpr) {
					exprBuilder.add(")");
				}

				exprBuilder.add(" " + bOP->getOpcodeStr().str() + " ");
			}

			// RHS
			if (isa<DeclRefExpr>(RHSExpr->IgnoreParenImpCasts())) {
				DeclRefExpr *DR = cast<DeclRefExpr>(RHSExpr->IgnoreParenImpCasts());
				VarDecl *VD = cast<VarDecl>(DR->getDecl());
				NamedDecl *ND = DR->getFoundDecl();

				// If the variable IS in the encoded domain
				if (theContext.search(VD)) {
					if (!exprBuilder.findDeclRef(DR)) {
						string strChange;

						strChange = "(" + VD->getType().getAsString() + ")Encoded::DECODE(";
						if (isRHSUOPPostfix) {
							strChange += ND->getNameAsString() + unaryOperatorRHS;
						}
						else {
							strChange += unaryOperatorRHS + ND->getNameAsString();
						}
						strChange += ")";

						if (parenRExpr) {
							strChange = "(" + strChange + ")";
						}

						exprBuilder.add(strChange);
						exprBuilder.addDeclRef(DR);
					}
				}
				else
				{
					if (!exprBuilder.findDeclRef(DR)) {
						string strChange;

						if (isRHSUOPPostfix) {
							strChange = VD->getDeclName().getAsString() + unaryOperatorRHS;
						}
						else {
							strChange = unaryOperatorRHS + VD->getDeclName().getAsString();
						}

						if (parenRExpr) {
							strChange = "(" + strChange + ")";
						}

						exprBuilder.add(strChange);
						exprBuilder.addDeclRef(DR);
					}
				}
			}
			else if (isa<IntegerLiteral>(RHSExpr->IgnoreParenImpCasts())) {
				IntegerLiteral *IL = cast<IntegerLiteral>(RHSExpr->IgnoreParenImpCasts());

				if (!exprBuilder.findIntLiteral(IL)) {
					string strChange;
					char buf[128] = {0};

					sprintf(buf, "%ld", IL->getValue().getSExtValue());

					if (isRHSUOPPostfix) {
						strChange = buf + unaryOperatorRHS;
					}
					else {
						strChange = unaryOperatorRHS + buf;
					}

					//if (unaryOperatorRHS.length() >= 2) {
					//	llvm::outs() << "error: there is an un-assignable expression at line " << getLineNo(LHSExpr) << "[" + strChange + "]\n";
					//}

					if (parenRExpr) {
						strChange = "(" + strChange + ")";
					}

					exprBuilder.add(strChange);
					exprBuilder.addIntLiteral(IL);
				}
			}
			else {
				DecoderVisitor rhsDecoder(theRewriter, theContext, theSourceMgr, exprBuilder);
				if (parenRExpr) {
					exprBuilder.add("(");
				}

				rhsDecoder.TraverseStmt(RHSExpr);

				// This is to check if the traverse was successful.
				// That means check if we found a BO on the RHS Expr.
				// If we didn't find any BO, then the Expr is not supported (at least for now)
	//			if (!rhsDecoder.foundBOperator()) {
	//				string expr;
	//
	//				expr = getExprAsString(RHSExpr);
	//
	//				// This will also include the BOP,
	//				// so do not explicitly add a BOP
	//				exprBuilder.add(expr);
	//			}
				if (!rhsDecoder.foundBOperator()) {

					DeclRefVisitor declRefVistor(theContext, theRewriter, exprBuilder, true);
					string expr;

					declRefVistor.TraverseStmt(RHSExpr);

					if (!declRefVistor.foundDeclReference()) {
						expr = rhsDecoder.getExprAsString(RHSExpr);
						exprBuilder.add(expr);
					}
					else {
						ConditionalOPVisitor condOPVisitor(theRewriter, theContext, theSourceMgr, exprBuilder);

						condOPVisitor.TraverseStmt(RHSExpr);

						if (!condOPVisitor.foundCOperator()) {
							vector<DeclRefExpr*> verDecl = declRefVistor.getVarDeclList();

							expr = rhsDecoder.getExprAsString(RHSExpr);
							for (vector<DeclRefExpr*>::iterator I=verDecl.begin(); I != verDecl.end(); ++I) {
								VarDecl *VD = dyn_cast_or_null<VarDecl>((*I)->getDecl());
								expr = rhsDecoder.replaceAll(expr, VD->getNameAsString(), "(" + VD->getType().getAsString() + ")Encoded::DECODE(" + VD->getNameAsString() + ")");
								exprBuilder.add(expr);
							}
						}
					}


				}

				if (parenRExpr) {
					exprBuilder.add(")");
				}
			}

		}
		// We're done, stop.
		return false;
	}

private:
    Rewriter &theRewriter;
    EncodingTransformationContext &theContext;
    SourceManager &theSourceMgr;
    ExprBuilder &exprBuilder;
    bool foundBO;
};

class EncoderVisitor: public RecursiveASTVisitor<EncoderVisitor>
{
public:
	EncoderVisitor(Rewriter &rewriter, EncodingTransformationContext &context, SourceManager &sourceMgr, ExprBuilder &expressionBuidler) :
            theRewriter(rewriter), theContext(context), theSourceMgr(sourceMgr), exprBuilder(expressionBuidler)
		{
			foundBO = false;
		}

	// Replace all occurrences of 'find' with 'replace' in inStr
	string replaceAll(string inStr, string find,string replace)
	{
		string alphanum("_0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");

		size_t found=0, found2=0;

		found = inStr.find(find);
		while (1) {
		  if (found != string::npos) {
			  found--;
			  if (found != string::npos) {

				  found2 = alphanum.find(inStr.at(found));

				  //
				  if (found2 != string::npos) {
					  found = found + find.length() + 1; // skip
				  }
				  else {
					  // TODO: check also the 'after' part
					  // e.g. day_at_mon1(__mon1__) find 'mon1'
					  inStr.replace(found+1, find.length(), replace);
					  found = found + replace.length() + 1;
				  }
			  }
			  else {
				  inStr.replace(found+1, find.length(), replace);
				  found = found + replace.length() + 1;

			  }
		  }
		  else {
			  break;
		  }
		  found = inStr.find(find, found);
		}
		return inStr;
	}


	unsigned getLineNo(Expr *expr)
	{
		SourceLocation StmtStartLoc = expr->getLocStart();
		FileID FID;


		pair<FileID,unsigned> V = theSourceMgr.getDecomposedLoc(StmtStartLoc);
		FID = V.first;

		unsigned lineNo = theSourceMgr.getLineNumber(FID,  V.second);

		return lineNo;
	}

    string getExprAsString(Expr *expression)
    {
    	LangOptions lopt;
    	SourceLocation begin(expression->getLocStart()), endToken(expression->getLocEnd());
    	SourceLocation end(clang::Lexer::getLocForEndOfToken(endToken, 0, theSourceMgr, lopt));

    	return string(theSourceMgr.getCharacterData(begin),	theSourceMgr.getCharacterData(end)-theSourceMgr.getCharacterData(begin));

//    	SourceRange exprRange = expression->getSourceRange();
//    	string exprString;
//
//    	int rangeSize = theRewriter.getRangeSize(exprRange);
//    	if (rangeSize == -1)
//    		return "";
//
//    	SourceLocation startLoc = exprRange.getBegin();
//    	const char *strStart = theSourceMgr.getCharacterData(startLoc);
//
//    	exprString.assign(strStart, rangeSize);
//
//    	return exprString;
    }

    bool foundBOperator() {
    	return foundBO;
    }

    bool VisitBinaryOperator(BinaryOperator *bOP) {

    	Expr *RHSExpr, *LHSExpr;
    	string unaryOperatorLHS, unaryOperatorRHS;
    	bool isLHSUOPPostfix=false;
    	bool isRHSUOPPostfix=false;
    	bool parenRExpr = false;
    	bool parenLExpr = false;

		ParenExpr *pExpr = dyn_cast_or_null<ParenExpr>(bOP->getRHS());
		if (pExpr) {
			parenRExpr = true;
		}

		pExpr = dyn_cast_or_null<ParenExpr>(bOP->getLHS());
		if (pExpr) {
			parenLExpr = true;
		}

    	// uOP in RHS: a + (-b)
		if (isa<UnaryOperator>(bOP->getRHS()->IgnoreParenImpCasts())) {
			UnaryOperator *uOP = dyn_cast_or_null<UnaryOperator>(bOP->getRHS()->IgnoreParenImpCasts());
			unaryOperatorRHS = uOP->getOpcodeStr(uOP->getOpcode()).str();
			RHSExpr = uOP->getSubExpr();
			isRHSUOPPostfix = uOP->isPostfix(); // otherwise it is a prefix

			while (isa<UnaryOperator>(RHSExpr)) {
				uOP = dyn_cast_or_null<UnaryOperator>(RHSExpr->IgnoreParenImpCasts());
				RHSExpr = uOP->getSubExpr();
				unaryOperatorRHS += " " + uOP->getOpcodeStr(uOP->getOpcode()).str();
			}
		}
		else {
			RHSExpr = bOP->getRHS();
			unaryOperatorRHS = "";
		}

		// uOP in LHS: (-a) + b
		if (isa<UnaryOperator>(bOP->getLHS()->IgnoreParenImpCasts())) {
			UnaryOperator *uOP = dyn_cast_or_null<UnaryOperator>(bOP->getLHS()->IgnoreParenImpCasts());
			unaryOperatorLHS = uOP->getOpcodeStr(uOP->getOpcode()).str();
			LHSExpr = uOP->getSubExpr();
			isLHSUOPPostfix = uOP->isPostfix();

			while (isa<UnaryOperator>(LHSExpr)) {
				uOP = dyn_cast_or_null<UnaryOperator>(LHSExpr->IgnoreParenImpCasts());
				LHSExpr = uOP->getSubExpr();
				unaryOperatorLHS += " " + uOP->getOpcodeStr(uOP->getOpcode()).str();
			}
		}
		else {
			LHSExpr = bOP->getLHS();
			unaryOperatorLHS = "";
		}

		if (bOP->isAdditiveOp() || bOP->getOpcode() == BO_Mul) {
	    	foundBO = true;
			// LHS
			if (isa<DeclRefExpr>(LHSExpr->IgnoreParenImpCasts())) {
				DeclRefExpr *DR = dyn_cast_or_null<DeclRefExpr>(LHSExpr->IgnoreParenImpCasts());
				VarDecl *VD = dyn_cast_or_null<VarDecl>(DR->getDecl());

				// If the variable isn't in the encoded domain
				if (VD && !theContext.search(VD)) {
					if (DR && !exprBuilder.findDeclRef(DR)) {

						string strChange;

						strChange += "Encoded(";
						if (isLHSUOPPostfix) {
							strChange += VD->getDeclName().getAsString() + unaryOperatorLHS;
						}
						// Also includes empty string for unaryOperatorLHS
						// i.e. when there is no unary operator at all == ""
						else {
							strChange += unaryOperatorLHS + VD->getDeclName().getAsString();
						}
						strChange += ")";

						if (parenLExpr) {
							strChange = "(" + strChange + ")";
						}

						exprBuilder.add(strChange + " " + bOP->getOpcodeStr().str() + " ");
						exprBuilder.addDeclRef(DR);
					}
				}
				else
				{
					if (DR && !exprBuilder.findDeclRef(DR)) {
						string strChange;

						if (isLHSUOPPostfix) {
							strChange += VD->getDeclName().getAsString() + unaryOperatorLHS;
						}
						else {
							strChange += unaryOperatorLHS + VD->getDeclName().getAsString();
						}

						if (parenLExpr) {
							strChange = "(" + strChange + ")";
						}

						exprBuilder.add(strChange + " " + bOP->getOpcodeStr().str() + " ");
						exprBuilder.addDeclRef(DR);
					}
				}
			}
			else if (isa<IntegerLiteral>(LHSExpr->IgnoreParenImpCasts())) {
				IntegerLiteral *IL = dyn_cast_or_null<IntegerLiteral>(LHSExpr->IgnoreParenImpCasts());

				if (IL && !exprBuilder.findIntLiteral(IL)) {
					string strChange;
					char buf[128] = {0};

					strChange += "Encoded(";
					sprintf(buf, "%ld",  IL->getValue().getSExtValue());

					strChange += unaryOperatorLHS + buf;

					///if (unaryOperatorLHS.length() >= 2) {
					//	llvm::outs() << "error: there is an un-assignable expression at line " << getLineNo(LHSExpr) << "[" + unaryOperatorLHS + buf + "]\n";
					//}

					strChange += ")";

					if (parenLExpr) {
						strChange = "(" + strChange + ")";
					}

					exprBuilder.add(strChange + " " + bOP->getOpcodeStr().str() + " ");
					exprBuilder.addIntLiteral(IL);
				}
			}
			else {
				EncoderVisitor lhsEncoder(theRewriter, theContext, theSourceMgr, exprBuilder);

				if (parenLExpr) {
					exprBuilder.add("(");
				}

				lhsEncoder.TraverseStmt(LHSExpr);

//				if (!lhsEncoder.foundBOperator()) {
//					DeclRefVisitor declRefVistor(theContext, theRewriter, exprBuilder, true);
//					string expr;
//
//
//					exprBuilder.add("Encoded(");
//
//					if (!isLHSUOPPostfix) {
//						exprBuilder.add(unaryOperatorLHS);
//					}
//
//					declRefVistor.TraverseStmt(LHSExpr);
//
//					if (!declRefVistor.foundDeclReference()) {
//						expr = getExprAsString(LHSExpr);
//						exprBuilder.add(expr);
//					}
//
//					if (isLHSUOPPostfix) {
//						exprBuilder.add(unaryOperatorLHS);
//					}
//
//					expr = ")";
//					exprBuilder.add(expr);
//				}
				if (!lhsEncoder.foundBOperator()) {

					DeclRefVisitor declRefVistor(theContext, theRewriter, exprBuilder, true);
					string expr;

					exprBuilder.add("Encoded(");

					if (!isLHSUOPPostfix) {
						exprBuilder.add(unaryOperatorLHS);
					}

					declRefVistor.TraverseStmt(LHSExpr);

					if (!declRefVistor.foundDeclReference()) {
						expr = lhsEncoder.getExprAsString(LHSExpr);
						exprBuilder.add(expr);
					} else {
						vector<DeclRefExpr*> verDecl = declRefVistor.getVarDeclList();

						expr = lhsEncoder.getExprAsString(LHSExpr);
						for (vector<DeclRefExpr*>::iterator I=verDecl.begin(); I != verDecl.end(); ++I) {
							VarDecl *VD = dyn_cast_or_null<VarDecl>((*I)->getDecl());
							expr = lhsEncoder.replaceAll(expr, VD->getNameAsString(), "(" + VD->getType().getAsString() + ")Encoded::DECODE(" + VD->getNameAsString() + ")");
						}
						exprBuilder.add(expr);
					}

					if (isLHSUOPPostfix) {
						exprBuilder.add(unaryOperatorLHS);
					}

					expr = ")";
					exprBuilder.add(expr);
				}

				if (parenLExpr) {
					exprBuilder.add(")");
				}

				exprBuilder.add(" " + bOP->getOpcodeStr().str() + " ");

			}

			// RHS
			if (isa<DeclRefExpr>(RHSExpr->IgnoreParenImpCasts())) {
				DeclRefExpr *DR = dyn_cast_or_null<DeclRefExpr>(RHSExpr->IgnoreParenImpCasts());
				VarDecl *VD = dyn_cast_or_null<VarDecl>(DR->getDecl());
				NamedDecl *ND = DR->getFoundDecl();

				// If the variable isn't in the encoded domain
				if (VD && !theContext.search(VD)) {
					if (DR && !exprBuilder.findDeclRef(DR)) {
						string strChange;

						strChange += "Encoded(";
						if (isRHSUOPPostfix) {
							strChange += ND->getNameAsString() + unaryOperatorRHS;
						}
						else {
							strChange += unaryOperatorRHS + ND->getNameAsString();
						}
						strChange += ")";

						if (parenRExpr) {
							strChange = "(" + strChange + ")";
						}

						exprBuilder.add(strChange);
						exprBuilder.addDeclRef(DR);
					}
				}
				else
				{
					if (DR && !exprBuilder.findDeclRef(DR)) {
						string strChange;

						if (isRHSUOPPostfix) {
							strChange = VD->getDeclName().getAsString() + unaryOperatorRHS;
						}
						else {
							strChange = unaryOperatorRHS + VD->getDeclName().getAsString();
						}

						if (parenRExpr) {
							strChange = "(" + strChange + ")";
						}

						exprBuilder.add(strChange);
						exprBuilder.addDeclRef(DR);
					}
				}
			}
			else if (isa<IntegerLiteral>(RHSExpr->IgnoreParenImpCasts())) {
				IntegerLiteral *IL = dyn_cast_or_null<IntegerLiteral>(RHSExpr->IgnoreParenImpCasts());

				if (IL && !exprBuilder.findIntLiteral(IL)) {
					string strChange;
					char buf[128] = {0};

					strChange += "Encoded(";
					sprintf(buf, "%ld",  IL->getValue().getSExtValue());

					//if (unaryOperatorRHS.length() >= 2) {
					//	llvm::outs() << "error: there is an un-assignable expression at line " << getLineNo(LHSExpr) << "[" + unaryOperatorRHS + buf + "]\n";
					//}

					strChange += unaryOperatorRHS + buf;
					strChange += ")";

					if (parenRExpr) {
						strChange = "(" + strChange + ")";
					}

					exprBuilder.add(strChange);
					exprBuilder.addIntLiteral(IL);
				}
			}
			else {
				EncoderVisitor rhsEncoder(theRewriter, theContext, theSourceMgr, exprBuilder);

				if (parenRExpr) {
					exprBuilder.add("(");
				}

				rhsEncoder.TraverseStmt(RHSExpr);

				if (!rhsEncoder.foundBOperator()) {

					DeclRefVisitor declRefVistor(theContext, theRewriter, exprBuilder, true);
					string expr;

					exprBuilder.add("Encoded(");

					if (!isRHSUOPPostfix) {
						exprBuilder.add(unaryOperatorRHS);
					}

					declRefVistor.TraverseStmt(RHSExpr);

					if (!declRefVistor.foundDeclReference()) {
						expr = rhsEncoder.getExprAsString(RHSExpr);
						exprBuilder.add(expr);
					} else {
						vector<DeclRefExpr*> verDecl = declRefVistor.getVarDeclList();

						expr = rhsEncoder.getExprAsString(RHSExpr);
						for (vector<DeclRefExpr*>::iterator I=verDecl.begin(); I != verDecl.end(); ++I) {
							VarDecl *VD = dyn_cast_or_null<VarDecl>((*I)->getDecl());
							expr = rhsEncoder.replaceAll(expr, VD->getNameAsString(), "(" + VD->getType().getAsString() + ")Encoded::DECODE(" + VD->getNameAsString() + ")");
						}
						exprBuilder.add(expr);
					}

					if (isRHSUOPPostfix) {
						exprBuilder.add(unaryOperatorRHS);
					}

					expr = ")";
					exprBuilder.add(expr);
				}

				if (parenRExpr) {
					exprBuilder.add(")");
				}
			}
		}
		else {
			// FIXME: Not supported binary operator
//			DecoderVisitor bopDecoder(theRewriter, theContext, theSourceMgr, exprBuilder);
//			exprBuilder.add("ppppEncoded(");
//			bopDecoder.TraverseStmt(bOP);
//			exprBuilder.add(")");
		}
		// We're done, stop.
		return false;
	}

private:
    Rewriter &theRewriter;
    EncodingTransformationContext &theContext;
    SourceManager &theSourceMgr;
    ExprBuilder &exprBuilder;
    bool foundBO;
};

class EncodedDomainVisitor: public RecursiveASTVisitor<EncodedDomainVisitor>
{
public:
	EncodedDomainVisitor(EncodingTransformationContext &context, ExprBuilder &builder) :
            theContext(context), theBuilder(builder)
		{
			containsEncoded = false;
		}

	bool isToEncode() {
		return containsEncoded;
	}

	void reset() {
		containsEncoded = false;
	}

	bool VisitDeclRefExpr(DeclRefExpr *DR) {

		VarDecl *VD = dyn_cast_or_null<VarDecl>(DR->getDecl());

		theBuilder.addDeclRef(DR);
		if (VD && theContext.search(VD)) {
			containsEncoded = true;

			// We found one so we're done
			return false;
		}

		return true;
	}

private:
	EncodingTransformationContext theContext;
	ExprBuilder &theBuilder;
	bool containsEncoded;
};

class TransformerASTVisitor : public RecursiveASTVisitor<TransformerASTVisitor >
{
public:
	TransformerASTVisitor (Rewriter &rewriter, EncodingTransformationContext &context, SourceManager &sourceMgr, ExprBuilder &builder) :
            theRewriter(rewriter), theContext(context), theSourceMgr(sourceMgr), theBuilder(builder)
		{ }
	unsigned getLineNo(Expr *expr)
	{
		SourceLocation stmtStartLoc = expr->getLocStart();
		FileID FID;


		pair<FileID,unsigned> V = theSourceMgr.getDecomposedLoc(stmtStartLoc);
		FID = V.first;

		unsigned lineNo = theSourceMgr.getLineNumber(FID,  V.second);

		return lineNo;
	}

    bool VisitBinaryOperator(BinaryOperator *binaryOP) {

    	if (binaryOP->isAssignmentOp()) {
    		EncoderVisitor encoderRHSEncoder(theRewriter, theContext, theSourceMgr, theBuilder);
    		DecoderVisitor decoderRHSEncoder(theRewriter, theContext, theSourceMgr, theBuilder);
    		SourceLocation startLoc = binaryOP->getRHS()->getLocStart();
    		SourceRange range = binaryOP->getRHS()->getSourceRange();
    		string unaryOperator;
    		Expr *RHSExpr;
    		bool isUOPPostfix=false;
    		bool parenExpr=false;

    		EncodedDomainVisitor encodedDomainVisitor(theContext, theBuilder);

    		// Check the LHS
    		encodedDomainVisitor.TraverseStmt(binaryOP->getLHS());

    		ParenExpr *pExpr = dyn_cast_or_null<ParenExpr>(binaryOP->getRHS());
			if (pExpr) {
				parenExpr = true;
			}

			// e.g. x = (-y);
			if (isa<UnaryOperator>(binaryOP->getRHS()->IgnoreParenImpCasts())) {
				UnaryOperator *uOP = dyn_cast_or_null<UnaryOperator>(binaryOP->getRHS()->IgnoreParenImpCasts());
				unaryOperator = uOP->getOpcodeStr(uOP->getOpcode()).str();
				RHSExpr = uOP->getSubExpr();
				isUOPPostfix = uOP->isPostfix();

				// e.g. x = (----y);
				while (isa<UnaryOperator>(RHSExpr)) {
					uOP = dyn_cast_or_null<UnaryOperator>(RHSExpr->IgnoreParenImpCasts());
					RHSExpr = uOP->getSubExpr();
					unaryOperator += " " + uOP->getOpcodeStr(uOP->getOpcode()).str();
				}
			}
			else {
				RHSExpr = binaryOP->getRHS();
				unaryOperator = "";
			}

    		// Do we need to encode the RHS?
    		if (encodedDomainVisitor.isToEncode()) {
				if (isa<DeclRefExpr>(RHSExpr->IgnoreParenImpCasts())) {
					DeclRefExpr *DR = dyn_cast_or_null<DeclRefExpr>(RHSExpr->IgnoreParenImpCasts());
					VarDecl *VD = dyn_cast_or_null<VarDecl>(DR->getDecl());
					NamedDecl *ND = DR->getFoundDecl();

					// If the variable isn't in the encoded domain
					if (VD && !theContext.search(VD)) {
						if (DR && !theBuilder.findDeclRef(DR)) {
							string strChange;

							strChange += "Encoded(";
							if (isUOPPostfix) {
								strChange += ND->getNameAsString() + unaryOperator;
							}
							else {
								strChange += unaryOperator + ND->getNameAsString();
							}
							strChange += ")";

							if (parenExpr) {
								strChange = "(" + strChange + ")";
							}

							theBuilder.add(strChange);
							theBuilder.addDeclRef(DR);
						}
					}
					else
					{
						if (DR && !theBuilder.findDeclRef(DR)) {
							string strChange;

							if (isUOPPostfix) {
								strChange += VD->getDeclName().getAsString() + unaryOperator;
							}
							else {
								strChange += unaryOperator + VD->getDeclName().getAsString();
							}

							if (parenExpr) {
								strChange = "(" + strChange + ")";
							}

							theBuilder.add(strChange);
							theBuilder.addDeclRef(DR);
						}
					}
				}
				else if (isa<IntegerLiteral>(RHSExpr->IgnoreParenImpCasts())) {
					IntegerLiteral *IL = dyn_cast_or_null<IntegerLiteral>(RHSExpr->IgnoreParenImpCasts());

					if (IL &&!theBuilder.findIntLiteral(IL)) {
						string strChange;
						char buf[128] = {0};

						strChange += "Encoded(";
						sprintf(buf, "%ld",  IL->getValue().getSExtValue());

						//if (unaryOperator.length() >= 2) {
						//	llvm::outs() << "error: there is an un-assignable expression at line " << getLineNo(RHSExpr) << "[" + unaryOperator + buf + "]\n";
						//}

						strChange += unaryOperator + buf;
						strChange += ")";

						if (parenExpr) {
							strChange = "(" + strChange + ")";
						}

						theBuilder.add(strChange);
						theBuilder.addIntLiteral(IL);
					}
				}
				else {

					if (parenExpr) {
						theBuilder.add("(");
					}
					encoderRHSEncoder.TraverseStmt(RHSExpr);

					if (!encoderRHSEncoder.foundBOperator()) {
						DeclRefVisitor declRefVistor(theContext, theRewriter, theBuilder, true);
						string expr;

						theBuilder.add("Encoded(");

						if (!isUOPPostfix) {
							theBuilder.add(unaryOperator);
						}

						declRefVistor.TraverseStmt(RHSExpr);

						if (!declRefVistor.foundDeclReference()) {
							expr = encoderRHSEncoder.getExprAsString(RHSExpr);
							theBuilder.add(expr);
						} else {
							vector<DeclRefExpr*> verDecl = declRefVistor.getVarDeclList();

							expr = encoderRHSEncoder.getExprAsString(RHSExpr);
							for (vector<DeclRefExpr*>::iterator I=verDecl.begin(); I != verDecl.end(); ++I) {
								VarDecl *VD = dyn_cast_or_null<VarDecl>((*I)->getDecl());
								expr = encoderRHSEncoder.replaceAll(expr, VD->getNameAsString(), "(" + VD->getType().getAsString() + ")Encoded::DECODE(" + VD->getNameAsString() + ")");
							}
							theBuilder.add(expr);
						}

						if (isUOPPostfix) {
							theBuilder.add(unaryOperator);
						}

						expr = ")";
						theBuilder.add(expr);
					}

					if (parenExpr) {
						theBuilder.add(")");
					}
				}
    		}
    		else {
    			// LHS isn't in the encoded domain
    			if (isa<DeclRefExpr>(RHSExpr->IgnoreParenImpCasts())) {
					DeclRefExpr *DR = dyn_cast_or_null<DeclRefExpr>(RHSExpr->IgnoreParenImpCasts());
					VarDecl *VD = dyn_cast_or_null<VarDecl>(DR->getDecl());
					NamedDecl *ND = DR->getFoundDecl();

					// If the variable IS in the encoded domain
					if (VD && theContext.search(VD)) {
						if (DR && !theBuilder.findDeclRef(DR)) {
							string strChange;

							strChange = "(" + VD->getType().getAsString() + ")Encoded::DECODE(";
							if (isUOPPostfix) {
								strChange += ND->getNameAsString() + unaryOperator;
							}
							else {
								strChange += unaryOperator + ND->getNameAsString();
							}
							strChange += ")";

							if (parenExpr) {
								strChange = "(" + strChange + ")";
							}

							theBuilder.add(strChange);
							theBuilder.addDeclRef(DR);
						}
					}
					else
					{
						if (DR && !theBuilder.findDeclRef(DR)) {
							string strChange;

							if (isUOPPostfix) {
								strChange += VD->getDeclName().getAsString() + unaryOperator;
							}
							else {
								strChange += unaryOperator + VD->getDeclName().getAsString();

							}

							if (parenExpr) {
								strChange = "(" + strChange + ")";
							}

							theBuilder.add(strChange);
							theBuilder.addDeclRef(DR);
						}
					}
				}
				else if (isa<IntegerLiteral>(RHSExpr->IgnoreParenImpCasts())) {
					IntegerLiteral *IL = dyn_cast_or_null<IntegerLiteral>(RHSExpr->IgnoreParenImpCasts());

					if (IL && !theBuilder.findIntLiteral(IL)) {
						string strChange;
						char buf[128] = {0};

						sprintf(buf, "%ld",  IL->getValue().getSExtValue());

						//if (unaryOperator.length() >= 2) {
						//	llvm::outs() << "error: there is an un-assignable expression at line " << getLineNo(RHSExpr) << "[" + unaryOperator + buf + "]\n";
						//}

						strChange = unaryOperator + buf;

						if (parenExpr) {
							strChange = "(" + strChange + ")";
						}

						theBuilder.add(strChange);
						theBuilder.addIntLiteral(IL);
					}
				}
				else {
					if (parenExpr) {
						theBuilder.add("(");
					}

					decoderRHSEncoder.TraverseStmt(RHSExpr);

					if (!decoderRHSEncoder.foundBOperator()) {
						string expr;


						if (isUOPPostfix) {
							expr += decoderRHSEncoder.getExprAsString(RHSExpr) + unaryOperator;
						}
						else {
							expr += unaryOperator + decoderRHSEncoder.getExprAsString(RHSExpr);
						}

						theBuilder.add(expr);
					}

					if (parenExpr) {
						theBuilder.add(")");
					}
				}
    		}


			theRewriter.RemoveText(range);
			theRewriter.InsertText(startLoc, theBuilder.getExprString(), true, true);
			theBuilder.clear();
    	}

    	return true;
    }

private:
    Rewriter &theRewriter;
    EncodingTransformationContext &theContext;
    SourceManager &theSourceMgr;
    ExprBuilder &theBuilder;
};

class RuleBuilderHelper : public RecursiveASTVisitor<RuleBuilderHelper >
{
public:
	RuleBuilderHelper (EncodingTransformationContext &context, string functionName) :
		theContext(context), inFunction(functionName)
	{
		block = -1;
	}

	bool traverseDecl(DeclStmt *DS)
	{
		if (!DS->isSingleDecl()) {

			DeclGroupRef declGroupRef = DS->getDeclGroup();

			for (DeclGroupRef::iterator I=declGroupRef.begin(), E=declGroupRef.end(); I != E; ++I) {
				VarDecl *V = dyn_cast_or_null<VarDecl>(*I);

				if (V) {
					ValueDecl *valueDecl = dyn_cast_or_null<ValueDecl>(V);

					if (valueDecl) {
						string type = valueDecl->getType().getAsString();

						if (type.compare("int") == 0 || type.compare("int *") == 0 || type.compare("unsigned long") == 0) {
							char buf[2] = "";

							if (block != 0) {
							   sprintf(buf, "%d/", block);
							}

							theContext.addVariablePath(V, "FUNCTION/" + inFunction + "/" + buf + V->getNameAsString());
						}
					}
				}
			}
		}
		else {
			VarDecl *V = dyn_cast_or_null<VarDecl>(DS->getSingleDecl());

			if (V) {
				ValueDecl *valueDecl = dyn_cast_or_null<ValueDecl>(V);

				if (valueDecl) {
					string type = valueDecl->getType().getAsString();

					if (type.compare("int") == 0 || type.compare("int *") == 0 || type.compare("unsigned long") == 0) {
						char buf[2] = "";

						if (block != 0) {
							sprintf(buf, "%d/", block);
						}

						theContext.addVariablePath(V, "FUNCTION/" + inFunction + "/" + buf + V->getNameAsString());
					}
				}
			}

		}
		return true;
	}

	bool VisitCompoundStmt(CompoundStmt *stmt) {

		block++;

		for (Stmt::child_iterator I = stmt->child_begin(), E = stmt->child_end(); I != E; ++I) {
			if (Stmt *child = *I) {
				if (DeclStmt *DS = dyn_cast_or_null<DeclStmt>(child)) {
					if (DS) {
						traverseDecl(DS);
					}
				}
			}
		}
		return true;
	}

    EncodingTransformationContext &theContext;
    string inFunction;
    int block; //  number of blocks {}
};

class RuleBuilder : public RecursiveASTVisitor<RuleBuilder>
{

public:
	RuleBuilder (EncodingTransformationContext &context) :
		theContext(context)
	{}

	bool VisitFunctionDecl(FunctionDecl *function) {

		string functionName = function->getNameInfo().getAsString();

		RuleBuilderHelper ruleBuilderHelper(theContext, functionName);

		ruleBuilderHelper.TraverseStmt(function->getBody());

		return true;
	}

	EncodingTransformationContext &theContext;
};

class SkipDecoding : public RecursiveASTVisitor<SkipDecoding>
{

public:
	SkipDecoding (EncodingTransformationContext &context, ExprBuilder &exprBuilder) :
		theContext(context), theBuilder(exprBuilder)
	{}

	bool VisitUnaryOperator(UnaryOperator *uOP) {
		Expr *expr = uOP->getSubExpr();
		DeclRefExpr *declRef = dyn_cast_or_null<DeclRefExpr>(expr->IgnoreParenCasts());

		if (declRef) {
			VarDecl *VD = dyn_cast_or_null<VarDecl>(declRef->getDecl());

			if (VD && theContext.search(VD) && !theBuilder.findDeclRef(declRef)) {
				theBuilder.addDeclRef(declRef);
			}
		}

		return true;
	}

	EncodingTransformationContext &theContext;
	ExprBuilder &theBuilder;
};


class TransformerASTConsumer : public ASTConsumer {
public:
	TransformerASTConsumer (Rewriter &rewriter, EncodingTransformationContext &context, SourceManager &sourceMgr) :
            Visitor(rewriter, context, sourceMgr, builder),
			VarDeclEncoderVisitor(rewriter, context, sourceMgr),
			declRefVistor(context, rewriter, builder, false),
			ruleBuilder(context),
			theRewriter(rewriter),
			skipDecoding(context, builder)
			{}

    // Override the method that gets called for each parsed top-level
    // declaration.
    virtual bool HandleTopLevelDecl(DeclGroupRef group) {

    	for (DeclGroupRef::iterator b = group.begin(), e = group.end(); b != e; ++b) {
    		ruleBuilder.TraverseDecl(*b);
    	}

    	for (DeclGroupRef::iterator b = group.begin(), e = group.end(); b != e; ++b) {
            // Traverse the declaration using our AST visitor.
    		VarDeclEncoderVisitor.TraverseDecl(*b);
    		Visitor.TraverseDecl(*b);
    		// Ignore decoding on some expressions (e.g i++ when i is encoded)
    		skipDecoding.TraverseDecl(*b);
    		declRefVistor.TraverseDecl(*b);
    	}

        return true;
    }

private:
    TransformerASTVisitor Visitor;
    VariableDeclEncoderVisitor VarDeclEncoderVisitor;
    DeclRefVisitor declRefVistor;
    RuleBuilder ruleBuilder;
    ExprBuilder builder;
    Rewriter &theRewriter;
    SkipDecoding skipDecoding;
};

void runTransformer(char *ruleFile, char *fileName) {
    // CompilerInstance will hold the instance of the Clang compiler for us,
    // managing the various objects needed to run the compiler.
    CompilerInstance theCompInst;

    theCompInst.getLangOpts().CPlusPlus = 1;
    theCompInst.createDiagnostics(NULL, false);

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
    const FileEntry *fileIn = fileMgr.getFile(fileName);
    sourceMgr.createMainFileID(fileIn);
    theCompInst.getDiagnosticClient().BeginSourceFile(theCompInst.getLangOpts(), &theCompInst.getPreprocessor());

    // Create an AST consumer instance which is going to get called by
    // ParseAST.
    EncodingTransformationContext context(ruleFile);
    TransformerASTConsumer theASTConsumer(theRewriter, context, sourceMgr);

    // Parse the file to AST, registering our consumer as the AST consumer.
    ParseAST(theCompInst.getPreprocessor(), &theASTConsumer, theCompInst.getASTContext());

    // TheCompInst.getDiagnosticClient().EndSourceFile();
    // At this point the rewriter's buffer should be full with the rewritten
    // file contents.
#ifdef TEST
    const RewriteBuffer *rewriteBuf = theRewriter.getRewriteBufferFor(sourceMgr.getMainFileID());
    llvm::errs() << string(rewriteBuf->begin(), rewriteBuf->end());
#endif
#ifndef TEST
    theRewriter.overwriteChangedFiles();
#endif

    if (context.foundUprocessed()) {
    	context.displayUnproccessedVariables();
    }
}

void runNormalizer(char *fileName) {
    Normalizer theNormalizer(fileName);
    theNormalizer.Transform();
}

void usage(char *base) {

	printf("%s [-n|-t] [rulefile] file\n", base);
}

int main(int argc, char *argv[]) {

	if (argc < 3 ) {
		usage(argv[0]);
		return 1;
	}

	if(strcmp(argv[1], "-n") == 0) {
		if (argc == 4)
			runNormalizer(argv[3]);
		else if (argc == 3)
			runNormalizer(argv[2]);
	}
	else if(strcmp(argv[1], "-t") == 0) {
		if (argc != 4) {
			usage(argv[0]);
			return 1;
		}
		runTransformer(argv[2], argv[3]);
	}
	else {
		usage(argv[0]);
		return 1;
	}

	return 0;
}

