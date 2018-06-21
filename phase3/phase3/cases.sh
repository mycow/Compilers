./scc < examples/conflicting.c >examples/conflicting_Mine.log 2> examples/conflicting_Mine.err
./scc < examples/nesting.c >examples/nesting_Mine.log 2> examples/nesting_Mine.err
./scc < examples/previously.c >examples/previously_Mine.log 2> examples/previously_Mine.err
./scc < examples/undeclared.c >examples/undeclared_Mine.log 2> examples/undeclared_Mine.err

if diff examples/conflicting.err examples/conflicting_Mine.err; then
  echo Conflicting:   Passed ✅
else
  echo Conflicting:   Failed ❌
fi

if diff examples/nesting.err examples/nesting_Mine.err; then
  echo Nesting:   Passed ✅
else
  echo Nesting:   Failed ❌
fi

if diff examples/previously.err examples/previously_Mine.err; then
  echo Previously:   Passed ✅
else
  echo Previously:   Failed ❌
fi

if diff examples/undeclared.err examples/undeclared_Mine.err; then
  echo Undeclared:   Passed ✅
else
  echo Undeclared:   Failed ❌
fi
