/**
* @file compare.cpp
* @author yewmint
*/

#include "compare.h"

#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <future>
#include <opencv2/opencv.hpp>

#ifdef WIN32
#include <windows.h>
#endif

#define CORE_NUM 8
#define HAMMING_DISTANCE_DIFF 3

using namespace std;
using namespace cv;
using namespace Compare;

#ifdef WIN32
/**
* convert utf8 to gbk to fit windows build
* @param strUTF8 original string
*/
string UTF8ToGBK(const std::string& strUTF8) {
  int len = MultiByteToWideChar(CP_UTF8, 0, strUTF8.c_str(), -1, NULL, 0);
  unsigned short * wszGBK = new unsigned short[len + 1];
  memset(wszGBK, 0, len * 2 + 2);
  MultiByteToWideChar(CP_UTF8, 0, (LPCTSTR)strUTF8.c_str(), -1, (LPWSTR)wszGBK, len);

  len = WideCharToMultiByte(CP_ACP, 0, (LPWSTR)wszGBK, -1, NULL, 0, NULL, NULL);
  char *szGBK = new char[len + 1];
  memset(szGBK, 0, len + 1);
  WideCharToMultiByte(CP_ACP, 0, (LPWSTR)wszGBK, -1, szGBK, len, NULL, NULL);
  //strUTF8 = szGBK;
  std::string strTemp(szGBK);
  delete[]szGBK;
  delete[]wszGBK;
  return strTemp;
}
#endif

/**
* show error message and throw it
* @param msg
*/
void showError(string msg) {
  cerr << msg << endl;
  throw msg;
}

/**
* get size of file
* @param path
* @return size
*/
long getFileSize(string path) {
  #ifdef WIN32
  path = UTF8ToGBK(path);
  #endif
  std::ifstream f(path);
  if (!f.is_open()) return 0;

  f.seekg(0, std::ios_base::end);
  std::streampos sp = f.tellg();
  return sp;
}

/**
* get fingerprint of an image
* @param path path to image
* @return fingerprint matrix
*/
Mat_<uchar> getFingerprint(string path) {
  #ifdef WIN32
  path = UTF8ToGBK(path);
  #endif
  auto img = imread(path);

  if (!img.data) {
    showError(path);
    showError("Error: invalid path to image.");
  }

  // resize gray scale image into 8x8 matrix
  cvtColor(img, img, COLOR_BGR2GRAY);
  resize(img, img, Size(8, 8), 0, 0, INTER_LINEAR);

  // elements greater than average are assigned to 1, others are assigned to 0
  uchar avg = sum(img)[0] / img.total();
  img = (img >= avg) / 255;

  return img;
}

/**
* scan paths with index from start to end
* used to invoke multi-thread scan
* @param paths
* @param pics Picture objects of paths
* @param begien begin index
* @param end end index
*/
void divideScan(
  std::vector<std::string> *paths,
  std::vector<Picture*> *pics,
  size_t begin,
  size_t end
) {
  for (size_t i = begin; i < end; ++i) {
    auto path = (*paths)[i];
    auto fp = getFingerprint(path);
    (*pics)[i] = new Picture(path, fp, getFileSize(path));
  }
}

/**
* get paths of images from lines file
* @param argc
* @param argv
* @param paths
*/
void Compare::getArgs(
	int argc,
	char **argv,
	std::vector<std::string> &paths,
	int *hamming
) {
  if (argc != 3) {
    showError("Error: invalid arguments.");
  }

	*hamming = atoi(argv[1]);

  // read lines into paths
  ifstream file(argv[2]);
  while (!file.eof()) {
    string line;
    getline(file, line);
    if (line.size() > 0) {
      paths.push_back(line);
    }
  }
}

/**
* scan fingerprints from paths
* @param paths
* @param pics
*/
void Compare::scan(std::vector<std::string>& paths, std::vector<Picture*>& pics) {
  // resize pics in advance to avoid changing size in loop
  size_t len = paths.size();
  pics.clear();
  pics.resize(len);

  // determine number of threads by length of pics and number of cpu cores
  size_t core = CORE_NUM;
  size_t thdNum = core > len ? len : core;

  // ensure that each pic is dispatched to a thread
  size_t eachNum = ceil(len / (double)thdNum);

  // store futures of async calls
  vector<future<void>> thdFut(thdNum);
  for (size_t thdIdx = 0; thdIdx < thdNum; ++thdIdx) {
    size_t begin = thdIdx * eachNum;
    size_t end = begin + eachNum;
    end = end > len ? len : end;
    thdFut[thdIdx] = async(divideScan, &paths, &pics, begin, end);
    // divideScan(&paths, &pics, begin, end);
  }

  for (const future<void> &fut : thdFut) {
    fut.wait();
  }
}

/**
* get duplicates using fingerprints
* @param pics
* @param dups
*/
void Compare::getDuplicate(
  std::vector<Picture*>& pics,
  std::vector<std::string>& dups
) {
  // store unique pictures
  vector<Picture*> unique;
  for (Picture *pic : pics) {
    bool isDebut = true;

    for (size_t i = 0; i < unique.size(); ++i) {
      auto uniquePic = unique[i];

      // calculate hamming distance
      auto tmpMat = pic->fingerprint + uniquePic->fingerprint;
      size_t hammingDistance = sum(tmpMat == 1)[0] / 255;

      if (hammingDistance <= HAMMING_DISTANCE_DIFF) {
        isDebut = false;
        // if current picture is of lower quality, drop it into dups
        // else exchange it with unique picture
        if (pic->fileSize < uniquePic->fileSize) {
          dups.push_back(pic->path);
        }
        else {
          dups.push_back(uniquePic->path);
          unique[i] = pic;
        }
        break;
      }
    }

    // if current picture debuts, store it in unique vector
    if (isDebut) {
      unique.push_back(pic);
    }
  }
}

/**
* Engroup duplicates using fingerprint
* @param pics
* @param groups
*/
void Compare::engroups(
  std::vector<Picture*> &pics,
  std::vector<std::vector<std::string>> &groups,
	const int HAMMING
){
  vector<vector<Picture*>> picGroups;

  for (Picture *pic : pics){
    bool isDebut = true;
    for (vector<Picture*> & picGroup: picGroups){
      // calculate hamming distance
      auto tmpMat = picGroup[0]->fingerprint + pic->fingerprint;
			size_t hammingDistance = sum(tmpMat == 1)[0] / 255;

			if (hammingDistance <= HAMMING) {
				isDebut = false;
				picGroup.push_back(pic);
				break;
			}
    }

		if (isDebut) {
			picGroups.push_back(vector<Picture*>({ pic }));
		}
  }

	groups.resize(0);
	for (const vector<Picture*> & picGroup : picGroups) {
		vector<string> pathGroup;

		for (const Picture *pic : picGroup) {
      ostringstream oss;
      oss << (int)pic->fileSize << endl << pic->path;
			pathGroup.push_back(oss.str());
		}

		groups.push_back(pathGroup);
	}
}

/**
* write paths of groups into file
* @param dups
* @param path
*/
void Compare::write(
	std::vector<std::vector<std::string>> &groups,
	std::string path
){
  ofstream file(path);
  for (const vector<string> group: groups) {
		for (const string path : group) {
			file << path << endl;
		}
		file << "------" << endl;
  }
}
