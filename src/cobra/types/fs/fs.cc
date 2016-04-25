#include "fs.h"

namespace Cobra {
namespace internal{

	void FS::CB(Isolate* isolate, std::string sub){
		Script::RunInternalScript(isolate, FsBytes, "fs", sub);
	}

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

} // namespace internal
}