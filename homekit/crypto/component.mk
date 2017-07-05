# Component makefile for extras/crypto

# expected anyone using this driver includes it as 'crypto/crypto.h'
INC_DIRS += $(crypto_ROOT)..

# args for passing into compile rule generation
crypto_SRC_DIR = $(crypto_ROOT)

$(eval $(call component_compile_rules,crypto))
