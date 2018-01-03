/**
* @file main.cpp
* @author yewmint
*/

#include <iostream>
#include "compare.h"

using namespace std;

int main(int argc, char **argv) {
	cout << "Parsing paths to images..." << endl;
	vector<string> paths;
	int hamming;
	Compare::getArgs(argc, argv, paths, &hamming);

	cout << "Scanning fingerprints of images..." << endl;
	vector<Compare::Picture*> pics;
	Compare::scan(paths, pics);

	cout << "Engrouping duplicates..." << endl;
	vector<vector<string>> groups;
	Compare::engroups(pics, groups, hamming);

	cout << "Writting paths of groups into file..." << endl;
	Compare::write(groups, "groups.lines");

	cout << "Done." << endl;
	return 0;
}
