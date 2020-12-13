# Language

Language es una pequeña librería que te permitirá trabajar con lenguajes (especialmente de programación) en C++. Su funcionamiento es simple y consiste en definir el lenguaje utilizando objetos de tipo `LanguageExpression`, el cual representa una única expresión.

## Comandos

Para definir una **expresión** se utilizan **comandos**. Los comandos son los pequeños objetos que se encargan de validar una parte específica de la cadena de texto.

A continuación se presenta una lista de todos los comandos, cada uno con una descripción de su funcionamiento. **Se muestran solo los comandos que se han implementado, ya que el proyecto aún se encuentra en desarrollo**.

### `UCHAR()`

```cpp
UCHAR::UCHAR(const std::string& unique_char);
```

El nombre de este comando significa *unique char* y sirve para definir un único caracter específico.

### `CHAR()`

```cpp
CHAR::CHAR();
```

Define un único caracter cualquiera dentro del rango unicode.

### `STR()`

```cpp
STR::STR(const std::string& str);
```

Define una cadena de caracteres completa.

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

### `INUMT()`

```cpp
INUMT::INUMT();
INUMT::INUMT(double num);
INUMT::INUMT(double min_num, double max_num);
```

Define un número entero con cualquier cantidad de caracteres.

### `_`

Define un espacio en blanco obligatorio, entendiéndose como espacio en blanco a cualquier cantidad de caracteres de espaciado.
Este comando no acepta ningún parámetro. Para especificar espacios en blanco de forma más específica, se deberá hacer uso de los comandos `UCHAR()` y `REP()`.

Ejemplo:
```cpp
LanguageExpression exp("UCHAR(\"a\")_UCHAR(\"b\")");

exp.check("a b").ok();    // true
exp.check("a   b").ok();  // true
exp.check("a\nb").ok();   // true
exp.check("ab").ok();     // false
```

### `-`

Al igual que el comando `_`, define un espacio en blanco, con la diferencia de que este no es obligatorio.

Ejemplo:
```cpp
LanguageExpression exp("STR(\"a;\")_STR(\"var\")");

exp.check("a; var").ok();    // true
exp.check("a;   var").ok();  // true
exp.check("a;\nvar").ok();   // true
exp.check("a;var").ok();     // true
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

exp.check("10001").ok();    // true
exp.check("1021").ok();     // false
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

exp.check("1-2-3-4").ok();   // true
exp.check("1-2-3-").ok();    // false

/*
   Para que la segunda cadena se evalue como verdadera,
   se define el parámetro 'ignore' como true.
*/
LanguageExpression exp("REPIF(NUM(),UCHAR(\"-\"),true)");

exp.check("1-2-3-4").ok();   // true
exp.check("1-2-3-").ok();    // true
```


### `OR()`

```cpp
OR::OR(const std::vector<Command*>& first, const std::vecotr<Command*>& second);
```

Define una condición lógica que significa "`first` o `last` o ambas".

### `XOR()`

```cpp
XOR::XOR(const std::vector<Command*>& first, const std::vecotr<Command*>& second);
```

Define una condición lógica similar al XOR y funciona como "`first` o `last`, pero no se evalúa el caso en que ambas sean verdaderas". Esto puede dar lugar a que una expresión se evalúe como verdadera o como falsa, dependiendo de la secuencia de comandos en la que se encuentra el `XOR`.

Ejemplo:
```cpp
LanguageExpression exp("XOR(NUM(0),NUM(1))NUM(2)");

exp.check("02").ok();     // true
exp.check("12").ok();     // true
exp.check("012").ok();    // false
```

### `OPT()`

```cpp
OPT::OPT(const std::vector<Command*>& sequence);
```

Define una sección opcional, por lo que el comando siempre evaluará como verdadero.

Ejemplo:
```cpp
LanguageExpression exp("OPT(NUM(0))NUM(1)");

exp.check("01").ok();  // true
exp.check("1").ok();   // true
exp.check("0").ok();   // false 
```

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

exp2.check("221-302").ok();     // true
```

## Características técnicas

* Las cadenas de texto analizadas deben de estar en formato `UTF-8`.