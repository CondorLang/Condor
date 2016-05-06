// Copyright 2016 Chase Willden and The CobraLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "path.h"

#ifdef _WIN32
	#include <Windows.h>
#endif

namespace Cobra {
namespace internal{

	Path::Path(Isolate* iso){
		isolate = iso;
	}

	Path* Path::New(Isolate* iso){
		void* p = (Path*) iso->GetMemory(sizeof(Path));
		Path* path = new(p) Path(iso);
		return path;
	}

	// TODO: Set Locale - http://askubuntu.com/questions/236924/matlab-not-working
	void Path::SetBase(std::string str){
		#ifdef _WIN32
			std::replace(str.begin(), str.end(), '/', '\\');
			int BUFSIZE = 4096;
			DWORD  retval=0;
		    BOOL   success; 
		    TCHAR  buffer[BUFSIZE]=TEXT(""); 
		    TCHAR  buf[BUFSIZE]=TEXT(""); 
		    TCHAR** lppPart={NULL};

		    retval = GetFullPathName(str.c_str(), BUFSIZE, buffer, lppPart);
		    std::string pth(buffer);
		    base = pth;
		#else
			base = realpath(str.c_str(), NULL);
		#endif
	}

	std::string Path::GetFromBase(std::string str){
		char ch = '/';
		#ifdef _WIN32
			ch = '\\';
		#endif
		int idx = -1;
		for (int i = base.length() - 1; i >= 0; i--){
			if (base.at(i) == ch){
				idx = i;
				break;
			}
		}
		std::string folder = base.substr(0, idx);
		folder += ch + str;


		#ifdef _WIN32
			std::replace(folder.begin(), folder.end(), '/', '\\');
			int BUFSIZE = 4096;
			DWORD  retval=0;
		    BOOL   success; 
		    TCHAR  buffer[BUFSIZE]=TEXT(""); 
		    TCHAR  buf[BUFSIZE]=TEXT(""); 
		    TCHAR** lppPart={NULL};

		    retval = GetFullPathName(folder.c_str(), BUFSIZE, buffer, lppPart);
		    std::string pth(buffer);
		    return pth;
		#else
			return realpath(folder.c_str(), NULL);
		#endif
	}

	std::string Path::GetCWD(){
 		char cCurrentPath[FILENAME_MAX];
 		if (!GetCurrentDir(cCurrentPath, sizeof(cCurrentPath))){} // TODO: Throw Error
		cCurrentPath[sizeof(cCurrentPath) - 1] = '\0'; /* not really required */
 		std::string result(cCurrentPath);
 		return result;
	}

	std::string Path::GetLibDir(){
		std::string path = Path::GetCWD();
		#ifdef WINDOWS
			path += "\\libs\\";
		#else
			path += "/libs/";
		#endif
		return path;
	}

} // namespace internal
}