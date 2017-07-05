# Component makefile for extras/tweetnacl

# expected anyone using this driver includes it as 'tweetnacl/tweetnacl.h'
INC_DIRS += $(tweetnacl_ROOT)..

# args for passing into compile rule generation
tweetnacl_SRC_DIR = $(tweetnacl_ROOT)

$(eval $(call component_compile_rules,tweetnacl))
