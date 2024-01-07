rm -rf ./*.o ./*.a ./*.so

icc  hook.c hash_table.c mem_pool.c  -c
ar -r libhook.a hook.o hash_table.o mem_pool.o

mpiicc  mpiwrapper.c -finstrument-functions -c -o mpiwrapper.o
ar -r libmpiwrapper.a mpiwrapper.o
