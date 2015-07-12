#include "fs.h"

namespace Cobra{
namespace FS{
	std::string GetFile(std::string path){
		std::ifstream in(path.c_str());
		std::string fileStr((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
		return fileStr;
	}
} // namespace FS
} // namespace Cobra