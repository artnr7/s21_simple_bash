# Instructions

1. Для запуска тестов используйте:  
`./test.sh <compare_app> ./{path_to_executable}`  
   Где **<compare_app>** – это утилита для сравнения (cat или grep).  
   **./{path_to_executable}** – путь к вашему исполняемому файлу  

2. Программа может быть поставлена на паузу через **Ctrl+c**.

3. Программа может быть включена вместе с использованием тройных, четверных, ... комбинаций, если запустить скрипт с аргументом 3:  
   `bash cat_test.sh <compare_file> ./{path_to_executable} 3`

4. Программа может автоматически останавливаться после каждых 500 тестов, если запустить скрипт с аргументом 1:  
   `bash cat_test.sh <compare_file> ./{path_to_executable} 1`

5. Программа может тестировать только конкретно твои флаги, если запустить скрипт с аргументом 2 и флагами, пример:  
   `bash cat_test.sh <compare_file> ./{path_to_executable} 2 ve`  
   Где ve - это флаги, которые будут протестированы.

6. Программа может проверять на утечки через valgrind с минимальным выводом, если запустите скрипт с аргументом leaks:  
   `bash cat_test.sh <compare_file> ./{path_to_executable} leaks`  
   или с полным выводом –  
   `bash cat_test.sh <compare_file> ./{path_to_executable} leaks full`

7. Программа может комбинировать аргументы leaks и full с аргументами 1, 2, 3 и flags. Для этого перед аргументами 1, 2, 3 и flags пропишите аргументы **leaks/leaks full**:  
   Примеры:  
   `bash cat_test.sh <compare_file> ./{path_to_executable} leaks full 2`  
   `bash cat_test.sh <compare_file> ./{path_to_executable} leaks 1`
