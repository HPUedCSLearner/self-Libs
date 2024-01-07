rm -rf ./*.o ./*.a ./*.so

icc -shared -fPIC hook.c hash_table.c mem_pool.c  -o libhook.so
mpiicc  mpiwrapper.c -finstrument-functions -c -o mpiwrapper.o
ar -r libmpiwrapper.a mpiwrapper.o
