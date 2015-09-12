#ifndef HANDLE_H_
#define HANDLE_H_

#include "heap.h"
#include "cobra/types/script/script.h"
#include "cobra/types/strings/string.h"
#include "cobra/ast/ast.h"

namespace Cobra {
namespace internal{

	class Script;
	class String;

	class Handle
	{
	private:
		HeapObject* obj;
	public:
		Handle(HeapObject* o = NULL, Isolate* iso = NULL) : obj(o), isolate(iso){}
		~Handle(){}
		Isolate* isolate;
		HeapObject* GetObject(){return obj;}
		String* ToString();
		Script* ToScript();
		static bool IsAddressValid(Isolate* iso, Address addr);
	};

	template<class T>
	class iHandle
	{
	private:
		Isolate* isolate;
		HeapObject* obj;
		bool destroyed;
		bool valid;
		bool IsValidDeep(){return valid && Handle::IsAddressValid(Location());}
	public:
		iHandle(){valid = false;obj = NULL; isolate = NULL;}
		iHandle(Isolate* iso, HeapObject* o){isolate = iso; obj = o; destroyed = false;valid = true;}
		~iHandle(){}
		iHandle<T> Localize(){
			iHandle<T> handle(isolate, obj);
			return handle;
		}
		TOKEN GetType(){return obj->type;}
		HeapObject* GetHeapObject(){return obj;}
		Address Location(){return obj->address;}
		bool IsNull(){return destroyed;}
		bool IsValid(){
			T* t = CAST(T*, obj->address);
			return obj != NULL && !obj->deleted && t != NULL;
		}
		T* operator->(){
			return CAST(T*, obj->address);
		}
	};

} // namespace internal
} // namespace Cobra

#endif // HANDLE_H_