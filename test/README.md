# Match tests

AI-generated test runner.

Run all tests against a built compiler:

```bat
python test\run.py --compiler build\spark-compiler.exe
```

Each `.test` file consists of CLI options, source code, and either expected output or an expected failure:

```text
#cli
--emit skr --optimize dce

#src
fun main(): int {
    return 1 + 0;
}

#expect
fun main(): int
    ...
```

The runner supplies `--code` itself and rejects both `--src` and `--code` in `#cli`. The compiler still runs in a temporary working directory, so binary-output tests do not create artifacts in the repository. For a negative test, replace `#expect` with `#expect_failure`.
