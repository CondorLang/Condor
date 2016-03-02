#ifndef TOKENS_H_
#define TOKENS_H_

#define TOKEN_LIST(TOK, KEY)  \
	TOK(LPAREN, "(", 0)			 	  \
	TOK(RPAREN, ")", 0)			    \
	TOK(LBRACK, "[", 0)			    \
	TOK(RBRACK, "]", 0)		 	    \
	TOK(LBRACE, "{", 0) 		    \
	TOK(RBRACE, "}", 0)			    \
	TOK(COLON, ":", 0)			    \
	TOK(SEMICOLON, ";", 0)      \
	TOK(PERIOD, ".", 0)			    \
	TOK(COMMA, ",", 0) 				  \
												      \
	/** LOGICAL **/						  \
	TOK(AND, "&&", 4) 				  \
	TOK(OR, "||", 5)					  \
	TOK(INC, "++", 0) 			    \
	TOK(DEC, "--", 0)				    \
	TOK(ARROW, "=>", 0)			    \
	TOK(EQ, "==", 9)					  \
	TOK(NEQ, "!=", 9)					  \
	TOK(LT, "<", 10)				    \
	TOK(GT, ">", 10)					  \
	TOK(LTE, "<=", 10)				  \
	TOK(GTE, ">=", 10)				  \
	TOK(NOT, "!", 0)					  \
														  \
	/** Math Operators **/		  \
	TOK(PLUS, "+", 12)				  \
	TOK(MINUS, "-", 12) 			  \
	TOK(MULT, "*", 13)				  \
	TOK(DIV, "/", 13) 				  \
	TOK(MOD, "%", 13)					  \
	TOK(PLUS_EQ, "+=", 2) 			\
	TOK(MINUS_EQ, "-=", 2) 		  \
	TOK(MULT_EQ, "*=", 2)			  \
	TOK(DIV_EQ, "/=", 2)			  \
	TOK(ASSIGN, "=", 0)					\
														  \
	/** Keywords **/					  \
	KEY(FUNC, "func", 0)			  \
	KEY(VAR, "var", 0) 				  \
	KEY(RETURN, "return", 0) 	  \
	KEY(TRY, "try", 0) 				  \
	KEY(CATCH, "catch", 0) 		  \
	KEY(FINALLY, "finally", 0)  \
	TOK(ILLEGAL, "illegal", 0)  \
															\
	/** Internal **/						\
	TOK(LITERAL, "literal", 0)	\
	TOK(NUMBER, "number", 0)		\
	TOK(BOOLEAN, "boolean", 0)	\
  TOK(EOS, "eos", 0)



class Tokens
{
public:
	#define TOK(name, string, precedence) name,
	  enum Value {
	    TOKEN_LIST(TOK, TOK)
	    NUM_TOKENS
	  };
	#undef TOK

	Tokens(){}
	~Tokens(){}
	static const char* GetName(Value v){return kNames[v];}
	static Tokens::Value GetToken(const char* name);

private:
	static const char* const kNames[NUM_TOKENS];

};

#endif // TOKENS_H_