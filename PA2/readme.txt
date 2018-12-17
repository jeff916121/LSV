1. Use Brute force approach to do Sat solve according to all input minterms and generate all onset minterms.
2. Use Quine-McCluskey method to generate all primes.
3. 印出每個minterm時，會比對所有prime裡面有哪些cover這個minterm

指令:
1.read XXXX.blif 讀進blif
ex:read pa2.blif
2.priosat -k 數字
ex: priosat -k 3