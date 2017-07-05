# Component makefile for extras/srp

# expected anyone using this driver includes it as 'srp/srp.h'
INC_DIRS += $(srp_ROOT)..

# args for passing into compile rule generation
srp_SRC_DIR = $(srp_ROOT)

$(eval $(call component_compile_rules,srp))
