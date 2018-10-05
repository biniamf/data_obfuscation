/*
 * VariableDeclEncoder.cpp
 *
 *  Created on: Dec 17, 2013
 *      Author: Biniam Fisseha Demissie
 */

#include "VariableDeclEncoder.h"

bool VariableDeclEncoderVisitor::VisitVarDecl(VarDecl *variableDecl) {

	if (theContext.search(variableDecl) && variableDecl->isLocalVarDecl()) {
		ValueDecl *valueDecl = dyn_cast<ValueDecl>(variableDecl);
		SourceRange range = variableDecl->getSourceRange();
		SourceLocation startLoc = variableDecl->getLocStart();
		string strChange;

		 if (valueDecl) {
			// We know that it's a compatible type by now (b/c search() says go ahead)
			strChange += "Encoded ";
			strChange += variableDecl->getNameAsString();
			theRewriter.RemoveText(range);
			theRewriter.InsertText(startLoc, strChange, true, true);
		 }
	}

	return true;
}

VariableDeclEncoderVisitor::~VariableDeclEncoderVisitor() {
	// TODO Auto-generated destructor stub
}

