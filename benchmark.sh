make
time (for i in $(seq 50); do cat data/input2.stream | ./main 5 >/dev/null ; done)