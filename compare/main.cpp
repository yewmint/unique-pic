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
  Compare::getPath(argc, argv, paths);

  cout << "Scanning fingerprints of images..." << endl;
  vector<Compare::Picture*> pics;
  Compare::scan(paths, pics);

  cout << "Comparing fingerprints for duplicates..." << endl;
  vector<std::string> duplicates;
  Compare::getDuplicate(pics, duplicates);

  cout << "Writting paths of duplicates into file..." << endl;
  Compare::write(duplicates, "dups.lines");

  cout << "Done." << endl;
  return 0;
}
