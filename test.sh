make compiler

exit_code() {
  expr=$1
  expected=$2
  echo $expr | ./compiler > test.s
  gcc -g test.s
  ./a.out
  exit_code=$?
  if [ $exit_code -eq $expected ] ; then
    echo "exit code test succeeded, got=${expr}"
  else
    echo "exit code test failed, args=${expr} expected=${expected}, got=${exit_code}"
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
    echo "printing test succeeded, got=${expr}"
  else
    echo "printing test failed, args=${expr} expected=${expected_output}, got=${result}"
    exit 1
  fi
  rm -rf a.out
}

exit_code "main(){1;}" 1
exit_code "main(){10;}" 10
exit_code "main(){22;}" 22
exit_code 'main(){1+2;}' 3
exit_code 'main(){12+34;}' 46
exit_code 'main(){1+2+3;}' 6
exit_code 'main(){11+22+33;}' 66
exit_code 'main(){3-2;}' 1
exit_code 'main(){3-2+1;}' 2
exit_code 'main(){40-20+11;}' 31
exit_code 'main(){99-21-37;}' 41
exit_code 'main(){1*2;}' 2
exit_code 'main(){1*2*3;}' 6
exit_code 'main(){1+2*3;}' 7
exit_code 'main(){1*2+3*4;}' 14
exit_code 'main(){1+2*3+4;}' 11
exit_code 'main(){15-3*4;}' 3
exit_code 'main(){1+2; 3+4;}' 7
exit_code 'main(){1+2; 1*4+3*10;}' 34
exit_code 'main(){11+22; 33+44; 114-10-40*2;}' 24
exit_code 'main(){(1+2)*3;}' 9
exit_code 'main(){1+(2*3);}' 7
exit_code 'main(){(1*2)+3;}' 5
exit_code 'main(){(1+2+3);}' 6
exit_code 'main(){(9-5)*10;}' 40

exit_code 'main(){ a=1; a;}' 1
exit_code 'main(){ a=1+2; a;}' 3
exit_code 'main(){ a=1;b=2; b;}' 2
exit_code 'main(){ a=1+2+3;10+20;}' 30
exit_code 'main(){ a=3; a;}' '3'
exit_code 'main(){ a=1; b=2; a+b;}' '3'

printing_test 'main(){ print_ok(); }' "OK"
printing_test 'main() { 1+2; print_ok(); }' "OK"
printing_test 'main() { add_one(1);}' "2"
printing_test 'main() { add(1,2);}' "3"
printing_test 'main() { print_all_args(1,2,3,4,5,6); }' "123456"

exit_code 'foo() { 10; } main(){ foo(); }' "10"
exit_code 'foo(x){ x + 1; } main(){ foo(1); }' "2"
exit_code 'add(x,y) { x + y; } main() { add(1,2); }' '3'
exit_code 'foo(x) { x + 1;} main() { a = 3; foo(a);}' '4'
exit_code 'add(x,y) { x + y; } main() { a=1; b=2; add(a,b);}' '3'
exit_code 'local(x) { y = 20; x + y;} main() { a=10; local(a);}' '30'
exit_code 'local_assign(x) { y = x; y;} main(){ local_assign(10); }' '10'
exit_code 'local_assign(x) { y = x; y;} main(){ a=10; local_assign(a); }' '10'
exit_code 'expr(x) { x+10; } main() { expr(10+10);} ' '30'
exit_code 'main(){ if (1 < 2) 10; }' '10'
exit_code 'main(){ if (1) 10;}' '10'
exit_code 'main(){ if (2 < 1) 10; 20;}' '20'
# exit_code 'main(){ if (10 > 5) 10;}' '10'
exit_code 'main() { if (1+2 < 4) 10;}' '10'
# exit_code 'main() { if (1) { 10; 20;} ' '20'
exit_code 'main() { a=1; v=1; if (a) v=20; v;}' '20'
exit_code 'main() { a=0; v=1; if (a) v=20; v;}' '1'

make clean
