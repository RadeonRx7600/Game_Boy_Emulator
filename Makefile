NAME = Gameboy

CC = gcc
CFLAGS = -Wall -Wextra -Werror -Iinclude

SRC = GB/Core.c \
      GB/Cartridges.c \
      GB/CPU.c \
      GB/Memory.c \
      GB/Opcodes_table.c \
      GB/Op_prefCB.c

OBJ = $(SRC:GB/%.c=Build/%.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

Build/%.o: GB/%.c
	@mkdir -p Build
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re

