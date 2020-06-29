# PL0-Analyzer

一个简单的 PL/0 词法/语法分析器

## Grammar
```
program = block "." .

block = [ "const" ident "=" number {"," ident "=" number} ";"]
        [ "var" ident {"," ident} ";"]
        { "procedure" ident ";" block ";" } statement .

statement = [ ident ":=" expression | "call" ident
              | "?" ident | "!" expression
              | "begin" statement {";" statement } "end"
              | "if" condition "then" statement
              | "while" condition "do" statement ].

condition = "odd" expression |
            expression ("="|"#"|"<"|"<="|">"|">=") expression .

expression = [ "+"|"-"] term { ("+"|"-") term}.

term = factor {("*"|"/") factor}.

factor = ident | number | "(" expression ")".
```

## Example
```pascal
const max = 100;
var arg, ret;

procedure isprime;
var i;
begin
	ret := 1;
	i := 2;
	while i < arg do
	begin
		if arg / i * i = arg then
		begin
			ret := 0;
			i := arg
		end;
		i := i + 1
	end
end;

procedure primes;
begin
	arg := 2;
	while arg < max do
	begin
		call isprime;
		if ret = 1 then write(arg);
		arg := arg + 1
	end
end;

call primes
.
```

## Build

### building with gcc
Currently it is not compatiable to mingw.
```bash
gcc -I. ./*.c -o analyzer
```

## Usage
CLI mode:
```bash
./analyzer
```

Read from file:
```bash
./analyzer filename
```

Help:
```bash
./analyzer -h
```

## License

This project is under GPL-3.0 license.
