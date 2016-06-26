#! /bin/bash

model=$1
duration=$2

filename=$(basename $model)
model_name=${filename%.*}

for level in `seq 1 6`
do
	./subd-bin -f $model -l $level -m $duration > "subd_${model_name}_${level}.csv"
done
