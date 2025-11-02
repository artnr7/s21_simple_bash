GCC = gcc -Wall -Werror -Wextra -std=c11 -pedantic -std=gnu11
CATFILES = cat.c

.PHONY: all r s21_cat s21_cat_test f c clean

all: f s21_cat

s21_cat:
	@$(GCC) $(CATFILES) -o s21_cat
	@echo 's21 cat is installed'

# TEST ---------------------->
t: s21_cat_test
val: s21_cat_test_valgrind


s21_cat_test:
	@echo "s21_cat test is running..."
	@chmod +x ../test/test.sh
	@./../test/test.sh cat ./s21_cat

s21_cat_test_valgrind:
	@echo "s21_cat test is running..."
	@chmod +x ../test/test.sh
	@./../test/test.sh cat ./s21_cat leaks full

# FORMAT ---------------------->
f: shfmt c

shfmt:
	@shfmt -w ../test/test.sh
	@echo "shfmt done"

c:
	@cp ../../materials/linters/.clang-format .
	@clang-format -i $(CATFILES)
	@rm -f .clang-format
	@echo "clang-format done"

cn:
	@cp ../../materials/linters/.clang-format .
	@clang-format -n $(CATFILES)
	@rm -f .clang-format
	@echo "clang-format checked"

# REBUILD ---------------------->
r: f clean s21_cat

# CLEAN ---------------------->
clean:
	@rm -f s21_cat ../grep/s21_grep ../test/log.log
	@echo "all clean"