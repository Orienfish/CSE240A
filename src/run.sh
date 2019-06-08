make clean;make;
setting=$1
file=$2
bunzip2 -kc ../traces/fp_1.bz2 | ./predictor $setting >> ./memory/$file
bunzip2 -kc ../traces/fp_2.bz2 | ./predictor $setting >> ./memory/$file
bunzip2 -kc ../traces/int_1.bz2 | ./predictor $setting >> ./memory/$file
bunzip2 -kc ../traces/int_2.bz2 | ./predictor $setting >> ./memory/$file
bunzip2 -kc ../traces/mm_1.bz2 | ./predictor $setting >> ./memory/$file
bunzip2 -kc ../traces/mm_2.bz2 | ./predictor $setting >> ./memory/$file
