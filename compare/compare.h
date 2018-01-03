/**
* @file compare.h
* @author yewmint
*/

#ifndef _COMPARE_H_
#define _COMPARE_H_

#include <vector>
#include <string>
#include <opencv2\opencv.hpp>

namespace Compare {
  typedef unsigned char uchar;

  /**
  * structure that store infomation of a picture
  */
  struct Picture {
    std::string path;
    cv::Mat_<uchar> fingerprint;
    long fileSize;
    Picture(
      std::string p,
      cv::Mat_<uchar> f,
      long fs
    ) : path(p), fingerprint(f), fileSize(fs) {}
  };

  /**
  * get paths of images from lines file
  * @param argc
  * @param argv
  * @param paths
  */
  void getArgs(
		int argc, 
		char **argv, 
		std::vector<std::string> &paths, 
		int *hamming
	);

  /**
  * scan fingerprints from paths
  * @param paths
  * @param pics
  */
  void scan(std::vector<std::string> &paths, std::vector<Picture*> &pics);

  /**
  * get duplicates using fingerprints
  * @param pics
  * @param dups
  */
  void getDuplicate(
    std::vector<Picture*> &pics,
    std::vector<std::string> &dups
  );

  /**
  * Engroup duplicates using fingerprint
  * @param pics
  * @param groups
  */
	void engroups(
		std::vector<Picture*> &pics,
		std::vector<std::vector<std::string>> &groups,
		const int HAMMING
  );

  /**
  * write paths of duplicates into file
  * @param dups
  * @param path
  */
  void write(
		std::vector<std::vector<std::string>> &groups, 
		std::string path
	);
}

#endif
