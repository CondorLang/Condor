#ifndef INCLUDE_H_
#define INCLUDE_H_

#include <fstream>

namespace Cobra {
	namespace Include {
		#define FILE_NOT_EXISTS 100
		#define FILE_UNREADABLE 101
		#define FILE_UNKNOWN 102
		bool include(const char* path);
	}

	namespace File {
		bool exists(const char* path);
	}
}

#endif // INCLUDE_H_