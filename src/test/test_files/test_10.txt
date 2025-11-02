#!/usr/bin/python3
import os
from colors import Color
from test_runner import run as run_func_test
from valgrind_test_runner import run as run_mem_test

cwd = os.getcwd()
print(f"{Color.BLUE}Running functional tests on CAT:{Color.NC}")
run_func_test("./s21_cat", "cat", "cat_tests")

print(f"{Color.BLUE}Running functional tests on GREP:{Color.NC}")
os.chdir(cwd)
run_func_test("./s21_grep", "grep", "grep_tests")

print(f"{Color.BLUE}Running memory tests on CAT:{Color.NC}")
os.chdir(cwd)
run_mem_test("./s21_cat", "cat_tests")

print(f"{Color.BLUE}Running memory tests on GREP:{Color.NC}")
os.chdir(cwd)
run_mem_test("./s21_grep", "grep_tests")