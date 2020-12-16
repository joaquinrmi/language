# Language

Language es una pequeña librería que te permitirá trabajar con lenguajes (especialmente de programación) en C++. Su funcionamiento es simple y consiste en definir el lenguaje utilizando objetos de tipo `LanguageExpression`, el cual representa una única expresión.

* [https://github.com/joaquinrmi/language#forma-de-uso][Forma de uso]
    + [https://github.com/joaquinrmi/language#validar-una-expresi%C3%B3n][Validar una expresión]
    + [https://github.com/joaquinrmi/language#callback-de-validaci%C3%B3n][Callback de validación]
* [https://github.com/joaquinrmi/language#comandos][Comandos]
    + [https://github.com/joaquinrmi/language#\_][`_`]
    + [https://github.com/joaquinrmi/language#-][`-`]
    + [https://github.com/joaquinrmi/language#char][`CHAR()`]
    + [https://github.com/joaquinrmi/language#exp][`EXP()`]
    + [https://github.com/joaquinrmi/language#inumt][`INUMT()`]
    + [https://github.com/joaquinrmi/language#l][`L()`]
    + [https://github.com/joaquinrmi/language#ll][`LL()`]
    + [https://github.com/joaquinrmi/language#lu][`LU()`]
    + [https://github.com/joaquinrmi/language#num][`NUM()`]
    + [https://github.com/joaquinrmi/language#numt][`NUMT()`]
    + [https://github.com/joaquinrmi/language#opt][`OPT()`]
    + [https://github.com/joaquinrmi/language#r][`R()`]
    + [https://github.com/joaquinrmi/language#rep][`REP()`]
    + [https://github.com/joaquinrmi/language#repif][`REPIF()`]
    + [https://github.com/joaquinrmi/language#s][`S()`]
    + [https://github.com/joaquinrmi/language#str][`STR()`]
    + [https://github.com/joaquinrmi/language#switch][`SWITCH()`]
    + [https://github.com/joaquinrmi/language#or][`OR()`]
    + [https://github.com/joaquinrmi/language#uchar][`UCHAR()`]
    + [https://github.com/joaquinrmi/language#xor][`XOR()`]
* [https://github.com/joaquinrmi/language#ejemplos][Ejemplos]
    + [https://github.com/joaquinrmi/language#calculadora][Calculadora]
* [https://github.com/joaquinrmi/language#caracter%C3%ADsticas-t%C3%A9cnicas][Características técnicas]

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

### Callback de validación

Si una expresión se valida como verdadera en un objeto `LanguageExpression`, entonces el objeto hará una llamada a una función `FactoryFunction`, la cual podrá ser definida con el método `LanguageExpression::setFactoryFunction()`.

La definición de `FactoryFunction` se encuentra dentro de `LanguageExpression`.
```cpp
class LanguageExpression
{
public:
   typedef std::function<void(ParseProduct)> FactoryFunction;
};
```

El objeto de tipo `ParseProduct` recibido como parámetro posee los métodos `ParseProduct::begin()` y `ParseProduct::end()` que indican la posición inicial y final de la sección de texto validada, respecticamente.

Ejemplo de uso de la función *callbak*:
```cpp
LanguageExpression exp("UCHAR(\"$\")REP(L())");

exp.setFactoryFunction([](ParseProduct product) {
   cout << "Validado!" << endl;
});

if(exp.check("$holaMundo"))
{
   cout << "veradadero" << endl;
}
else cout << "false" << endl;

/*
   La salida tendrá una forma similar a la siguiente:
*/
// >> Validado!
// >> verdadero
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

Define un conjunto de caracteres para validar un único catacter de una expresión y que este pertenezca a dicho conjunto. Recibe como parámetros cualquier cantidad de elementos de los siguientes tipos: **cadena** de texto, de la que se tomará cada caracter como un elemento del conjunto y **rango** de valores unicode.

Ejemplo de uso
```cpp
LanguageExpression exp("S(R(97,122),\"#\")");

exp.check("g");   // true
exp.check("#");   // true
exp.check("5");   // false
```

### `STR()`

```cpp
STR::STR(const std::string& str);
```

Define una cadena de caracteres completa.

### `SWITCH()`

```cpp
SWITCH::SWITCH(Command* command, ...);
```

Define un conjunto de comandos. Como parámetros recibe cualquier cantidad de comandos individuales, por lo que no se aceptarán secuencias de comandos. En muchos casos, se preferirá utilizar otro comando en vez de `SWITCH()`, como `OR()` o `XOR()` o `SET()`, ya que tendrán comportamientos similares. Este comando siempre devuelve `true`.

**Importante**: en la versión actual, este comando tiene un mal rendimiento debido al algoritmo utilizado como validación: intenta validar la expresión una vez para cada comando en el conjunto hasta dar con el correcto. Como se podrá inferir, dependiendo de los comandos involucrados, esta técnica puede tener un pésimo rendimiento debido al coste de validación de los comandos individuales.

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

## Ejemplos

### Calculadora

En este ejemplo se construirá una calculadora con las siguientes operaciones:
* suma (`+`),
* resta (`-`),
* multiplicación (`*`),
* división (`/`).
Además, se incluirá soporte para la utilización de paréntesis `()` y corchetes `[]`.

Comenzamos sumando dos números, por ejemplo `5 + 6`, para ello podemos definir un objeto de tipo `LanguageExpression` de la siguiente forma:
```cpp
auto* sum_exp = new LanguageExpression("NUMT()-UCHAR(\"+\")-NUMT()");
```
De la misma forma, podemos definir el objeto correspondiente para las demás operaciones:
```cpp
auto* rest_exp = new LanguageExpression("NUMT()-UCHAR(\"-\")-NUMT()");
auto* mult_exp = new LanguageExpression("NUMT()-UCHAR(\"*\")-NUMT()");
auto* div_exp = new LanguageExpression("NUMT()-UCHAR(\"/\")-NUMT()");
```

Con todo lo anterior, podemos reconocer las operaciones básicas entre dos números, pero no seremos capaces de reconocer operaciones combinadas, como `2 * 3 + 5`. Para lograr esto tendremos que redefinir nuestras operaciones: cuando decidimos que queríamos sumar (o restas, o multiplicar, etc.) dos números, en realidad necesitábamos sumar dos **expresiones**.

Pero ¿qué entendemos por expresión? En primer lugar, una expresión puede ser un número, por lo que necesitamos definir:
```cpp
LanguageExpression num_exp("NUMT()");
```
Además, como vemos en `2 * 3 + 5`, una expresión puede ser también una operación entre dos expresiones. Y, si analizamos `2 * (3 + 5)` o `2 * [3] + 5`, vemos que una expresión es también un número o una operación encerrada entre paréntesis o entre corchetes. Pero ¿qué sucede con una expresión como `(((((5))))) + 3`? Con este ejemplo vemos que tanto los paréntesis como los corchetes pueden encerrar también a otros paréntesis y/o corchetes, por lo que necesitamos que puedan encerrar cualquier expresión válida.

Ahora estamos en condiciones de definir correctamente todas las operaciones y los paréntesis y los corchetes:
```cpp
auto* sum_exp = new LanguageExpression("EXP(0)-UCHAR(\"+\")-EXP(0)", 0, { math_exp });
auto* rest_exp = new LanguageExpression("EXP(0)-UCHAR(\"-\")-EXP(0)", 0, { math_exp });
auto* mult_exp = new LanguageExpression("EXP(0)-UCHAR(\"*\")-EXP(0)", 0, { math_exp });
auto* div_exp = new LanguageExpression("EXP(0)-UCHAR(\"/\")-EXP(0)", 0, { math_exp });
auto* bracket_exp = new LanguageExpression("UCHAR(\"(\")-EXP(0)-UCHAR(\")\")", 0, { math_exp });
auto* sq_bracket_exp = new LanguageExpression("UCHAR(\"[\")-EXP(0)-UCHAR(\"]\")", 0, { math_exp });
```
Aquí estamos definiendo todas las expresiones en términos de todas las expresiones, las cuales se referencian con la expresión `math_exp`. Como `math_exp` puede ser un número, una operación o una expresión encerrada entre paréntesis o corchetes, podemos definirla con el comando `SWITCH()`:
```cpp
LanguageExpression math_exp(
   "SWITCH(EXP(0),EXP(1),EXP(2),EXP(3),EXP(4),EXP(5),EXP(6))", 0,
   { num_exp, bracket_exp, sq_bracker_exp,
      sum_exp, rest_exp, mult_exp, div_exp }
);
```

## Características técnicas

* Las cadenas de texto analizadas deben de estar en formato `UTF-8`.