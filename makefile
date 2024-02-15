debug:
	gcc ./source/main.c -Wall -Wextra -fsanitize=address -g -o ./../frost.elf

release:
	gcc ./source/main.c -Wall -Wextra -o ./../frost.elf
