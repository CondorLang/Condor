#ifndef STRING_H_
#define STRING_H_

namespace Cobra {
namespace internal{

	class String
	{
	private:
		const char* value;
	public:
		String(){}
		~String(){}
		void SetValue(const char* val){value = val;}
		const char* GetValue(){return value;}
	};

} // namespace internal
}

#endif // STRING_H_
