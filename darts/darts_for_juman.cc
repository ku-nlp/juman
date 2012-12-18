#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "config.h"
#include "darts_for_juman.h"
#include "darts.h"

std::vector<Darts::DoubleArray *> darts_files;
std::vector<std::vector<std::string> *> darts_dbs;

#ifndef MAX_DIC_NUMBER 
#define MAX_DIC_NUMBER 5 /* 同時に使える辞書の数の上限 (JUMAN) */
#endif

#ifndef DICFILE
#define DICFILE "jumandic.dat"
#endif
#define DAFILE "jumandic.da"

static struct _dic_t {
  int used;
  int fd;
  off_t size;
  caddr_t addr;
} dicinfo[MAX_DIC_NUMBER];

// split function with split_num
template<class T>
inline int split_string(const std::string &src, const std::string &key, T &result, int split_num)
{
    result.clear();
    int len =  src.size();
    int i = 0, si = 0, count = 0;

    while(i < len) {
	while (i < len && key.find(src[i]) != std::string::npos) { si++; i++; } // skip beginning spaces
	while (i < len && key.find(src[i]) == std::string::npos) i++; // skip contents
	if (split_num && ++count >= split_num) { // reached the specified num
	    result.push_back(src.substr(si, len - si)); // push the remainder string
	    break;
	}
	result.push_back(src.substr(si, i - si));
	si = i;
    }

    return result.size();
}

// split function
template<class T>
inline int split_string(const std::string &src, const std::string &key, T &result)
{
    return split_string(src, key, result, 0);
}

/* open for reading */
std::vector<std::string> *read_text(const char *filename) {
    std::ifstream f_in(filename, std::ios::in);
    if (!f_in.is_open()) {
        fprintf(stderr, ";; cannot open %s for reading.\n", filename);
        return NULL;
    }

    std::vector<std::string> *db = new std::vector<std::string>;
    std::string buffer;
    while (getline(f_in, buffer)) {
        if (buffer.at(0) == '#') // comment line
            continue;
        // std::vector<std::string> line;
        // split_string(buffer, " ", line, 2);
        // db->at(atoi(line[0].c_str())) = line[1];
        // db->push_back(line[1]);
        db->push_back(buffer);
    }

    return db;
}

/* DB get */
char *db_get(std::vector<std::string> *db, const size_t index) {
    if (index < db->size())
        return (char *)(db->at(index).c_str());
    else
        return NULL;
}

/* DB close */
void db_close(std::vector<std::string> *db) {
    free(db);
}

void push_darts_file(char *basename) {
    Darts::DoubleArray *darts = new Darts::DoubleArray;
    std::string darts_filename = basename;
    darts_filename += DAFILE;

    if (darts->open(darts_filename.c_str()) != -1) {
        // std::cerr << "opened darts file: " << darts_filename << std::endl;
        darts_files.push_back(darts);
    }
    else {
        fprintf(stderr, ";; cannot open darts file: %s.\n", darts_filename.c_str());
        return;
    }

    std::string darts_dbname = basename;
    darts_dbname += DICFILE;
    std::vector<std::string> *db = read_text(darts_dbname.c_str());
    if (db)
        darts_dbs.push_back(db);
    else
        fprintf(stderr, ";; cannot open dbtext file: %s.\n", darts_dbname.c_str());
    return;
}

char *da_strcpy(char *s1, char *s2) {
    while (*s1++ = *s2++);
    return s1 - 1;
}

char *get_line(FILE *f, long pos){
  int i = 0, j = 0, ch, ffd = fileno(f);
  static int oldf = -1;
  static caddr_t addr;
  static off_t size;
  struct stat st;

  if (oldf != ffd){
    for (i = 0; i < MAX_DIC_NUMBER; i++){
      if (ffd == dicinfo[i].fd && dicinfo[i].used){
	oldf = dicinfo[i].fd;
	addr = dicinfo[i].addr;
	size = dicinfo[i].size;
	break;
      }
      if (dicinfo[i].used == 0){
	dicinfo[i].fd   = ffd;
	dicinfo[i].used = 1;
	fstat(dicinfo[i].fd, &st);
	dicinfo[i].size = size = st.st_size;
	dicinfo[i].addr = addr = reinterpret_cast<char *>(mmap(0, dicinfo[i].size, PROT_READ,
                                                               MAP_PRIVATE, dicinfo[i].fd, 0));
	break;
      }
    }
    if (i == MAX_DIC_NUMBER){
      exit(1);
    }
    oldf = ffd;
  }

  if (pos >= size)
    return NULL;

  return addr + pos;
}

// search the double array with a given str
size_t da_search(int dic_no, char *str, char *rslt) {
    char *buf;
    std::ostringstream o;
    rslt += strlen(rslt);

    Darts::DoubleArray::result_pair_type result_pair[1024];
    size_t num = darts_files[dic_no]->commonPrefixSearch(str, result_pair, 1024);

    for (size_t i = 0; i < num; i++) { // hit num
        size_t size = result_pair[i].value & 0xff;
        size_t start_id = result_pair[i].value >> 8;
        for (size_t j = 0; j < size; j++) {
            o << start_id + j;
            if ((buf = db_get(darts_dbs[dic_no], start_id + j))) {
                o.str("");
                *rslt++ = 1 + PAT_BUF_INFO_BASE;
                *rslt++ = 1 + PAT_BUF_INFO_BASE;
                size_t buf_len = strlen(buf);
                memcpy(rslt, buf, buf_len);
                rslt += buf_len;
                // rslt = da_strcpy(rslt, buf);
                *rslt++ = '\n';
                *rslt = '\0';
                // free(buf);
            }
        }
    }

    return num;
}

// traverse the double array with a given str
int da_traverse(int dic_no, char *str, size_t *node_pos, size_t key_pos, size_t key_length, char key_type, char deleted_bytes, char *rslt) {
    char *buf;
    std::ostringstream o;
    rslt += strlen(rslt);
    deleted_bytes++;

    int value = darts_files[dic_no]->traverse(str, *node_pos, key_pos, key_length);
    if (value >= 0) {
        size_t size = value & 0xff;
        size_t start_id = value >> 8;
        for (size_t j = 0; j < size; j++) {
            o << start_id + j;
            if ((buf = db_get(darts_dbs[dic_no], start_id + j))) {
                o.str("");
                *rslt++ = key_type + PAT_BUF_INFO_BASE;
                *rslt++ = deleted_bytes + PAT_BUF_INFO_BASE;
                size_t buf_len = strlen(buf);
                memcpy(rslt, buf, buf_len);
                rslt += buf_len;
                // rslt = da_strcpy(rslt, buf);
                *rslt++ = '\n';
                *rslt = '\0';
                // free(buf);
            }
        }
        return 1;
    }
    else {
        return value;
    }
}

void close_darts() {
    for (size_t i = 0; i < darts_files.size(); i++) {
        darts_files[i]->clear();
        delete darts_files[i];
        db_close(darts_dbs[i]);
    }
}
