#include "include.h"

namespace Cobra {
	namespace Include {
		bool include(const char* path){
			try {
				return File::exists(path);
			}
			catch (int e){
				throw e;
			}
		}
	}

	namespace File {
		bool exists(const char* path){
			FILE *fp = fopen(path, "r");
			if(fp) {
				fclose(fp);
				return true;
			} 
			else {
				throw FILE_NOT_EXISTS;
			}
		}
	}
}