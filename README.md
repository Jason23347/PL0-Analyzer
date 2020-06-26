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
		if ret = 1 then write arg;
		arg := arg + 1
	end
end;

call primes
.
```

## Build

### building with gcc/mingw
```bash
gcc -I. ./*.c -o analyzer
# or
x86_64-w64-mingw32-gcc -I. ./*.c -o analyzer
```

## Usage
Read from file:
```bash
./analyzer inputfile [-o outputfile]
```

Read from stdin:
```bash
./analyzer -
```

## Tests
```bash
$ gcc -Wall -I. ./*.c -o analyzer && ./analyzer - <<<'a := 1'
PL/0 Analizer Copyright 2020    ShuaiCheng Zhu

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

Lexical analysis result:

+-----+--------------------+--------------------+
|  No |             Symbol |        Symbol Type |
+-----+--------------------+--------------------+
|   1 |                  a |              ident |
|   2 |                 := |            becomes |
|   3 |                  1 |             number |
+-----+--------------------+--------------------+

Lexical and syntax analysis completed (0.000 secs)
```

```bash
$ ./analyzer - <<END
if a > b then
123begin
    a = a + 1;
end;
END
PL/0 Analizer Copyright 2020    ShuaiCheng Zhu

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

lex:2:1: invalid symbol: 123begin
```

```bash
$ ./analyzer - <<END
if a > b then
begin
    a = a + 1;
end
END
PL/0 Analizer Copyright 2020    ShuaiCheng Zhu

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

syntax:3:7: syntax error, expected becomes but got eql.
```

## License

This project is under GPL-3.0 license.
