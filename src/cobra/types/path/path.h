#ifndef PATH_H_
#define PATH_H_

#include <string>
#ifdef _WIN32
	#include <windoww.h>
#endif

#include "cobra/types/script/script.h"
#include "cobra/mem/isolate.h"
#include "path-bytes.h"

namespace Cobra {
namespace internal{

	class Path
	{
	private:
		Isolate* isolate;
		std::string base;

	public:
		Path(Isolate* iso);
		~Path(){}
		static Path* New(Isolate* iso);
		static void CB(Isolate* isolate, std::string sub);
		void SetBase(std::string str);
		std::string GetFromBase(std::string str);

	};

} // namespace internal
}

#endif // PATH_H_
