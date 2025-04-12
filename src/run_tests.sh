#!/bin/bash

TESTS_ROOT="tests"
MANGEN_PATH="$(realpath ./mangen)"

cleanup_previous() {
    if [ -d "$TESTS_ROOT" ]; then
        find "$TESTS_ROOT" -type d -exec chmod 700 {} \; 2>/dev/null
        find "$TESTS_ROOT" -type f -exec chmod 600 {} \; 2>/dev/null
        rm -rf "$TESTS_ROOT"
    fi
}

cleanup_previous
mkdir -p "$TESTS_ROOT"
[ ! -f "$MANGEN_PATH" ] && { echo "Error: 'mangen' not found"; exit 1; }

create_test_env() {
    local test_dir="$1"
    
    mkdir -p "$test_dir"/{dir1,dir2/subdir}
    touch "$test_dir"/file1.txt \
          "$test_dir"/dir1/file2.txt \
          "$test_dir"/dir2/subdir/file3.txt
}

run_test() {
    local test_name="$1"
    local command="$2"
    local expected="$3"
    
    echo -n "Testing $test_name... "
    
    actual_output=$(eval "$command" 2>&1 | grep -v '\[ERROR\]' | sort -k1)
    
    if diff -w <(echo "$expected") <(echo "$actual_output") >/dev/null; then
        echo "PASSED"
    else
        echo "FAILED"
        echo "--- Expected ---"
        echo "$expected"
        echo "--- Actual ---"
        echo "$actual_output"
        exit 1
    fi
}

current_run="$TESTS_ROOT/run_$(date +%s)"
mkdir -p "$current_run"

echo "Running tests in: $current_run"

# Тест 1: Базовый тест
(
    test_dir="$current_run/test1"
    create_test_env "$test_dir"
    expected="dir1/file2.txt : $(sha256sum "$test_dir/dir1/file2.txt" | awk '{print $1}')
dir2/subdir/file3.txt : $(sha256sum "$test_dir/dir2/subdir/file3.txt" | awk '{print $1}')
file1.txt : $(sha256sum "$test_dir/file1.txt" | awk '{print $1}')"
    run_test "basic" "$MANGEN_PATH '$test_dir'" "$expected"
)

# Тест 2: Исключение директории
(
    test_dir="$current_run/test2"
    create_test_env "$test_dir"
    expected="dir1/file2.txt : $(sha256sum "$test_dir/dir1/file2.txt" | awk '{print $1}')
file1.txt : $(sha256sum "$test_dir/file1.txt" | awk '{print $1}')"
    run_test "exclude dir" "$MANGEN_PATH '$test_dir' -e dir2" "$expected"
)

# Тест 3: Wildcard исключение
(
    test_dir="$current_run/test3"
    create_test_env "$test_dir"
    expected="file1.txt : $(sha256sum "$test_dir/file1.txt" | awk '{print $1}')"
    run_test "exclude wildcard" "$MANGEN_PATH '$test_dir' -e 'dir*'" "$expected"
)

# Тест 4: Текущая директория
(
    test_dir="$current_run/test4"
    create_test_env "$test_dir"
    cd "$test_dir"
    expected="dir1/file2.txt : $(sha256sum "dir1/file2.txt" | awk '{print $1}')
file1.txt : $(sha256sum "file1.txt" | awk '{print $1}')"
    run_test "current dir" "$MANGEN_PATH . -e dir2" "$expected"
    cd - >/dev/null
)

echo "Test data saved in: $current_run"
