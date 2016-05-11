// Copyright 2016 Chase Willden and The StoneLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "fs.h"

namespace Stone {
namespace internal{

	bool FS::FileExists(std::string path){
		std::ifstream in(path);
		return in.good();
	}

	std::string FS::ReadFile(std::string path){
		std::ifstream in(path);
		if (!in) return "";
		std::string fileStr((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
		return fileStr;
	}

	void FS::WriteFile(std::string path, std::string contents){
		std::ofstream file;
    file.open(path.c_str());
    file << contents.c_str();
    file.close();
	}

	bool FS::DeleteFile(std::string path){
		return remove(path.c_str()) == 0;
	}

	std::vector<std::string> FS::ReadDir(std::string path, bool recursive){
		std::vector<std::string> result;
		char ch = '/';
		#ifdef _WIN32
			ch = '\\';
		#endif

		if (path[path.length() - 1] != ch) path += ch;
		DIR *dir;
		struct dirent* ent;
		if ((dir = opendir (path.c_str())) != NULL) {
		  while ((ent = readdir (dir)) != NULL) {
		  	std::string resultPath(ent->d_name);
		  	if (resultPath == "." || resultPath == "..") continue;
		  	resultPath = path + resultPath;
		    result.push_back(resultPath);
		    if (recursive &&  FS::IsDir(resultPath)){
		    	std::vector<std::string> sub = FS::ReadDir(resultPath, recursive);
		    	result.insert(result.end(), sub.begin(), sub.end());
		    }
		  }
		  closedir(dir);
		}
		return result;
	}

	bool FS::IsDir(std::string path){
		DIR *dir;
		struct dirent* ent;
		if ((dir = opendir (path.c_str())) != NULL) {
		  closedir(dir);
		  return true;
		}
		return false;
	}

} // namespace internal
}