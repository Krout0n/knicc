exit_code() {
  expr=$1
  expected=$2
  echo $expr | ./compiler > test.s
  gcc -g test.s
  ./a.out
  exit_code=$?
  if [ $exit_code -eq $expected ] ; then
    echo "[\e[32mSUCCESS\e[37m] exit code test succeeded, got=${expr}"
  else
    echo "[\e[31mFAILED!!\e[37m]exit code test failed, args=${expr} expected=${expected}, got=${exit_code}"
    exit 1
  fi
  rm -rf a.out
}

printing_test() {
  expr=$1
  expected_output=$2
  echo $expr | ./compiler > test.s
  gcc -c test/lib.c
  gcc -g test.s lib.o
  result=`./a.out`
  if [ $expected_output = $result ] ; then
    echo "[\e[32mSUCCESS\e[37m] printing test succeeded, got=${expr}"
  else
    echo "[\e[31mFAILED\e[37m] printing test failed, args=${expr} expected=${expected_output}, got=${result}"
    exit 1
  fi
  rm -rf a.out
}

using_other() {
  expr=$1
  expected=$2
  echo $expr | ./compiler > test.s
  gcc -c test/lib.c
  gcc -g test.s lib.o
  ./a.out
  exit_code=$?
  if [  $exit_code -eq $expected ] ; then
    echo "[\e[32mSUCCESS\e[37m] using other test succeeded, got=${expr}"
  else
    echo "[\e[31mFAILED\e[37m] using other test failed, args=${expr} expected=${expected}, got=${exit_code}"
    exit 1
  fi
  rm -rf a.out
}

# exit_code "int main() { 3 * 2; }" '6'
# # exit_code 'int main() { int i; i = 3 * 2; return i; return 5;}' '6'
# exit_code 'int main() { 3*2; }' '6'
# exit_code 'int main() { return 3*2; }' '6'
# exit_code 'int main() { 3 * 2; }' '6'
# exit_code 'int main() { return 3 * 2; }' '6'
exit_code "int main(){ return 1;}" 1
exit_code 'int main() { 10; return; }' '10'
exit_code "int main(){10;}" 10
exit_code "int main(){22;}" 22
exit_code 'int main(){1+2;}' 3
exit_code 'int main(){12+34;}' 46
exit_code 'int main(){1+2+3;}' 6
exit_code 'int main(){11+22+33;}' 66
exit_code 'int main(){3-2;}' 1
exit_code 'int main(){3-2+1;}' 2
exit_code 'int main(){40-20+11;}' 31
exit_code 'int main(){99-21-37;}' 41
exit_code 'int main(){1*2;}' 2
exit_code 'int main(){1*2*3;}' 6
exit_code 'int main(){1+2*3;}' 7
exit_code 'int main(){1*2+3*4;}' 14
exit_code 'int main(){1+2*3+4;}' 11
exit_code 'int main(){15-3*4;}' 3
exit_code 'int main(){1+2; 3+4;}' 7
exit_code 'int main(){1+2; 1*4+3*10;}' 34
exit_code 'int main(){11+22; 33+44; 114-10-40*2;}' 24
exit_code 'int main(){(1+2)*3;}' 9
exit_code 'int main(){1+(2*3);}' 7
exit_code 'int main(){(1*2)+3;}' 5
exit_code 'int main(){(1+2+3);}' 6
exit_code 'int main(){(9-5)*10;}' 40

exit_code 'int main(){ int a; a = 1;}' 1
exit_code 'int main(){ int a; a=1+2;}' 3
exit_code 'int main(){ int a; a = 1; int b; b=2;}' 2
exit_code 'int main(){ int a; a=1+2+3;10+20;}' 30
exit_code 'int main(){ int a; a=1; int b; b=2; a+b;}' '3'

printing_test 'int main(){ print_ok(); }' "OK"
printing_test 'int main() { 1+2; print_ok(); }' "OK"
printing_test 'int main() { add_one(1);}' "2"
printing_test 'int main() { add(1,2);}' "3"
printing_test 'int main() { print_all_args(1,2,3,4,5,6); }' "123456"

