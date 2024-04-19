make
time (for i in $(seq 1); do cat data/input1.stream | ./main 5 > data/output2.log; done)