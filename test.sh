exit_code() {
  expected=$1
  gcc -o compiler main.c
  echo $expected | ./compiler
  exit_code=$?
  if [ $exit_code -eq $expected ] ; then
    echo 'exit code test succeeded'
  else
    echo "exit code test failed expected=${expected}, got=${exit_code}"
  fi
}

exit_code 2