exit_code 'int foo() { 10; } int main(){ foo(); }' "10"
exit_code 'int foo(int x){ x + 1; } int main(){ foo(1); }' "2"
exit_code 'int add(int x, int y) { x + y; } int main() { add(1,2); }' '3'
exit_code 'int foo(int x) { x + 1;} int main() { int a; a =3; foo(a);}' '4'
exit_code 'int add(int x,int y) { x + y; } int main() { int a; a = 1; int b; b=2; add(a,b);}' '3'
exit_code 'int local(int x) { int y; y = 20; x + y;} int main() { int a; a = 10; local(a);}' '30'
exit_code 'int local_assign(int x) { int y; y = x + 1;} int main(){ local_assign(10); }' '11'
exit_code 'int local_assign(int x) { int y; y = x + 1;} int main(){ int a; a=10; local_assign(a); return a; }' '10'
exit_code 'int expr(int x) { x+10; } int main() { expr(10+10);} ' '30'
exit_code 'int main() { return 10 < 20; }' '1'
exit_code 'int main(){ if (1 < 2) 10; }' '10'
exit_code 'int main(){ if (1) 10;}' '10'
exit_code 'int main(){ if (2 < 1) 10; 20;}' '20'
exit_code 'int main() { if (1+2 < 4) 10;}' '10'
exit_code 'int main() { int a; a=1; int v; v=1; if (a) v=20; v;}' '20'
exit_code 'int main() { int a; a=0; int v; v=1; if (a) v=20; v;}' '1'
exit_code 'int main() { int a; a=1; int v; v=1; if (a+1 < 3) v=20; v;}' '20'
exit_code 'int main() { int a; a=1; int v; v=1; if (a+1 < 1) v=20; v;}' '1'
exit_code 'int main() { if (1) { 10; 20;}}' '20'
exit_code 'int main() { int a; a = 10; a = a + 1; a;}' '11'
exit_code 'int main() { int a; a = 10; a = a + 1;}' '11'
exit_code 'int inc(int a) { a+1; } int main() { int a; a=10; if (1) { inc(a); }}' '11'
exit_code 'int main(){ int a; a=1; while(a < 5){ a = a + 1;} a;}' '5'
exit_code 'int main() { int a; a = 1; while (0) { a= a+1;} a;}' '1'
exit_code 'int main() { int a; int b; b = 3; for (a = 1; a < 10; a = a + 1) { a = a + 2; }  return a;}' '10'
exit_code 'int main() { int a; int b; b = 3; for (a = 1; a < 10; a = a + 1) { a = a + 2; }  return b;}' '3'
exit_code 'int main() {int a;int b;b = 3;for (a = 1; a < 3; a = a + 1) { a = a + 2; }return b;}' '3'
exit_code 'int main() { int a; a = 1; int *b; b = &a; *b;}' '1'
exit_code 'int main() { int a; a = 1; int *b; b = &a; *b + 3;}' '4'
exit_code 'int main() { int a; a = 1; int c; c = 2; int *b; b = &c; if(1){b = &a;} *b;}' '1'
exit_code 'int main() { int a; a = 1; int c; c = 2; int *b; b = &c; if(0){b = &a;} *b;}' '2'
exit_code 'int main() { int a; a = 3; if (a < 2+3) return 10; return 20; }' '10'
exit_code 'int main() { int a; a = 3; if (a < 3) return 10; return 20; }' '20'
exit_code 'int lower(int x, int y) { if (x < y) return x; return y; } int main() { return lower(10, 20); }' '10'
exit_code 'int lower(int x, int y) { if (x < y) return x; return y; } int main() { return lower(20, 10); }' '10'
exit_code 'int bigger(int x, int y) { if (x < y) return y; return x; } int main() { return bigger(5+5, 10+20); }' '30'
exit_code 'int second() { return 10; } int first() { return second(); } int main() { return first(); }' '10'
exit_code 'int main() {int a;int b;b = 3;for (a = 1; a < 3; a = a + 1) { b = a + 10; }return b;}' '12'
exit_code 'int main() {int a;int b;b = 3;for (a = 1; a < 3; a = a + 1) { b = a + 2; }return b;}' '4'
exit_code 'int main() { int a; int b; b = 3; for (a = 1; a < 3; a = a + 1) { b = b + a; }  return b;}' '6'
exit_code 'int main() { int a; int b; b = 0; for (a = 1; a < 3; a = a + 1) { b = b + a; }  return b;}' '3'
exit_code 'int fib(int n) {int a;a = 0;int b;b = 1;int i;int temp; for (i = 1; i < n; i = i + 1) {temp = b; b = a + b; a = temp;}return a;} int main() { return fib(10); }' '34'
exit_code 'int inc(int n) { return n+1; } int main() { int ten; ten = 10; return inc(ten + 1); }' '12'
exit_code 'int get_ptr_set(int x) { return x+1; } int main() { int *x; int y; y = 3; x = &y; get_ptr_set(*x); }' '4'
exit_code 'int set_local_ptr_var() {int x; x = 10; int *p; p = &x; return p;} int main() { int *p; p=set_local_ptr_var(); return *p;}' '10'
exit_code 'int main() { int a; a = 10; int *b; b = &a; int **c; c = &b; return **c;}' '10'
exit_code 'int main() { int a; a = 10; int *b; b = &a; return *b + 1;}' '11'
exit_code 'int main() { int a; a = 10; int *b; b = &a; int **c; c = &b; return **c;}' '10'
exit_code 'int main() { int a; a = 10; int *b; b = &a; int **c; c = &b; return **c+1;}' '11'
# exit_code "int *f(int *p) { return p; } int main() { int x; x = 123; int *y; y = f(&x); return *y; }" 123
using_other 'int main() { int *p; allocate4(&p, 1, 2, 4, 8); return *p; }' '1'
using_other 'int main() { int *p; allocate4(&p, 1, 2, 4, 8); int *q; q = p+1; return *q; }' '2'
using_other 'int main() { int *p; allocate4(&p, 1, 2, 4, 8); int *q; q = p+2; return *q; }' '4'
using_other 'int main() { int *p; allocate4(&p, 1, 2, 4, 8); int *q; q = p+3; return *q; }' '8'
using_other 'int main() { int *p; allocate4(&p, 1, 2, 4, 8); p = p+3; return *p; }' '8'
# using_other 'int main() { int *p; allocate4(&p, 1, 2, 4, 8); p = p+3; p = p -1 ;return *p; }' '4'
exit_code 'int main() { int *a; int b; a = &b; *a = 10; return b; }' '10'
exit_code 'int main() { int a[3]; int *b; b = a + 2; *b = 10; return *b; }' '10'
exit_code 'int main() { int a[2]; *a = 1; *(a + 1) = 2; int *p; p = a; *p + *(p + 1); }' '3'
exit_code 'int main() { int a[2]; *a = 1; a[1] = 2; return a[1]; }' '2'
exit_code 'int main() { int a[10]; a[7] = 21; return a[7]; }' '21'
exit_code 'int main() { int a[10]; a[6]=20; a[3+4] = 5*6; return a[2+5]; }' '30'
exit_code 'int main() {int a[10]; int i; i = 3; a[i] = 5; return a[3];}' '5'
using_other 'int main() { int *p; allocate4(&p, 1, 2, 4, 8); int i; i = 3; int *q; q = p + i; return *q; }' '8'
exit_code 'int main() { int a[10]; int i; for (i = 0; i < 10; i = i + 1) { a[i] = i; } return a[5];}' '5'
exit_code 'int main() { int a[10]; int i; for (i = 0; i < 10; i++) { a[i] = i; } return a[5] + a[7];}' '12'
exit_code 'int main() { int i; i = 4; i--; return i; }' '3'
exit_code 'int i; int main() { i = 5; return i; }' '5'
exit_code 'int i; int hoge() { i = 10; } int main(){i=3; hoge(); return i + 1; }' '11'
exit_code 'int i; int hoge() { int j; j = 10; i = 20;} int main() { int j; j = 1; hoge(); return i + j; }' '21'
exit_code 'int main(){ int i; i = 20; i += 3; return i;}' '23'
exit_code 'int foo() { return 10; } int main(){ int i; i = 20; i += foo() + 3; return i;}' '33'
exit_code 'int main() { int i; i = 0; int a; a = 0; for (; i < 10;) { a += i; i++; } return a; }' '45'
exit_code 'int fib(int n) { if (0 < n) { return n + fib(n-1); } } int main() { return fib(10); }' '55'
exit_code 'int ten(int x){ if (x < 10) { return ten(x+1); } return x; } int main() { return ten(9); }' '10'
exit_code 'int fib(int a, int b, int i, int n) {if (i < n) return fib(b, a+b, i+1, n); return a;} int main(){return fib(0, 1, 0, 10); }' '55'
exit_code 'int main() { char a; a=1; return a; }' '1'
exit_code 'int main() { int b; b = 10; char a; a=1; return a; }' '1'
exit_code 'int main() { int sum; sum = 0; int a; a = 1; int b; b = 10; for(; a < b; a++) { sum += a; } return sum; }' '45'
exit_code 'int main() { return -1 + 10; }' '9'
exit_code 'int main() { return -1 + -2 + 10; }' '7'
exit_code 'int main() { return -1 - 2 + 10; }' '7'
exit_code 'int main() { int i; i = 0; int a; a = 0; for(; i < 10;) { a += i; i++; } return a;}' '45'
exit_code 'int main() { if (1) return 10; else return 20; }' '10'
exit_code 'int main() { if (0) return 10; else return 20; }' '20'
exit_code 'int main() { int i; if (0) i = 10; else { i = 20; i++; } return i; }' '21'
exit_code 'int main() { if (1) { if (1) return 4;} return 3; }' '4'
exit_code 'int main() { if (1) { if (0) return 4;} return 3; }' '3'
exit_code 'int main() {if (0) return 10; else if (0) return 20; else return 30;}' '30'
exit_code 'int main() {if (1) { if (0) return 0; else if (0) return 1; else return 2;  }}' '2'
exit_code 'int main() {char x[3]; x[0] = 20; x[1] = 2; int y; y = 4; return x[0] + y;}' '24'
exit_code 'int main() {char x[3]; x[0] = 20; x[1] = 30; x[2] = 10; return x[1]; }' '30'
exit_code 'int main() {char x[3]; x[0] = 1; x[1] = 2; x[2] = 10; return x[2]; }' '10'
exit_code 'int main() { char *x; x = "abc"; return x[0]; }' '97'
exit_code 'int main() { char *x; x = "abc"; char *y; y = "def"; return y[0]; }' '100'
exit_code 'int main() { char *x; x = "abc"; return x[1]; }' '98'
exit_code 'int main() { char *x; x = "abc"; return x[2]; }' '99'
exit_code 'char foo() { char x; x = 10; return x;} int main() { return foo(); }' '10'
printing_test 'int main() { print("abc"); }' 'abc'
exit_code 'int main() { if (2 == 2) return 3; return 4;}' '3'
exit_code 'int main() { if (1 == 2) return 3; return 4;}' '4'
exit_code 'int main() { if (1 != 2) return 3; return 4;}' '3'
exit_code 'int main() { if (2 != 2) return 3; return 4;}' '4'
exit_code 'int main() { int a; int *b; b = &a; if (&a == b) return 10; return 20; }' '10'
exit_code 'int main() { int a; int *b; int **c; b = &a; c = &b; if (*c == &a) return 10; return 20; }' '10'
exit_code 'int main() { if (1 && 1) return 2; return 3;}' '2'
exit_code 'int main() { if (0 && 1) return 2; return 3;}' '3'
exit_code 'int main() { if (1 && 0) return 2; return 3;}' '3'
exit_code 'int main() { if (1 || 0) return 2; return 3;}' '2'
exit_code 'int main() { if (0 || 1) return 2; return 3;}' '2'
exit_code 'int main() { if (0 || 0) return 2; return 3;}' '3'
exit_code 'int main() { if (1 < 2 && 3 < 4) return 2; return 3; }' '2'
exit_code 'int main() { if (2 < 1 && 3 < 4) return 2; return 3; }' '3'
exit_code 'int main() { if (2 < 1 || 3 < 4) return 2; return 3; }' '2'
exit_code 'int main() { if (1 < 2 < 3) return 4; return 5;}' '4'
exit_code 'int main() { if (1 < 1 < 3) return 4; return 5;}' '5'
exit_code 'int main() { if (1 < 4 < 3) return 4; return 5;}' '5'
exit_code 'int main() { int i; i = 10; if (1 < i < 3) return 4; return 5;}' '5'
exit_code 'int main() { int i; i = 10; if (i < 11 < 2) return 4; return 5;}' '5'
exit_code 'int main() { int i; i = 10; if (i < 1 < 2) return 4; return 5;}' '5'
exit_code 'int main() { int i; i = 10; if (1 < i < i + 1) return 4; return 5;}' '4'
exit_code 'int main() { int i; i = 10; if (i < i+1 < i + 2) return 4; return 5;}' '4'
exit_code 'int main() { if (-10 < -9 < -8) return 4; return 5;}' '4'
exit_code 'int main() { enum N { ZERO, ONE, TWO}; TWO;}' '2'
exit_code 'int main() { enum Numbers { ZERO, ONE, TWO, THREE }; ONE + TWO;}' '3'
exit_code 'int main() { enum Numbers { ZERO, ONE, TWO, THREE }; return TWO + THREE;}' '5'
exit_code 'int main() { enum Numbers { ZERO, ONE, TWO}; if (ONE < TWO) return 10; return 20;}' '10'
exit_code 'int main() { if (10 < 20) { int i; i = 9; return i + 1;} return 9; }' '10'
exit_code 'int main() { if (1) { int i; i = 3*2; return i;} return 5; }' '6'
exit_code 'int main() { enum N { ZERO, ONE, TWO, THREE }; if (1) { int i; i = ONE + TWO; return i;} else return 5;}' '3'
exit_code 'enum Num {ZERO, ONE, TWO}; int main() { return ONE + TWO; }' '3'
exit_code 'enum Num {ZERO, ONE, TWO, THREE}; int plus_one(int x, int y) { return x + y + ONE; } int main() { return plus_one(TWO, THREE); }' '6'
exit_code 'enum Num {ZERO, ONE, TWO, THREE, FOUR}; int main() { int a[5]; int i; for (i = 0; i < FOUR; i++) a[i] = i; return a[THREE]; }' '3'
exit_code 'int main() { return 10/2;}' '5'
exit_code 'int main() { return 8/2; }' '4'
exit_code 'int main() { return 8%4; }' '0'
exit_code 'int main() { return 8%3; }' '2'
exit_code 'int main() { return 273%11; }' '9'
exit_code 'int main() {if (1) { if (0) return 0; else if (0) return 1; else return 2; } else if (0) return 20; else return 30;}' '2'
exit_code 'int main() {if (1) { if (0) return 0; else if (1) return 1; else return 2; } else if (0) return 20; else return 30;}' '1'
exit_code 'int main() {if (0) { if (0) return 0; else if (1) return 1; else return 2; } else if (1) return 20; else return 30;}' '20'
exit_code 'int main() { int a; a = 0; int i; for (i = 1; i <= 10; i++) a+= i; return a;}' '55'
exit_code 'int main() { int a; a = 0; int i; for (i = 10; i > 0; i--) a += i; return a;}' '55'
exit_code 'int main() { int a; a = 0; int i; for (i = 10; i >= -10; i--) a += i; return a;}' '0'
exit_code 'char plus_one(char x) { return x + 1; } int main() { return plus_one(10); }' '11'
exit_code 'char plus_xyz(char x, int y, char z) { return x + y + z; } int main() { return plus_xyz(10, 20, 30); }' '60'
exit_code 'int ten(int *x) { *x = 10; } int main() { int i; i = 1; ten(&i); return i;}' '10'
exit_code 'int main() { int i; for (i = 0; i < 10; i++) { if (i == 5) break;} return i; }' '5'
exit_code 'int main() { int i; int j; for (i = 0; i < 10; i++) { for (j = 0; j < 10; j++) { 9; } if (i == 6) break; } return i + j;}' '16'
exit_code 'int main() { int i; int j; for (i = 0; i < 10; i++) { for (j = 0; j < 10; j++) { if (i == 6 && j == 7) break; }} return i + j;}' '20'
exit_code 'int main() { int i; int s; s = 0; for (i = 0; i <= 10; i++) { if (i % 2 != 0) continue; s += i; } return s;}' '30'
exit_code 'int main() { int i = 10; return i;}' '10'
exit_code 'int main() { for (int i = 0; i < 100; i++) { continue; } return i;}' '100'
exit_code 'int foo() { return 10;} int main() { int ten = foo(); return ten;}' '10'
exit_code 'int main() { struct Point { int x; int y; }; struct Point p; p.x = 1; p.y = 2; return p.x + p.y; }' '3'
# exit_code 'int main() { int x[3]; x = {1,10,20}; return x[1] + x[2]; }'  '30'
# exit_code 'int main() {if (1) { if (0) return 0; else if (0) return 1; else return 2; } else if (0) return 20; else retrn 30;}' '2'
# exit_code 'int main() { int a[2]; *a = 1; *(a + 1) = 2; int *p; p = a; return *p + *(p + 1);  }' '3'
# using_other 'int main() { int *p; allocate4(&p, 1, 2, 4, 8); int q; q = *(p+3); return q; }' '8'
# exit_code 'int main() {int *a; int b; a=&b; *a=10; return b;}' '10'
# exit_code 'int main() { int a[2]; int *p; p = a + 1; *p = 2; return *(a+1); }' '2'

# exit_code 'int main() { int a; int b; a = 1; b = 2; int *p; p = &b; p = p + 1;}' '1' // ローカル変数のアラインメントを利用したかった
# exit_code 'int fib(int a, int b, int i, int n) {if (i < n) return fib(b, a+b, i+1, n); return a;} int main(){return fib(0, 1, 0, 10); }' '34'
# exit_code 'int fib(int a, int b, int i, int n){ if (n < i) { return a;} return fib(b, a+b, i+1, n); } int main() {return fib(0, 1, 0, 1);}' '34'
# failng_test 'int inc (a) { a + 1; } int main() { inc(10) }' 正しく落ちた

make clean
