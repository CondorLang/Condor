#ifndef ERROR_H_
#define ERROR_H_

#define ERR_LIST(ERR)  \
	ERR(MISSING_SEMICOLON, "Missing Semicolon")			 	  



class Error
{
public:
	#define ERR(name, string) name,
	  enum Value {
	    ERR_LIST(ERR)
	    NUM_ERRORS
	  };
	#undef ERR

	Error(){}
	~Error(){}
	static const char* GetName(Value v){return kNames[v];}

private:
	static const char* const kNames[NUM_ERRORS];

};

#endif // ERROR_H_