model=$1

filename=$(basename $model)
model_name=${filename%.*}

for level in `seq 2 6`
do
	./gpusubd-bin -f $model -l $level -w gpusubd_${model_name}_${level}.ppm
done
