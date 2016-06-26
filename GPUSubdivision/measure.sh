#! /bin/bash

model=$1
duration=$2

filename=$(basename $model)
model_name=${filename%.*}

for level in `seq 2 6`
do
	./gpusubd-bin -f $model -l $level -m $duration > "gpusubd_${model_name}_${level}.csv"
done
