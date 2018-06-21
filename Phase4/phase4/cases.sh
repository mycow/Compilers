./scc < examples/expr.c >examples/expr_Mine.log 2> examples/expr_Mine.err
./scc < examples/math.c >examples/math_Mine.log 2> examples/math_Mine.err
./scc < examples/stmt.c >examples/stmt_Mine.log 2> examples/stmt_Mine.err
./scc < examples/tree.c >examples/tree_Mine.log 2> examples/tree_Mine.err

if diff examples/expr.err examples/expr_Mine.err; then
  echo Expression:   Passed ✅
else
  echo Expression:   Failed ❌
fi

if diff examples/math.err examples/math_Mine.err; then
  echo Math:   Passed ✅
else
  echo Math:   Failed ❌
fi

if diff examples/stmt.err examples/stmt_Mine.err; then
  echo Statement:   Passed ✅
else
  echo Statement:   Failed ❌
fi

if diff examples/tree.err examples/tree_Mine.err; then
  echo Tree:   Passed ✅
else
  echo Tree:   Failed ❌
fi
