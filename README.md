# Language

Language es una pequeña librería que te permitirá trabajar con lenguajes (especialmente de programación) en C++. Su funcionamiento es simple y consiste en definir el lenguaje utilizando objetos de tipo `LanguageExpression`, el cual representa una única expresión.

## Forma de uso

Hay dos formas de crear una nueva expresión, éstas son mediante su constructor y con el uso del método `LanguageExpression::create()`. En ambos casos, habrá que pasarle uno o dos argumentos. El primero es un `std::string` que contiene los **comandos** que formarán la expresión. El segundo argumento es un `std::vector<LanguageExpression*>` que contiene punteros a otros objetos de tipo `languageExpression` creados con anterioridad. Este segundo comando sólo será utilizado en caso de usar el comando `EXP()`.

```cpp
/*
   Crear una expresión mediante su construtor.
   En este caso la expresión estará vacía.
*/
LanguageExpression exp1;

/*
   En este caso la expresión representa un número entero en el rango [0,9].
*/
LanguageExpression exp2("NUM()");

/*
   Crear una expresión con el método create().
*/
LanguageExpression exp3;
exp3.create("REP(NUM(3,5),2,8)");
```

### Validar una expresión

Los objetos `LanguageExpression` sirven para validar cadenas de texto con un cierto formato. Una vez creado el objeto, se podrán validar cadenas con su método `LanguageExpression::check()`, el cual podrá devolver `true` o `false`.

```cpp
/*
   El siguiente objeto valida expresiones de ocho letras minúsculas.
*/
LanguageExpression exp("REP(R(97,122),8,8)");

exp.check("hola");      // false, no tiene ocho letras
exp.check("s4ludos");   // false, contiene un número
exp.check("diyxjre");   // true
```

## Comandos

Para definir una **expresión** se utilizan **comandos**. Los comandos son los pequeños objetos que se encargan de validar una parte específica de la cadena de texto.

A continuación se presenta una lista de todos los comandos, cada uno con una descripción de su funcionamiento. Los comandos están ordenados alfabéticamente.

### `_`

Define un espacio en blanco obligatorio, entendiéndose como espacio en blanco a cualquier cantidad de caracteres de espaciado.
Este comando no acepta ningún parámetro. Para especificar espacios en blanco de forma más específica, se deberá hacer uso de los comandos `UCHAR()` y `REP()`.

Ejemplo:
```cpp
LanguageExpression exp("UCHAR(\"a\")_UCHAR(\"b\")");

exp.check("a b");    // true
exp.check("a   b");  // true
exp.check("a\nb");   // true
exp.check("ab");     // false
```

### `-`

Al igual que el comando `_`, define un espacio en blanco, con la diferencia de que este no es obligatorio.

Ejemplo:
```cpp
LanguageExpression exp("STR(\"a;\")_STR(\"var\")");

exp.check("a; var");    // true
exp.check("a;   var");  // true
exp.check("a;\nvar");   // true
exp.check("a;var");     // true
```

### `CHAR()`

```cpp
CHAR::CHAR();
```

Define un único caracter cualquiera dentro del rango unicode.

### `EXP()`

```cpp
EXP::EXP(uint32_t expression_index);
```

Este comando se reemplaza por una expresión definida con anterioridad. `expression_index` es el índice de la expresión a utilizar dentro del vector de expresiones.

Ejemplo de uso:
```cpp
/*
   La primera expresión representa números de tres dígitos en los cuales
   el primer dígito está en el rango [1,3] y los siguientes dos dígitos
   en el rango [0,3].
*/
LanguageExpression exp1("NUM(1,3)REP(NUM(0,3),2,2)");

/*
   Se utiliza el comando EXP() pasándole 0 como argumento, lo que significa
   que tomará el puntero en la posición 0 del arreglo { &exp1 }.
*/
LanguageExpression exp2("EXP(0)UCHAR(\"-\")EXP(0)", 0, { &exp1 });

exp2.check("221-302");     // true
```

### `INUMT()`

```cpp
INUMT::INUMT();
INUMT::INUMT(double num);
INUMT::INUMT(double min_num, double max_num);
```

Define un número entero con cualquier cantidad de caracteres.

### `L()`

```cpp
L::L();
```

Define una letra del abecedario inglés que puede ser tanto minúscula como mayúscula.

### `LL()`

```cpp
LL::LL();
```

Funciona igual que `L()`, pero solo define letras minúsculas, excluyendo a las mayúsculas.

### `LU()`

```cpp
LU::LU();
```

Funciona igual que `L()`, pero solo define letras mayúsculas, excluyendo a las minúsculas.

### `NUM()`

```cpp
NUM::NUM();
NUM::NUM(uint8_t num);
NUM::NUM(uint8_t min_num, uint8_t max_num);
```

