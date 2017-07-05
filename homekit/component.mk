# Component makefile for extras/homekit

# expected anyone using this driver includes it as 'homekit/homekit.h'
INC_DIRS += $(homekit_ROOT)..

# args for passing into compile rule generation
homekit_SRC_DIR = $(homekit_ROOT)

$(eval $(call component_compile_rules,homekit))
