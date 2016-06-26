#! /bin/bash

model=$1

filename=$(basename $model)
model_name=${filename%.*}

for level in `seq 2 6`
do
	output="gpusubd_${model_name}_${level}_space.csv"

	> $output

	./gpusubd-bin -f $model -l $level -s | tee -a $output
done
