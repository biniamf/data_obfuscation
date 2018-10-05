#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// calculate data dependecy distance as explained in:
// https://aspire-fp7.eu/publication/494/2014/implementation-and-assessment-data-obfuscation-cc-code-based-residue-number

struct depends {
	char source[256];
	char dest[256];
};

int ruleIndex=0, depndsIndex=0, newRulesIndex=0;
struct depends dep[8192];
char *rules[256];
char *newRules[512];

void distance(FILE *fd, char *var, int dist) {

	int i, j;
	char buf[256];

	for (j=0; j<newRulesIndex;j++) {
		if (strcmp(newRules[j], var) == 0) {
			break;
		}
	}

	if (j == newRulesIndex) {
		memset(buf, '\0', sizeof(buf));
		sprintf(buf, "%s\n\n", var);
		fputs(buf, fd);

		newRules[newRulesIndex] = (char*)malloc(sizeof(char) * 256);
		strcpy(newRules[newRulesIndex++], var);
	}

	if (dist == 1) {
		for (i=0; i<depndsIndex; i++) {

			if (strcmp(var, dep[i].source) == 0) {

				if (strcmp(dep[i].source, dep[i].dest) == 0) continue;

				for (j=0; j<ruleIndex; j++) {

					if (strcmp(rules[j], dep[i].dest) == 0) {
						break;
					}
				}

				// avoid duplicates
				if (j == ruleIndex) {

					for (j=0; j<newRulesIndex;j++) {
						if (strcmp(newRules[j], dep[i].dest) == 0) {
							break;
						}
					}

					if (j == newRulesIndex) {
						memset(buf, '\0', sizeof(buf));
						sprintf(buf, "%s\n", dep[i].dest);
						fputs(buf, fd);

						newRules[newRulesIndex] = (char*)malloc(sizeof(char) * 256);
						strcpy(newRules[newRulesIndex++], dep[i].dest);
					}
				}
			}
		}
		return;
	}
	else {


		for (i=0; i<depndsIndex; i++) {

			if (strcmp(var, dep[i].source) == 0) {
				if (strcmp(dep[i].source, dep[i].dest) == 0) continue;

				for (j=0; j<ruleIndex; j++) {

					if (strcmp(rules[j], dep[i].dest) == 0) {
						break;
					}
				}

				// avoid duplicates
				if (j == ruleIndex) {

					for (j=0; j<newRulesIndex;j++) {
						if (strcmp(newRules[j], dep[i].dest) == 0) {
							break;
						}
					}

					if (j == newRulesIndex) {
						memset(buf, '\0', sizeof(buf));
						sprintf(buf, "%s\n", dep[i].dest);
						fputs(buf, fd);

						newRules[newRulesIndex] = (char*)malloc(sizeof(char) * 256);
						strcpy(newRules[newRulesIndex++], dep[i].dest);

						// for each in distance 1, find also the next...
						distance(fd, dep[i].dest, dist-1);
					}
				}
			}
		}

	}
}

int main(int argc, char *argv[])
{
	FILE *rule, *depend;
	char rulefile[256] = "";
	char dep_file[256] = "";
	int len=0; // distance

	if (argc < 4) {
		printf("USAGE: %s rule_file dependency_file distance\n", argv[0]);
		exit(1);
	}

	strncpy(rulefile, argv[1], 256);
	strncpy(dep_file, argv[2], 256);

	len = atoi(argv[3]);

	rule = fopen(rulefile, "r");
	if (rule == NULL) {
		printf("Error: Cannot read rule file!\n");
		exit (1);
	}

	depend = fopen(dep_file, "r");
	if (depend == NULL) {
		fclose(rule);
		printf("Error: Cannot read dependency file!\n");
		exit (1);
	}


	while (!feof(depend)) {

        fscanf(depend, "%256[^,],%256[^,\n]\n", dep[depndsIndex].source, dep[depndsIndex].dest);
        depndsIndex++;
	}

	while (!feof(rule)) {
		rules[ruleIndex] = (char*)malloc(sizeof(char) * 256);

		fscanf(rule, "%256[^\n]\n", rules[ruleIndex]);

		ruleIndex++;
	}

	char buf[64];
	FILE *fd;
	for (int i=1; i<len; i++) { 		// distance == len
		memset(buf, '\0', sizeof(buf));
		sprintf(buf, "%s.%d.rule", rulefile, i);
		fd = fopen(buf, "w");
		for (int j=0; j<ruleIndex; j++) { // for all the variables in the Rule file
			if (strlen(rules[j]) != 0) {

				distance(fd, rules[j], i);	// variables in distance i from variable j
			}
		}
		newRulesIndex=0;
		fclose(fd);
	}

	fclose(rule);
	fclose(depend);
}
