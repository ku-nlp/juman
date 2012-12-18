/*
  Darts -- Double-ARray Trie System

  $Id$;

  Copyright(C) 2001-2007 Taku Kudo <taku@chasen.org>
  All rights reserved.
*/
#include "darts.h"
#include <cstdio>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <sstream>
#include <fcntl.h>

int progress_bar(size_t current, size_t total) {
  static char bar[] = "*******************************************";
  static int scale = sizeof(bar) - 1;
  static int prev = 0;

  int cur_percentage  = static_cast<int>(100.0 * current/total);
  int bar_len         = static_cast<int>(1.0   * current*scale/total);

  if (prev != cur_percentage) {
      fprintf(stderr, "Making Double Array: %3d%% |%.*s%*s| ",
              cur_percentage, bar_len, bar, scale - bar_len, "");
      if (cur_percentage == 100)  fprintf(stderr, "\n");
      else                        fprintf(stderr, "\r");
      fflush(stdout);
  }

  prev = cur_percentage;

  return 1;
};

template <class Iterator>
inline size_t tokenizeTSV(char *str,
                          Iterator out, size_t max) {
// inline size_t tokenizeCSV(char *str, char** out, size_t max) {
  char *eos = str + std::strlen(str);
  char *start = 0;
  char *end = 0;
  size_t n = 0;

  for (; str < eos; ++str) {
    // skip white spaces
    // while (*str == ' ' || *str == '\t') ++str;
    start = str;
    str = std::find(str, eos, '\t');
    end = str;
    if (max-- > 1) *end = '\0';
    *out++ = start;
    ++n;
    if (max == 0) break;
  }

  return n;
}

unsigned int get_feature2id(std::map<char *, unsigned int> &feature2id, unsigned int &feature2id_count, char *key) {
    std::map<char *, unsigned int>::iterator it;
    it = feature2id.find(key);
    if (it != feature2id.end()) {
        return (*it).second;
    }
    else {
        feature2id[key] = feature2id_count++;
        std::cerr << "REG : " << key << " : " << feature2id_count - 1 << std::endl;
        return feature2id[key];
    }
}

int main(int argc, char **argv) {
  if (argc < 3) {
    std::cerr << "Usage: " << argv[0] << " File Index" << std::endl;
    return -1;
  }

  std::string file  = argv[argc-2];
  std::string index = argv[argc-1];

  Darts::DoubleArray da;

  std::vector<std::pair<std::string, std::string> > dic;
  std::istream *is;

  if (file == "-") {
    is = &std::cin;
  } else {
    is = new std::ifstream(file.c_str());
  }

  if (!*is) {
    std::cerr << "Cannot Open: " << file << std::endl;
    return -1;
  }

  std::string line;
  char *col[2];
  std::map<char *, unsigned int> feature2id;
  std::map<unsigned int, std::string> id2feature;
  unsigned int feature2id_count = 0;

  while (std::getline(*is, line)) {
      const size_t n = tokenizeTSV((char *)line.c_str(), col, 2);
      if (n != 2) {
          std::cerr << ";; format error: " << line;
      }
      dic.push_back(std::make_pair<std::string, std::string>(col[0], col[1]));
  }
  if (file != "-") delete is;

  // std::sort(dic.begin(), dic.end());

  size_t bsize = 0;
  size_t idx = 0;
  unsigned int start_feature2id_count = 0;
  std::string prev;
  std::vector<const char *> str;
  std::vector<size_t> len;
  std::vector<Darts::DoubleArray::result_type> val;

  for (size_t i = 0; i < dic.size(); ++i) {
      if (i != 0 && prev != dic[i].first) {
          str.push_back(dic[idx].first.c_str());
          len.push_back(dic[idx].first.size());
          val.push_back(bsize + (start_feature2id_count << 8));
          bsize = 1;
          idx = i;
          start_feature2id_count = feature2id_count;
      } else {
          ++bsize;
      }

      unsigned int feature_id = feature2id_count;
      id2feature[feature2id_count] = dic[i].first + "\t" + dic[i].second;
      feature2id_count++;

      prev = dic[i].first;
  }
  str.push_back(dic[idx].first.c_str());
  len.push_back(dic[idx].first.size());
  val.push_back(bsize + (start_feature2id_count << 8));

  if (da.build(str.size(), &str[0],
               &len[0], &val[0], &progress_bar) != 0
      || da.save(index.c_str()) != 0) {
      std::cerr << "Error: cannot build double array  " << file << std::endl;
      return -1;
  };

  std::cerr << "Done!, Compression Ratio: " <<
      100.0 * da.nonzero_size() / da.size() << " %" << std::endl;

  // for (std::map<unsigned int, std::string>::iterator it = id2feature.begin(); it != id2feature.end(); it++) {
  //     std::cout << (*it).first << " " << (*it).second << std::endl;
  // }

  return 0;
}
