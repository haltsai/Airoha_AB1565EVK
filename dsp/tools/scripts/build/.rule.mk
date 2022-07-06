################################################################################
# Compiler Toolchain Settings
################################################################################
#Xtensa tool chain path & license setting,
#These settings can be configured either in a project's Makefile for the specific
#project or setting here for all the projets.
-include ~/airoha_sdk_toolchain/airoha_sdk_env_156x

export IOT_SDK_XTENSA_VERSION := $(IOT_SDK_XTENSA_VERSION)

ifeq ($(shell uname), Linux)

    ifeq ($(IOT_SDK_XTENSA_VERSION), 809)
        XTENSA_ROOT ?= /mtkeda/xtensa/Xplorer-8.0.9
        XTENSA_VER ?= RG-2019.12-linux
    else
        XTENSA_ROOT ?= /mtkeda/xtensa/Xplorer-7.0.7
        XTENSA_VER ?= RG-2017.7-linux
    endif

    # XTENSA_LICENSE_FILE Server
    ifeq ($(shell domainname), mcdswrd)
      XTENSA_LICENSE_FILE ?= 7400@10.19.50.25
    else
      XTENSA_LICENSE_FILE ?= 7400@mtklc17
    endif

    ifeq ($(SDK_PACKAGE), yes)
        XTENSA_SYSTEM ?= $(XTENSA_ROOT)/xtensa/$(XTENSA_VER)/XtensaTools/config/
        XTENSA_BIN_PATH ?= $(XTENSA_ROOT)/xtensa/$(XTENSA_VER)/XtensaTools/bin
    else
       XTENSA_BIN_PATH ?= $(XTENSA_ROOT)/XtDevTools/install/tools/$(XTENSA_VER)/XtensaTools/bin
    endif

else
    XTENSA_ROOT ?= $(HOME)/airoha_sdk_toolchain
    ifeq ($(IOT_SDK_XTENSA_VERSION), 809)
        XTENSA_VER ?= RG-2019.12-win32
    else
        XTENSA_VER ?= RG-2017.7-win32
    endif
    XTENSA_SYSTEM ?= $(XTENSA_ROOT)/xtensa/$(XTENSA_VER)/XtensaTools/config/
    XTENSA_BIN_PATH ?= $(XTENSA_ROOT)/xtensa/$(XTENSA_VER)/XtensaTools/bin
endif

export PATH := $(XTENSA_BIN_PATH):$(PATH)

$(info XTENSA_ROOT : $(XTENSA_ROOT) )
$(info XTENSA_VER : $(XTENSA_VER) )
$(info XTENSA_BIN_PATH : $(XTENSA_BIN_PATH) )

ifeq ($(shell uname), Linux)
    ifeq ($(wildcard $(XTENSA_BIN_PATH)),)
        $(error Folder $(XTENSA_BIN_PATH) not exist! )
    else
        # check xtensa toolchain
        xtensa_status:=$(shell export PATH=$(XTENSA_BIN_PATH):$(PATH); which xt-xcc)
        ifeq ($(xtensa_status),)
            $(error Error : Missing Xtensa toolchain. The Xtensa toolchain may not be installed correctly or been removed. Please run install.sh to resovle the problem.)
        endif
    endif
endif

export LM_LICENSE_FILE := $(XTENSA_LICENSE_FILE)
export XTENSA_SYSTEM ?= $(XTENSA_ROOT)/XtDevTools/XtensaRegistry/$(XTENSA_VER)
export XTENSA_CORE := $(XTENSA_CORE)
LM_LICENSE_FILE := $(strip $(LM_LICENSE_FILE))
XTENSA_CORE     := $(strip $(XTENSA_CORE))
XTENSA_SYSTEM   := $(strip $(XTENSA_SYSTEM))

.PHONY: clean_log

include $(ROOTDIR)/middleware/MTK/verno/module.mk

################################################################################
# Common Rule
################################################################################
clean_log:
ifeq ($(TARGET_PATH),)
	@if [ -e "$(strip $(LOGDIR))" ]; then rm -rf "$(strip $(LOGDIR))"; fi
	@mkdir -p "$(strip $(LOGDIR))"
	@make gen_copy_firmware_opts
