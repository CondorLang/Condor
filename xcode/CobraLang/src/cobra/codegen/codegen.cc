#include "codegen.h"

namespace Cobra {
namespace internal{
	
	CodeGen::CodeGen(Context* context){
		baseContext = context;
	}

	CodeGen::~CodeGen(){}

} // namespace internal
} // namespace Cobra