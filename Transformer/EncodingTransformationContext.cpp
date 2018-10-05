/*
 * EncodingTransformationContext.cpp
 *
 *  Created on: Dec 17, 2013
 *      Author: Biniam Fisseha Demissie
 */

#include "EncodingTransformationContext.h"

void EncodingTransformationContext::addVariablePath(VarDecl *VD, string path) {

	variableMembershipPath[VD] = path;
}

string EncodingTransformationContext::findVariablePath(VarDecl *VD) {

	const map<VarDecl*, string>::iterator I = variableMembershipPath.find(VD);

	if (I != variableMembershipPath.end()) {
		return I->second;
	}

	return "";
}

void EncodingTransformationContext::display() {
	for (map<VarDecl*, string>::iterator I=variableMembershipPath.begin(); I != variableMembershipPath.end(); ++I)
		llvm::errs() << "["<<I->first <<"] => " << I->second << "\n";
}

bool EncodingTransformationContext::foundUprocessed(){
	vector<string>::iterator J;

	for (vector<string>::iterator I=variableFromRule.begin(); I != variableFromRule.end(); ++I) {
		for (J=processedVariableFromRule.begin(); J != processedVariableFromRule.end(); ++J) {
			if (*I == *J) break;
		}
		if (J == processedVariableFromRule.end()) {
			return true;
		}
	}
	return false;
}

void EncodingTransformationContext::displayUnproccessedVariables() {

	vector<string>::iterator J;

	llvm::outs() << "===== Variables Not Found =====\n";
	for (vector<string>::iterator I=variableFromRule.begin(); I != variableFromRule.end(); ++I) {
		for (J=processedVariableFromRule.begin(); J != processedVariableFromRule.end(); ++J) {
			if (*I == *J) break;
		}
		if (J == processedVariableFromRule.end()) {
			llvm::outs() << *I << "\n";
		}
	}
}

bool EncodingTransformationContext::findVar(string path) {
	for (vector<string>::iterator I=variableFromRule.begin(); I != variableFromRule.end(); ++I) {
		if (path == *I) {
			processedVariableFromRule.push_back(path);
			return true;
		}
	}
	return false;
}

bool EncodingTransformationContext::search(VarDecl *VD) {

	string path = findVariablePath(VD);

	if (path != "" && findVar(path))
		return true;
	else
		return false;
}

