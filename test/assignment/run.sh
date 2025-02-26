make -B test && 

./test 1000000 | tee logs/exp_1M.log
./test 2000000 | tee logs/exp_2M.log
./test 4000000 | tee logs/exp_4M.log
./test 8000000 | tee logs/exp_8M.log
./test 16000000 | tee logs/exp_16M.log
./test 32000000 | tee logs/exp_32M.log
./test 64000000 | tee logs/exp_64M.log
./test 128000000 | tee logs/exp_128M.log
./test 256000000 | tee logs/exp_256M.log
./test 512000000 | tee logs/exp_512M.log
