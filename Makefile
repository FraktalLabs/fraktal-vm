CC := clang++
# TODO: Remove Wno's
CPPFLAGS ?= -std=c++20 -Wall -Wno-for-loop-analysis -Wno-unused-variable

INTX_PATH ?= ../intx
ETHASH_PATH ?= ../ethash
PROC_EVM_PATH ?= ../proc-evm
EVM_STATE_DB_PATH ?= ../evm-state-db

INTX_LIB_PATH ?= ${INTX_PATH}/include
ETHASH_LIB_PATH ?= ${ETHASH_PATH}/include
ETHASH_LINK_PATHS ?= $(shell find ${ETHASH_PATH}/build/lib -name '*.o')
PROC_EVM_LIB_PATH ?= ${PROC_EVM_PATH}/include
#TODO: Remove external from finds after proper linking setup
PROC_EVM_LINK_PATHS ?= $(shell find ${PROC_EVM_PATH}/builds -not -path '*/external/*' -not -path '*main.cpp*' -name '*.o')
EVM_STATE_DB_LIB_PATH ?= ${EVM_STATE_DB_PATH}/include
EVM_STATE_DB_LINK_PATHS ?= $(shell find ${EVM_STATE_DB_PATH}/builds -not -path '*main.cpp*' -name '*.o')

INC_LIBS := -I ${PROC_EVM_LIB_PATH} -I ${EVM_STATE_DB_LIB_PATH} -I ${INTX_LIB_PATH} -I ${ETHASH_LIB_PATH}
LINK_LIBS := ${PROC_EVM_LINK_PATHS} ${EVM_STATE_DB_LINK_PATHS} ${INTX_LINK_PATHS} ${ETHASH_LINK_PATHS}

BUILD_DIR := ./builds
BIN_DIR := ./bin
SRC_DIR := ./src

SRCS := $(shell find $(SRC_DIR) -name '*.cpp')
SRCS_WITH_SRC_STRIPPED := $(subst $(SRC_DIR)/,,$(SRCS))
OBJS := $(SRCS_WITH_SRC_STRIPPED:%=$(BUILD_DIR)/%.o)

all: fraktal-vm

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

fraktal-vm: $(OBJS)
	mkdir -p $(BIN_DIR)
	$(CC) $(CPPFLAGS) $(INC_LIBS) $(LINK_LIBS) $(OBJS) -o $(BIN_DIR)/fraktal-vm

$(BUILD_DIR)/%.cpp.o: $(SRC_DIR)/%.cpp
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CXXFLAGS) $(INC_LIBS) -c $< -o $@

TEST_DIR := ./test

run-fraktal-vm-test:
	${BIN_DIR}/fraktal-vm run
