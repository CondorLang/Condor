#include "path.h"

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

	void Path::CB(Isolate* isolate, std::string sub){
		Script::RunInternalScript(isolate, ConsoleBytes, "path", sub);
	}

	// TODO: Implement path for Windows
	void Path::SetBase(std::string str){
		#ifdef _WIN32
			
		#else
			base = realpath(str.c_str(), NULL);
		#endif
	}

	// TODO: Implement path for Windows
	std::string Path::GetFromBase(std::string str){
		#ifdef _WIN32
			
		#else
			int idx = -1;
			for (int i = base.length() - 1; i >= 0; i--){
				if (base.at(i) == '/'){
					idx = i;
					break;
				}
			}
			std::string folder = base.substr(0, idx);
			folder += "/" + str;
			return realpath(folder.c_str(), NULL);
		#endif
	}

} // namespace internal
}