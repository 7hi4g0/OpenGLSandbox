model=$1

filename=$(basename $model)
model_name=${filename%.*}

for level in `seq 1 6`
do
	./subd-bin -f $model -l $level -w subd_${model_name}_${level}.ppm
done
