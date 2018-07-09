exit_code() {
  expr=$1
  expected=$2
  gcc -o compiler main.c
  echo $expr | ./compiler > test.s
  gcc test.s -o test
  ./test
  exit_code=$?
  if [ $exit_code -eq $expected ] ; then
    echo 'exit code test succeeded'
  else
    echo "exit code test failed expected=${expected}, got=${exit_code}"
  fi
  rm -rf test.s test
}

exit_code '1+2' 3
exit_code '12+34' 46
exit_code '1+2+3' 6
exit_code '11+22+33' 66