Define un número entero de un dígito. Los parámetros que recibe con el mínimo número y el máximo número del rango permitido, respectivamente. Si no se especifica un rango, se utilizará [0-9].

### `NUMT()`

```cpp
NUMT::NUMT();
NUMT::NUMT(double num);
NUMT::NUMT(double min_num, double max_num);
```

Define un número real con cualquier cantidad de caracteres.

### `OPT()`

```cpp
OPT::OPT(const std::vector<Command*>& sequence);
```

Define una sección opcional, por lo que el comando siempre evaluará como verdadero.

Ejemplo:
```cpp
LanguageExpression exp("OPT(NUM(0))NUM(1)");

exp.check("01");  // true
exp.check("1");   // true
exp.check("0");   // false 
```

### `R()`

```cpp
R::R(uint32_t min, uint32_t max);
```

Define un rango válido de caracteres unicode. Este rango se compone de un valor mínimo `min` y uno máximo `max`, especificados como enteros sin signo.

Ejemplo de uso:
```cpp
/*
   El siguiente objeto valida letras de la 'a' a la 'z'.
*/
LanguageExpression exp("R(97,122)");

exp.check("k");   // true
exp.check("s");   // true
exp.check("5");   // false
```

### `REP()`

```cpp
REP::REP(const std::vector<Command*>& sequence, uint32_t min = 1, uint32_t max = 4294967295);
```

Define una repetición de la secuencia de comandos especificada en `sequence`. `min` y `max` son la cantidad mínima y máxima de repeticiones de la secuencia, respectivamente.

Ejemplo de repetición con números:
```cpp
/*
   La siguiente expresión valida cadenas formadas por "0" y "1"
   con un mínimo de cuatro dígitos y un máximo de 8.
*/
LanguageExpression exp("REP(NUM(0,1),4,8)");

exp.check("10001");    // true
exp.check("1021");     // false
```

### `REPIF()`

```cpp
REPIF::REPIF(const std::vector<Command*>& sequence, const std::vector<Command*>& condition, bool ignore = false, uint32_t min = 1, uint32_t max = 4294967295);
```

Define una repetición condicional. La secuencia que se debe repetir es `sequence` y la condición para que se evalue una nueva repetición es `condition`. El parámetro `ignore` controla la última repetición: si su valor es `false`, entonces la secuencia no podrá terminar en `condition` y si su valor es `true`, la secuencia podrá terminar en `condition`. Los parámetros `min` y `max` controlan la cantidad de repeticiones, al igual que con el comando `REP`.

Ejemplo de repetición condicional con números:
```cpp
/*
   En la siguiente expresión, NUM() es la secuencia de comandos a repetir
   y UCHAR("-") es la condición para que haya una repetición.
*/
LanguageExpression exp("REPIF(NUM(),UCHAR(\"-\"))");

exp.check("1-2-3-4");   // true
exp.check("1-2-3-");    // false

/*
   Para que la segunda cadena se evalue como verdadera,
   se define el parámetro 'ignore' como true.
*/
LanguageExpression exp("REPIF(NUM(),UCHAR(\"-\"),true)");

exp.check("1-2-3-4");   // true
exp.check("1-2-3-");    // true
```

### `S()`

```cpp
S::S(const std::string& char_set);
```

Define un conjunto de caracteres para validar un único catacter de una expresión y que este pertenezca a dicho conjunto.

Ejemplo de uso
```cpp
LanguageExpression exp("S(\"abc#\"");

exp.check("b");   // true
exp.check("#");   // true
exp.check("d");   // false
```

### `STR()`

```cpp
STR::STR(const std::string& str);
```

Define una cadena de caracteres completa.

### `OR()`

```cpp
OR::OR(const std::vector<Command*>& first, const std::vecotr<Command*>& second);
```

Define una condición lógica que significa "`first` o `last` o ambas".

### `UCHAR()`

```cpp
UCHAR::UCHAR(const std::string& unique_char);
```

El nombre de este comando significa *unique char* y sirve para definir un único caracter específico.

### `XOR()`

```cpp
XOR::XOR(const std::vector<Command*>& first, const std::vecotr<Command*>& second);
```

Define una condición lógica similar al XOR y funciona como "`first` o `last`, pero no se evalúa el caso en que ambas sean verdaderas". Esto puede dar lugar a que una expresión se evalúe como verdadera o como falsa, dependiendo de la secuencia de comandos en la que se encuentra el `XOR`.

Ejemplo:
```cpp
LanguageExpression exp("XOR(NUM(0),NUM(1))NUM(2)");

exp.check("02");     // true
exp.check("12");     // true
exp.check("012");    // false
```

## Características técnicas

* Las cadenas de texto analizadas deben de estar en formato `UTF-8`.