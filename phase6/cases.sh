./scc < examples/acid.c > examples/acid.s
gcc -o acid examples/acid.s
./acid > examples/acid-mine.out

if diff examples/acid.out examples/acid-mine.out; then
    echo Acid: Passed✅
else 
    echo Acid: Failed❌
fi

./scc < examples/calc.c > examples/calc.s
gcc -o calc examples/calc.s
./calc > examples/calc-mine.out

if diff examples/calc.out examples/calc-mine.out; then
    echo Calc: Passed✅
else 
    echo Calc: Failed❌
fi

./scc < examples/fib.c > examples/fib.s
gcc -o fib examples/fib.s
./fib > examples/fib-mine.out

if diff examples/fib.out examples/fib-mine.out; then
    echo Fib: Passed✅
else 
    echo Fib: Failed❌
fi

./scc < examples/global.c > examples/global.s
gcc -o global examples/global.s
./global > examples/global-mine.out

if diff examples/global.out examples/global-mine.out; then
    echo Global: Passed✅
else 
    echo Global: Failed❌
fi

./scc < examples/hello.c > examples/hello.s
gcc -o hello examples/hello.s
./hello > examples/hello-mine.out

if diff examples/hello.out examples/hello-mine.out; then
    echo Hello: Passed✅
else 
    echo Hello: Failed❌
fi

./scc < examples/int.c > examples/int.s
gcc -o int examples/int.s
./int > examples/int-mine.out

if diff examples/int.out examples/int-mine.out; then
echo Int: Passed✅
else
echo Int: Failed❌
fi

./scc < examples/matrix.c > examples/matrix.s
gcc -o matrix examples/matrix.s
./matrix > examples/matrix-mine.out

if diff examples/matrix.out examples/matrix-mine.out; then
echo Matrix: Passed✅
else
echo Matrix: Failed❌
fi

./scc < examples/mixed.c > examples/mixed.s
gcc -o mixed examples/mixed.s
./mixed > examples/mixed-mine.out

if diff examples/mixed.out examples/mixed-mine.out; then
echo Mixed: Passed✅
else
echo Mixed: Failed❌
fi

./scc < examples/qsort.c > examples/qsort.s
gcc -o qsort examples/qsort.s
./qsort > examples/qsort-mine.out

if diff examples/qsort.out examples/qsort-mine.out; then
echo Qsort: Passed✅
else
echo Qsort: Failed❌
fi

./scc < examples/tree.c > examples/tree.s
gcc -o tree examples/tree.s
./tree > examples/tree-mine.out

if diff examples/tree.out examples/tree-mine.out; then
echo Tree: Passed✅
else
echo Tree: Failed❌
fi