else
	@echo "trigger by build.sh, skip clean_log"
	@make gen_copy_firmware_opts
endif

gen_copy_firmware_opts:
ifeq ($(wildcard $(strip $(OUTDIR))/copy_firmware_opts.log),)
    ifeq ($(filter clean prebuilt%,$(MAKECMDGOALS)),)
        MAKE_VARS := $(shell echo '$(.VARIABLES)' | awk -v RS=' ' '/^[a-zA-Z0-9]+[a-zA-Z0-9_]*[\r\n]*$$/' | sort | uniq)
        CP_FIRMWARE_OPTS_TMP := $(shell cat $(strip $(ROOTDIR))/tools/scripts/build/copy_firmware_opts.lis | tr -d ' ' | sort | uniq)
        CP_FIRMWARE_OPTS := $(filter $(CP_FIRMWARE_OPTS_TMP), $(MAKE_VARS))
        #$(shell ( rm -f $(strip $(OUTDIR))/dsp_cflags.log && echo $(CCFLAG) |tr " " "\n" |grep "\-D"|tr -d "\-D" > $(strip $(OUTDIR))/dsp_cflags.log) )
        $(shell (test -e $(strip $(OUTDIR))/copy_firmware_opts.log && rm -f $(strip $(OUTDIR))/copy_firmware_opts.log))
        $(shell mkdir -p $(strip $(OUTDIR)))
        $(foreach v,$(CP_FIRMWARE_OPTS), $(shell echo $(v)='$($(v))' >> $(strip $(OUTDIR))/copy_firmware_opts.log))
    endif
endif

################################################################################
# Pattern Rule
################################################################################
$(OUTDIR)/%.o : $(ROOTDIR)/%.S
	@echo Assembling... $^
	@mkdir -p $(dir $@)
	@echo $(AS) $(ASFLAG) $(INC:%=-I"$(ROOTDIR)/%") -MD -MF $(subst .o,.d,$@) -c -o $@ $< >> $(BUILD_LOG)
	@$(AS) $(ASFLAG) $(INC:%=-I"$(ROOTDIR)/%") -MD -MF $(subst .o,.d,$@) -c -o $@ $< 2>>$(ERR_LOG)

$(OUTDIR)/%.o : $(ROOTDIR)/%.c
	@echo Compiling... $^
	@mkdir -p $(dir $@)
	@echo $(CC) $(CCFLAG) $(INC:%=-I"$(ROOTDIR)/%") $(DEFINE:%=-D%) -MMD -MF $(subst .o,.d,$@) -c -o $@ $< >> $(BUILD_LOG)
	@if [ -e "$@" ]; then rm -f "$@"; fi ;\
	$(CC) $(CCFLAG) $(INC:%=-I"$(ROOTDIR)/%") $(DEFINE:%=-D%) -MMD -MF $(subst .o,.d,$@) -c -o $@ $< 2>>$(ERR_LOG) ;\
	if [ "$$?" != "0" ]; then \
		echo "Build... $$(basename $@) FAIL"; \
		echo "Build... $@ FAIL" >> $(BUILD_LOG); \
	else \
    sed -i 's/\([A-Z]\):\//\/\L\1\//g' $(basename $@).d ;\
		echo "Build... $$(basename $@) PASS"; \
		echo "Build... $@ PASS" >> $(BUILD_LOG); \
	fi;

$(TARGET_LIB).a: $(OBJ)
	@echo Gen $(TARGET_LIB).a
	@echo Gen $(TARGET_LIB).a >>$(BUILD_LOG)
	@if [ -e "$@" ]; then rm -f "$@"; fi
	@mkdir -p $(dir $@)
	@$(AR) -r $@ $(OBJ) >>$(BUILD_LOG) 2>>$(ERR_LOG);  \
	if [ "$$?" != "0" ]; then \
		echo MODULE BUILD $@ FAIL ; \
		echo MODULE BUILD $@ FAIL >> $(BUILD_LOG) ; \
		exit 1 ;\
	else \
		echo MODULE BUILD $@ PASS ; \
		echo MODULE BUILD $@ PASS >> $(BUILD_LOG) ; \
	fi;


