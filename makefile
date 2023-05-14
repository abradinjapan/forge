debug:
	gcc ./source/main.c -Wall -Wextra -fsanitize=address -g -o ./binary/forge.elf

release:
	gcc ./source/main.c -Wall -Wextra -o ./binary/forge.elf
