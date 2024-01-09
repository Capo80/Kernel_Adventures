# AFL++

Trying to understand how this works

Coverage saned in hash_map in shared memory saved here:

https://github.com/AFLplusplus/AFLplusplus/blob/stable/include/forkserver.h#L103

run of test case happens here:

https://github.com/AFLplusplus/AFLplusplus/blob/108fb0b29ad1586e668ba23e23a0eb1a13c45c49/src/afl-fuzz-run.c#L839C26-L839C26

checks if interesting:

https://github.com/AFLplusplus/AFLplusplus/blob/108fb0b29ad1586e668ba23e23a0eb1a13c45c49/src/afl-fuzz-bitmap.c#L456

classify counts normalizes the result if we are looking at new edges only or we want count also how many edges we have:

https://github.com/AFLplusplus/AFLplusplus/blob/stable/src/afl-tmin.c#L153

this actually checks if there are new paths:

https://github.com/AFLplusplus/AFLplusplus/blob/108fb0b29ad1586e668ba23e23a0eb1a13c45c49/src/afl-fuzz-bitmap.c#L205

shared memory with hashmap initialized here here:

https://github.com/AFLplusplus/AFLplusplus/blob/108fb0b29ad1586e668ba23e23a0eb1a13c45c49/src/afl-fuzz.c#L2203C25-L2203C25

sharem mem functoin here:

https://github.com/AFLplusplus/AFLplusplus/blob/108fb0b29ad1586e668ba23e23a0eb1a13c45c49/src/afl-fuzz.c#L2203C25-L2203C25

mmap happens here:

https://github.com/AFLplusplus/AFLplusplus/blob/108fb0b29ad1586e668ba23e23a0eb1a13c45c49/src/afl-fuzz.c#L2203C25-L2203C25


## commands

```
sudo docker run -ti -v /home/user/fuzz_programs:/src aflplusplus/aflplusplus

sudo docker run -ti -v /home/user/fuzz_programs:/src vanhauser/afl-dyninst
```

fucking dyninst:
```
export DYNINSTAPI_RT_LIB=/opt/dyninst/lib/libdyninstAPI_RT.so

export LD_LIBRARY_PATH="/opt/dyninst/lib:$LD_LIBRARY_PATH"
export LD_LIBRARY_PATH="/opt/oneTBB/lib:$LD_LIBRARY_PATH"

export LD_LIBRARY_PATH="/opt/page_coverage/user/afl-dyninst:$LD_LIBRARY_PATH"


export AFL_SKIP_BIN_CHECK=1

export AFL_DEBUG=1
```

instrument woth page coverage:
```

```

it just wont die:
```
kill -9 $(pgrep -f afl-fuzz)
```

general:
```
afl-fuzz -E 1 -i /home/user/fuzz_programs/INPUTS/ -o /home/user/fuzz_programs/OUTPUTS_TEST -- /home/user/fuzz_programs/tar_fork_pg -xvf @@ -C tar_extracted/


afl-showmap -e -i queue/ -o test -- /home/user/fuzz_programs/tar_llvm -xvf @@ -C tar_extracted/

afl-showmap -C -i /home/user/fuzz_programs/OUTPUTS_PG_BASE/ -o /home/user/fuzz_programs/coverage/pg_block -- /home/user/fuzz_programs/tar_llvm -xvf @@ -C tar_extracted/
```

maybe useful:
```
export AFL_NO_FORKSRV=1
```


results:
```
6 minutes PG BASE (BLOCK ONLY): 
[+] Captured 862 tuples (map size 65536, highest value 255, total values 355429) in '/home/user/fuzz_programs/coverage/pg_block'.
[+] A coverage of 862 edges were achieved out of 65536 existing (1.32%) with 68 input files.

6 minutes DYNINST (EDGE):
[+] Captured 933 tuples (map size 65536, highest value 255, total values 1103503) in '/home/user/fuzz_programs/coverage/inst_edge'.
[+] A coverage of 933 edges were achieved out of 65536 existing (1.42%) with 209 input files.
user@fuzzer:~/fuzz_programs/coverage$

6 minutes PG BASE (EDGE): 
[+] Captured 795 tuples (map size 65536, highest value 210, total values 136848) in '/home/user/fuzz_programs/coverage/page_edge'.
[+] A coverage of 795 edges were achieved out of 65536 existing (1.21%) with 25 input files.
```

results:
```
DYN 8 HOURS:
[+] Captured 1025 tuples (map size 65536, highest value 255, total values 1997828) in '/home/user/fuzz_programs/coverage/dyn_8_hours'.
[+] A coverage of 1025 edges were achieved out of 65536 existing (1.56%) with 341 input files.


```