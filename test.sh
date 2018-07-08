exit_code() {
  expected=$1
  gcc -o compiler main.c
  echo $expected | ./compiler > test.s
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

exit_code 2
exit_code 22
exit_code 127
