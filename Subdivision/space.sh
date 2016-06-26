#! /bin/bash

model=$1

filename=$(basename $model)
model_name=${filename%.*}

output="subd_${model_name}_space.csv"

> $output

for level in `seq 1 6`
do
	./subd-bin -f $model -l $level -s | tee -a $output
done
