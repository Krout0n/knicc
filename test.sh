make compiler

exit_code() {
  expr=$1
  expected=$2
  echo $expr | ./compiler > test.s
  gcc test.s -o test
  ./test
  exit_code=$?
  if [ $exit_code -eq $expected ] ; then
    echo "exit code test succeeded, got=${expr}"
  else
    echo "exit code test failed expected=${expected}, got=${exit_code}"
  fi
  rm -rf test
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

make clean
