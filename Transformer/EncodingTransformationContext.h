/*
 * EncodingTransformationContext.h
 *
 *  Created on: Dec 17, 2013
 *      Author: Biniam Fisseha Demissie
 */

#ifndef ENCODINGTRANSFORMATIONCONTEXT_H_
#define ENCODINGTRANSFORMATIONCONTEXT_H_

#include <cstdio>
#include <string>
#include <sstream>
#include <vector>

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

using namespace clang;
using namespace std;

class EncodingTransformationContext {
public:

	int validSyntaxForRule(char *rule, int lineNum) {

		char buf[20] = "";

		strncpy(buf, rule, 8);

		if (strcmp(buf, "FUNCTION") == 0) {
			char *a;

			a = (char*)malloc(sizeof(char)*128);
			memset(a, '\0', sizeof(&a));

			strncpy(a, rule+8, strlen(rule)-8);

			// "/method/[block]/variable"
			if (a[0] != '/')
				return 1;

			a++;

			while (*a != '\0' && *a != '/')	a++;
			// function name misssing
			if (*a == '\0')
				return 2;

			// "/[block]/variable
			a++;

			// variable name missing
			if (strlen(a) == 0)
				return 3;

			while (*a != '\0' && *a != '/')	a++;

			// no block
			if (*a == '\0')
				return 0;

			// "/variable"
			a++;

			// variable name missing
			if (strlen(a) == 0)
				return 3;

			return 0;

		}
		/*
		strncpy(buf, rule, 5);

		if (strcmp(buf, "CLASS") == 0){
			return 0;
		}

		strncpy(buf, rule, 6);

		if (strcmp(buf, "METHOD") == 0){
			return 0;
		}

		strncpy(buf, rule, 6);

		if (strcmp(buf, "GLOBAL") == 0){
			return 0;
		}
		*/
		return 1;
	}

	void whiten(char *in, char *out) {
		int i, j=0;

		for (i=0, j=0; in[i] != 0; i++) {

			// skip whitespace
			if (in[i] == ' ') continue;

			out[j++] = in[i];
		}
		out[j] = 0;
	}

    EncodingTransformationContext(char *rulefile) {
    	int lineNum=0;
    	FILE *fd;
		char *line, lineout[256];
		fd = fopen(rulefile, "r");

		if (fd == NULL) {
			printf("Error: could not open %s!\n", rulefile);
			exit(1);
		}

		line = (char*)malloc(sizeof(char) * 256);
		while (!feof(fd)) {

			fscanf(fd, "%256[^\n]\n", line);

			lineNum++;

			whiten(line, lineout);

			switch (validSyntaxForRule(lineout, lineNum)) {
				case 0:
					break;
				// case 1: exit(1);
				case 2:
					printf("RULE_FILE: Function name missing at line %d in file %s!\n", lineNum, rulefile);
					exit(1);
					break;
				case 3:
					printf("RULE_FILE: Variable name missing at line %d in file %s!\n", lineNum, rulefile);
					exit(1);
					break;
				default:
					printf("RULE_FILE: Invalid syntax at line %d in file %s!\n", lineNum, rulefile);
					exit(1);
					break;
			}

			variableFromRule.push_back(lineout);
		}
    }

    void display();
    bool foundUprocessed();
    void displayUnproccessedVariables();
    string findVariablePath(VarDecl* VD);
    void addVariablePath(VarDecl *VD, string path);
    bool findVar(string v);
    bool search(VarDecl *VD);

private:
    vector<VarDecl*> EncodedVariables;
    vector<string> variableFromRule, processedVariableFromRule;
    map<VarDecl*, string> variableMembershipPath; // functionName/{block}/var
};

#endif /* ENCODINGTRANSFORMATIONCONTEXT_H_ */
