#!/bin/bash

# Color codes
# ------------------------------------------------------------------------------------------------------->
RED='\033[31m'
GREEN='\033[32m'
BLUE='\033[34m'
YELLOW='\033[33m'
RESET='\033[0m'

##########################################################################################################

success_test_count=0
all_test_cnt=0
flags_string=""
test_dir="../test/test_files"
# IFS=' ' read -r -a flags_list <<< "$flags_string"
batch_mode=false
exec_file=""
grep_template=""
test_filess=(
	"$test_dir/test_0.txt"
	"$test_dir/test_1.txt"
	"$test_dir/test_2.txt"
	"$test_dir/test_3.txt"
	"$test_dir/test_4.txt"
	"$test_dir/test_5.txt"
	"$test_dir/test_6.txt"
	"$test_dir/test_7.txt"
	"$test_dir/test_8.txt"
	"$test_dir/test_9.txt"
	"$test_dir/test_10.txt"
	"$test_dir/test_11.txt"
	"$test_dir/test_12.txt"
	"$test_dir/test_13.txt"
	"$test_dir/test_14.txt"
	"$test_dir/test_0.txt $test_dir/test_0.txt"
	"$test_dir/test_1.txt $test_dir/test_1.txt"
	"$test_dir/test_2.txt $test_dir/test_2.txt"
	"$test_dir/test_3.txt $test_dir/test_3.txt"
	"$test_dir/test_4.txt $test_dir/test_4.txt"
	"$test_dir/test_5.txt $test_dir/test_5.txt"
	"$test_dir/test_6.txt $test_dir/test_6.txt"
	"$test_dir/test_7.txt $test_dir/test_7.txt"
	"$test_dir/test_8.txt $test_dir/test_8.txt"
	"$test_dir/test_9.txt $test_dir/test_9.txt"
	"$test_dir/test_10.txt $test_dir/test_10.txt"
	"$test_dir/test_11.txt $test_dir/test_11.txt"
	"$test_dir/test_12.txt $test_dir/test_12.txt"
	"$test_dir/test_13.txt $test_dir/test_13.txt"
	"$test_dir/test_14.txt $test_dir/test_14.txt"
)

# Ctrl + C trap
# ------------------------------------------------------------------------------------------------------->
trap 'handle_interrupt' SIGINT

handle_interrupt() {
	# echo ""
	# echo "Testing interrupted. Do you want to continue? (y/n)"
	# read -r answer
	# if [[ "$answer" == "n" ]]; then
	rm -f 1.txt 2.txt
	echo ""
	echo "Testing terminated."
	echo -e "Success: ${GREEN}$success_test_count${RESET} / ${BLUE}$all_test_cnt.${RESET}"
	exit 0
	# fi
}

##########################################################################################################

# --help
# ------------------------------------------------------------------------------------------------------->
show_help() {
	echo -e "${RED}Прочитай README.txt${RESET}"
	echo -e "${BLUE}Usage: $0 <compare_app> <path_to_executable> [leaks] [full] [0|1|2] [flags]${RESET}"
	echo ""
	echo -e "${YELLOW}Arguments:${RESET}"
	echo "  {compare_app}           Утилита для сравнения (cat или grep)."
	echo "  ./{path_to_executable}  Путь к исполняемому файлу, который будет тестироваться."
	echo "  leaks                   Проверка на утечки памяти с использованием Valgrind."
	echo "  leaks full              Полный вывод отчета Valgrind."
	echo "  1                       Автоматическая остановка после каждых 500 тестов."
	echo "  2                       Тестирование только указанных флагов (например, ve)."
	echo "  3                       Тестирование с тройными, четверными,... комбинациями флагов"
	echo ""
	echo -e "${GREEN}Examples:${RESET}"
	echo "  bash cat_test.sh cat ./a.out"
	echo "  bash cat_test.sh grep ./a.out leaks"
	echo "  bash cat_test.sh cat ./a.out leaks full 1"
	echo "  bash cat_test.sh grep ./a.out leaks 0"
	echo ""
	echo -e "${BLUE}Вывод теста хранится в ../test/log.log${RESET}"
	echo ""
}

if [[ "$1" == "--help" ]]; then
	show_help
	exit 0
fi

##########################################################################################################

# Compare app
# ------------------------------------------------------------------------------------------------------->
compare_app="$1"
shift

use_grep=false
if [[ $compare_app != "cat" && $compare_app != "grep" ]]; then
	echo "Test: Not correct: must be 'cat' or 'grep'!"
	echo "Test: Usage: $0 {compare_app} ./{path_to_executable} [leaks] [full] [0|1|2] [flags]."
	echo -e "Test: ${RED}Use --help for more information.${RESET}"
	exit 1
elif [[ $compare_app == "cat" ]]; then
	flags_string="b e n s t v"
elif [[ $compare_app == "grep" ]]; then
	flags_string="i v c l n h e s o"
	grep_template="char"
	temp_filess=("${test_filess[@]}")
	test_filess=("${temp_filess[@]:0:14}" "${temp_filess[@]:15:14}")
	use_grep=true
fi

IFS=' ' read -r -a flags_list <<<"$flags_string"

