#ifndef FS_H_
#define FS_H_

#include <fstream>
#include <streambuf>
#include <string>

namespace Cobra {
	namespace FS{
		std::string GetFile(std::string path);
	}	
}

#endif // FS_H_
