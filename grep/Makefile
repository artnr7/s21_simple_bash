GCC = gcc -Wall -Werror -Wextra -std=c11 -pedantic -std=gnu11
GREPFILES = grep.c

.PHONY: all r s21_grep s21_grep_test f c clean

all: f s21_grep 

s21_grep:
	@$(GCC) $(GREPFILES) -o s21_grep
	@echo 's21_grep is installed'

# TEST ---------------------->
t: s21_grep_test
val: s21_grep_test_valgrind

s21_grep_test:
	@echo "s21_grep test is running..."
	@chmod +x ../test/test.sh
	@./../test/test.sh grep ./s21_grep

s21_grep_test_valgrind:
	@echo "s21_grep test is running..."
	@chmod +x ../test/test.sh
	@./../test/test.sh grep ./s21_grep leaks full

# FORMAT ---------------------->
f: shfmt c

shfmt:
	@shfmt -w ../test/test.sh
	@echo "shfmt done"

c:
	@cp ../../materials/linters/.clang-format .
	@clang-format -i $(GREPFILES)
	@rm -f .clang-format
	@echo "clang-format done"

cn:
	@cp ../../materials/linters/.clang-format .
	@clang-format -n $(GREPFILES)
	@rm -f .clang-format
	@echo "clang-format checked"

# REBUILD ---------------------->
r: f clean s21_grep

# CLEAN ---------------------->
clean:
	@rm -f s21_grep ../cat/s21_cat ../test/log.log
	@echo "all clean"