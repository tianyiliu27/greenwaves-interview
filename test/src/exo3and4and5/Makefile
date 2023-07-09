# User Test
#------------------------------------


APP              = test
APP_SRCS        += copydata.c 
APP_INC         += /home/osboxes/test-C/exo3and4/include
APP_CFLAGS      += 
PMSIS_OS = freertos
ifeq ($(ASYNC), 1)
APP_CFLAGS 	+= -DASYNC
endif

include /home/osboxes/test-C/greenwaves-interview/test/gap_sdk/utils/rules/pmsis_rules.mk
