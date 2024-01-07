export LD_LIBRARY_PATH=/public1/home/fio_climate_model/esm_liuyao/my-c-hashlib/latest-wys-timer/lib
export LD_LIBRARY_PATH=/public1/soft/intel/2017/compilers_and_libraries/linux/lib/intel64:$LD_LIBRARY_PATH

rm -rf ./a.out
mpiicpc testMpi.cpp   -finstrument-functions -no-pie  -g -L/public1/home/fio_climate_model/esm_liuyao/my-c-hashlib/latest-wys-timer/lib -lmpiwrapper -lhook
./a.out
