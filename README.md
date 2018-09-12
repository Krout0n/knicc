# knicc

This is my toy C compiler. When you want to play this compiler, look at `test.sh` for valid syntaxes.

### How to play

```
$ make
$ echo 'int main() { return 0; }' | ./compiler
  => some assembly
```

```
$ echo 'int ten() { return 10;}  int main() { int i; i = 3; return ten() + i; }' | ./compiler > hoge.s && gcc hoge.s && ./a.out
$ echo $?
  => 13
```

### How to run test

```
$ make test
```
