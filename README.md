# Dobre - general purpose language
![workflow](https://github.com/GerbenAaltink/dobre/actions/workflows/make-dobre-single-platform.yml/badge.svg)

## Usage
### Syntax example
```
class A {}
class B(A){
    A a = 0
}
B *b = 1
```
### Compliation
Run `make build`
### Tests
Run `make test`
### Run custom file
Execute `./bin/dobre [file]`



## Parser

### Statistics
1841408 lines and 16572616 tokens
 - Lines per second: 170748.984375
 - Time; 10.78s

### Features 
 - declare class optionally classes to extend. These are two different notations.
 - prevent double declaration of classes.
 - prevent double declaration of classes to extend.
 - declare variable with native types (char, int, bool).
 - declare variable with new specified classes.
 - prevent declaration of variable with non existing type or class.
 - declare variable assign.

### Todo
 - assignment to boolean (crashes).
 - assignment to symbol (crashes).
