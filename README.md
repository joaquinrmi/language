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

### `NUM()`

```cpp
NUM::NUM();
NUM::NUM(uint8_t num);
NUM::NUM(uint8_t min_num, uint8_t max_num);
```

### `NUMT()`

```cpp
NUMT::NUMT();
NUMT::NUMT(double num);
NUMT::NUMT(double min_num, double max_num);
```

### `INUMT()`

```cpp
INUMT::INUMT();
INUMT::INUMT(double num);
INUMT::INUMT(double min_num, double max_num);
```

### `_`

### `-`

### `REP()`

```cpp
REP::REP(const std::vector<Command*>& commands, uint32_t min = 1, uint32_t max = 4294967295);
```

Define una repetición de la secuencia de comandos especificada en `commands`. `min` y `max` son la cantidad mínima y máxima de repeticiones de la secuencia, respectivamente.

Ejemplo de repetición con números:
```cpp
/*
La siguiente expresión valida cadenas formadas por "0" y "1" con un mínimo de cuatro dígitos y un máximo de 8.
*/
LanguageExpression exp("REP(NUM(0,1),4,8)");

e.check("10001");    // true
e.check("1021");     // false
```

## Características técnicas

* Las cadenas de texto analizadas deben de estar en formato `UTF-8`.