# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: 개포동블루벨벳                                 +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/06/12 18:05:34 by 개포동블루벨벳        #+#    #+#              #
#    Updated: 2023/06/12 19:42:55 by 개포동블루벨벳       ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# ---- NAME ---- #

NAME		=	webserv

CC			=	c++
STD			=	-std=c++98
CFLAGS		=	-Wall -Wextra -Werror $(STD) $(INCLUDE)
DBGS		=	-fsanitize=address -g3

RM			=	rm -rf

OBJ_DIR	=	./objects
SRCS_DIR = ./srcs

# ---- INCLUDE ---- #

INCLUDE_DIRS = $(shell find $(SRCS_DIR) -type d -name "include")
INCLUDE = $(patsubst %, -I %, $(INCLUDE_DIRS))

# ---- TEST ---- #

EXEC		=	webserv
TEST		=	test

# ---- escape ---- #

DELINE = \033[K
CURSUP = \033[A

RESET = \033[0m
RESTINT = \033[22m

BOLD = \033[1m

MAGENTA = \033[35m
GREEN = \033[32m
RED = \033[31m

# ---- Mandatory ---- #

sources1 :=	main.cpp

# ---- Utils ---- #

sources1 += FileChecker.cpp \
						Reader.cpp \
						Utils.cpp \
						Status.cpp

# ---- Network ---- #

sources1 +=	Client.cpp

sources1 +=	ServerManager.cpp \
						Server.cpp \
						Kqueue.cpp \
						EventHandler.cpp

# ---- Method ---- #
sources1 +=	AMethod.cpp \
						GET.cpp \
						POST.cpp \
						DELETE.cpp \
						PUT.cpp \
						DummyMethod.cpp

# ---- Config ---- #

sources1 += Config.cpp \
						ConfigParser.cpp \
						RootConfig.cpp \
						ProxyConfig.cpp \
						MimeTypesConfig.cpp \
						ServerConfig.cpp \
						LocationConfig.cpp

# ---- Exception ---- #

sources1 +=	ExceptionThrower.cpp


# ---- SRC ---- #

SRCS = $(shell for name in $(sources1); do find $(SRCS_DIR) -name $$name; done)
SRCSS = $(patsubst ./%,%,$(SRCS))

SRCS_BONUS = $(shell for name in $(sources2); do find $(SRCS_DIR) -name $$name; done)
SRCS_BONUS := $(patsubst ./%,%,$(SRCS_BONUS))

# ---- Elements ---- #

all_sources = $(sources1) $(sources2)
objects1 = $(SRCSS:.cpp=.o)
objects2 = $(SRCS_BONUS:.cpp=.o)
all_objects = $(objects1) $(objects2)

define objects_goal
$(addprefix $(OBJ_DIR)/, $(call $(if $(filter bonus, $(MAKECMDGOALS)), objects2, objects1))) 
endef

define react
$(if $(filter bonus, $(MAKECMDGOALS)), bonus, all)
endef

# ---- Command ---- #

.PHONY : all bonus clean fclean re test

all : $(NAME)

$(NAME) : $(objects_goal)
	@$(CC) $(CFLAGS) -o $(NAME) $(objects_goal)
	@echo "$(DELINE) $(MAGENTA)$@ $(RESET) is compiled $(GREEN)$(BOLD) OK ✅ $(RESET)"

bonus : $(NAME)

$(OBJ_DIR)/%.o : %.cpp
	@mkdir -p $(OBJ_DIR)/$(dir $^)
	@$(CC) $(CFLAGS) -c $^ -o $@
	@echo " $(MAGENTA)$(NAME) $(RESET)objects file compiling... $(DELINE)$(GREEN) $^ $(RESET)$(CURSUP)"

clean :
	@$(RM) $(all_objects)
	@rm -rf $(OBJ_DIR)
	@echo "$(RED) Delete$(BOLD) objects $(RESTINT)file $(RESET)"

fclean : clean
	@$(RM) $(NAME)
	@$(RM) $(TEST)
	@echo "$(RED) Delete$(BOLD) $(NAME) $(RESTINT)file $(RESET)"

re : fclean
	@make $(react)
	
# ---- Test ---- #

leaks: export MallocStackLogging=1
leaks: CFLAGS += -D LEAKS -g3
leaks: fclean all
	./$(EXEC)
leaks: export MallocStackLogging=0

dbg: CFLAGS += -fsanitize=address -g3 -D DEBUG_MSG
dbg: fclean all
	./$(EXEC)

dbg!: CFLAGS += -fsanitize=address -g3 -D DEBUG_MSG
dbg!: all
	./$(EXEC)

# test: CFLAGS += -D PORT=8080
test: all
	./$(EXEC)

# test1: CFLAGS += -D PORT=8081
test1: fclean all
	./$(EXEC)
	
# test2: CFLAGS += -D PORT=8082
test2: fclean all
	./$(EXEC)