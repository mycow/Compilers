./scc < examples/array.c > outputs/array.txt 2> outputs/array_log.txt
./scc < examples/fib.c > outputs/fib.txt 2> outputs/fib_log.txt
./scc < examples/hello.c > outputs/hello.txt 2> outputs/hello_log.txt
./scc < examples/malloc.c > outputs/malloc.txt 2> outputs/malloc_log.txt //caught in parameter
./scc < examples/params.c > outputs/params.txt 2> outputs/params_log.txt
./scc < examples/sum.c > outputs/sum.txt 2> outputs/sum_log.txt

if diff examples/array.out outputs/array.txt; then
  echo Array:   Passed ✅
else
  echo Array:   Failed ❌
fi

if diff examples/fib.out outputs/fib.txt; then
  echo Fib:   Passed ✅
else
  echo Fib:   Failed ❌
fi

if diff examples/hello.out outputs/hello.txt; then
  echo Hello:   Passed ✅
else
  echo Hello:   Failed ❌
fi

if diff examples/malloc.out outputs/malloc.txt; then
  echo Malloc:   Passed ✅
else
  echo Malloc:   Failed ❌
fi

if diff examples/params.out outputs/params.txt; then
  echo Params:   Passed ✅
else
  echo Params:   Failed ❌
fi

if diff examples/sum.out outputs/sum.txt; then
  echo Sum:   Passed ✅
else
  echo Sum:   Failed ❌
fi
