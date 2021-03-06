## Data Obfuscation with Residue Number Coding for C/C++ Codes

A major threats to software security is malicious code tampering, where an attacker modifies the original program to make it work in a way that was not intended by the developer. Software obfuscation was proposed as a technique to mitigate the problem of malicious code tampering. Obfuscation is intended to protect the software from unauthorized modification by making code more difficult to understand to the potential attacker, and consequently more difficult and expensive to alter.

Software obfuscation can be applied either to code or to data. In this study we focus on the latter variant, i.e. data obfuscation. We adopt a data obfuscation technique based on homomorphic encoding. Program variables are encode to hide their actual values. While the encoding scheme allows to hide values of variables, it also supports operations in the encoded domain, so the computations on encoded variables can proceed without the need to decode them back in the clear, so that their value is not revealed to a potential attacker.

We improved the level of obscurity by applying obfuscation not just to sensitive variables, but also to those variables that are near and related to these sensitive variables. We propose a distance metric based on data-dependency among variables. We obfuscate also those variable whose distance from a sensitive variable is shorter than a given threshold. Depending on the threshold, we obtain different configurations of variables to obfuscate, based on their distance from the initial sensitive variable.

Obfuscating transformation and metric computation have been implemented in tool prototypes and evaluated empirically on real source codes. We measured memory and runtime overhead due to obfuscation when the distance threshold increases and data obfuscation is applied on more and more variables.

https://ieeexplore.ieee.org/document/7174809
