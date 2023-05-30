debug:
	g++ ./source/main.c -Wall -Wextra -fsanitize=address -g -o ./binary/forge.elf

release:
	g++ ./source/main.c -Wall -Wextra -o ./binary/forge.elf
