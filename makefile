debug:
	gcc ./source/main.c -Wall -Wextra -fsanitize=address -g -o ./../flame.elf

release:
	gcc ./source/main.c -Wall -Wextra -o ./../flame.elf
