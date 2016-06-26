#! /bin/bash

model=$1
quant=$2

filename=$(basename $model)
model_name=${filename%.*}

for level in `seq 1 6`
do
	output="subd_${model_name}_${level}_time_${quant}.csv"

	> $output
	echo $output

	for i in `seq 1 $quant`
	do
		./subd-bin -f $model -l $level -t | tee -a $output
	done
done
