make compiler

exit_code() {
  expr=$1
  expected=$2
  echo $expr | ./compiler > test.s
  gcc test.s
  ./a.out
  exit_code=$?
  if [ $exit_code -eq $expected ] ; then
    echo "exit code test succeeded, got=${expr}"
  else
    echo "exit code test failed, args=${expr} expected=${expected}, got=${exit_code}"
  fi
  rm -rf a.out
}

printing_test() {
  expr=$1
  expected_output=$2
  echo $expr | ./compiler > test.s
  gcc -c test/lib.c
  gcc test.s lib.o
  result=`./a.out`
  if [ $expected_output = $result ] ; then
    echo "printing test succeeded, got=${expr}"
  else
    echo "printing test failed, args=${expr} expected=${expected_output}, got=${result}"
  fi
  rm -rf a.out
}

exit_code 1 1
exit_code 10 10
exit_code 22 22
exit_code '1+2;' 3
exit_code '12+34;' 46
exit_code '1+2+3;' 6
exit_code '11+22+33;' 66
exit_code '3-2;' 1
exit_code '3-2+1;' 2
exit_code '40-20+11;' 31
exit_code '99-21-37;' 41
exit_code '1*2;' 2
exit_code '1*2*3;' 6
exit_code '1+2*3;' 7
exit_code '1*2+3*4;' 14
exit_code '1+2*3+4;' 11
exit_code '15-3*4;' 3
exit_code '1+2; 3+4;' 7
exit_code '1+2; 1*4+3*10' 34
exit_code '11+22; 33+44; 114-10-40*2' 24
exit_code '(1+2)*3;' 9
exit_code '1+(2*3)' 7
exit_code '(1*2)+3;' 5
exit_code '(1+2+3);' 6
exit_code '(9-5)*10;' 40
exit_code 'a=1;' 1
exit_code 'a=1+2;' 3
exit_code 'a=1;b=2;' 2
exit_code 'a=1+2+3;10+20;' 30
printing_test 'print_ok();' "OK"
printing_test '1+2; print_ok();' "OK"
printing_test 'add_one(1);' "2"
printing_test 'add(1,2);' "3"
printing_test 'print_all_args(1,2,3,4,5,6);' "123456"

make clean
