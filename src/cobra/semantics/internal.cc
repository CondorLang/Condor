#include "internal.h"

namespace Cobra {
namespace internal{

	void Internal::PrintF(std::string msg){
		printf("%s", msg.c_str());
	}

} // namespace internal
}