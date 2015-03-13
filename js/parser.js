var code = '2 * 2 + 1';

var pos = 0;
var curr;
var OP = 'op', INT = 'int', FLOAT = 'float';
var number = /[0-9]/g;
var op = /[*+-\/]/g;
var stack = [];

function nextChar(peek){
	if (code.length > pos){
		return code[(peek ? pos++ : pos)];
	}
	return -1;
}

function nextToken(){
	curr = nextChar();
	if (curr === -1)
		return null;
	if (curr == ' ')
		return nextToken();
	if (number.test(curr)){
		var peek = nextChar(true);
		if (number.test(peek) || peek == '.'){

		}
		else{
			return {
				type: INT,
				data: parseInt(curr)
			};
		}
	}
	else if (op.test(curr)){
		return {
			type: OP,
			data: curr
		};
	}
	else{
		return null;
	}
}

function build(callback){
	var tok = nextToken();
	while (tok !== null){
		console.log(tok);
	}
}

build();