if [[ $# -lt 1 ]]; then
	echo "Test: Error: Path to the executable file must be provided."
	echo "Test: Usage: $0 {compare_app} ./{path_to_executable} [leaks] [full] [0|1|2] [flags]."
	echo -e "Test: ${RED}Use --help for more information.${RESET}"
	exit 1
fi

##########################################################################################################

# Exec file
# ------------------------------------------------------------------------------------------------------->

exec_file="$1"
shift

# if [[ ! -x "$exec_file" ]]; then
#     echo "Error: The specified file '$exec_file' is not executable or does not exist."
#     exit 1
# fi

##########################################################################################################

# Modes
# ------------------------------------------------------------------------------------------------------->
use_full_valgrind=false
use_valgrind=false
use_more=false
if [[ "$1" == "leaks" ]]; then
	use_valgrind=true
	shift

	if [[ "$1" == "full" ]]; then
		use_full_valgrind=true
		shift
	fi
fi

if [[ "$1" == "1" ]]; then
	batch_mode=true
	echo "Batch mode enabled: testing will pause every 500 tests."
	shift
elif [[ "$1" == "2" ]]; then
	if [[ -z "$2" ]]; then
		echo "Error: No flags provided for custom testing mode."
		exit 1
	fi
	flags_string="$2"
	IFS=' ' read -r -a flags_list <<<"$flags_string"
	shift 2
elif [[ "$1" == "3" ]]; then
	use_more=true
fi

rm -f ../test/log.log

##########################################################################################################

# Testing
# ------------------------------------------------------------------------------------------------------->
tests() {
	local flag="$1"
	local reg_file="$2"
	local test_number=-1
	local test_cnt=0

	echo " "
	echo -e ${BLUE} "Flag: -$flag:${RESET}"
	echo " " >>../test/log.log
	echo "###########" >>../test/log.log
	echo "Flag: -$flag:" >>../test/log.log
	echo "###########" >>../test/log.log

	for test_files in "${test_filess[@]}"; do
		all_test_cnt=$((all_test_cnt + 1))
		test_number=$((test_number + 1))

		if $use_valgrind; then
			valgrind_output=$(valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes $exec_file -$flag $reg_file $grep_template $test_files 2>&1 | tr -d '\0')
			error_summary_count=$(echo "$valgrind_output" | grep -c "ERROR SUMMARY: 0")

			if [ $error_summary_count -eq 0 ]; then
				echo -e "Test $test_number: ${RED}ERROR${RESET} ${RED}LEAKS${RESET}"
				echo -e "Test $test_number: ERROR LEAKS" >>../test/log.log
				echo -e "${YELLOW}valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes $exec_file -$flag $reg_file $grep_template $test_files${RESET}"
				echo -e "valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes $exec_file -$flag $reg_file $grep_template $test_files" >>../test/log.log
				if $use_full_valgrind; then
					echo "$valgrind_output"
					echo "$valgrind_output" >>../test/log.log
				fi
				continue
			fi
		fi

		$exec_file -$flag $reg_file $grep_template $test_files >1.txt
		$compare_app -$flag $reg_file $grep_template $test_files >2.txt

		if cmp -s 1.txt 2.txt; then
			success_test_count=$((success_test_count + 1))
			test_cnt=$((test_cnt + 1))
			echo -e "Test $test_number: ${GREEN}OK${RESET}"
			echo -e "Test $test_number: OK" >>../test/log.log
		else
			echo -e "Test $test_number: ${RED}ERROR${RESET}"
			echo -e "Test $test_number: ERROR" >>../test/log.log
			echo "$exec_file -$flag $reg_file $grep_template $test_files"
			echo "$exec_file -$flag $reg_file $grep_template $test_files" >>../test/log.log
		fi

		# Batch mode
		if $batch_mode && ((all_test_cnt % 500 == 0 && all_test_cnt != 0)); then
			echo "Executed $all_test_cnt tests. Do you want to continue? (y/n)"
			read -r answer
			if [[ "$answer" == "n" ]]; then
				echo ""
				echo "Testing terminated."
				echo -e "Success: ${GREEN}$success_test_count${RESET} / ${BLUE}$all_test_cnt.${RESET}"
				exit 0
			fi
		fi
	done

	rm -f 1.txt 2.txt
	# if [[ "$compare_app" == "grep" && "$test_cnt" -eq 28 ]]; then
	# 	echo -e "${GREEN}WORK${RESET}"
	# elif [[ "$compare_app" == "cat" && "$test_cnt" -eq 30 ]]; then
	# 	echo -e "${GREEN}WORK${RESET}"
	# fi
}

# Начало скрипта
# ------------------------------------------------------------------------------------------------------->
# echo "README in the dir"
# echo "Вывод тестов хранится в ../test/log.log!"
# echo "Готов? Жми Enter!"
# read -r otv

for x in "${flags_list[@]}"; do
	tests "$x"
done

if [ "$compare_app" == "grep" ]; then
	for x in "${flags_list[@]}"; do
		for y in "${flags_list[@]}"; do
			if [[ "$x" != "$y" ]]; then
				flag="$x$y"
				tests "$flag"
			fi
		done
	done
fi

grep_template=""
if $use_grep; then
	f="f"
	for x in "${flags_list[@]}"; do
		flag="$x$f"
		tests "$flag" "$test_dir/reg.txt"
	done
	grep_template="char"
fi

# Triple, quadruple flag combinations
# ------------------------------------------------------------------------------------------------------->
generate_combinations() {
	local current_combination="$1"
	local remaining_flags="$2"

	if [[ -n "$current_combination" ]]; then
		tests "$current_combination"
	fi

	for flag in $(echo "$remaining_flags" | grep -o .); do
		if [[ "$current_combination" != *"$flag"* ]]; then
			generate_combinations "$current_combination$flag" "$remaining_flags"
		fi
	done
}

if $use_more; then
	generate_combinations "" "$flags_string"
fi

##########################################################################################################

echo ""
echo "Testing completed."
echo -e "Successfully: ${GREEN}$success_test_count${RESET} / ${BLUE}$all_test_cnt${RESET}"
echo ""
