
.PHONY: FORCE all

MTK_FW_VERSION ?= undefined
ifeq ($(MTK_FW_VERSION),)
MTK_FW_VERSION := undefined
endif

BOARD_CONFIG ?= undefined


CCFLAG += -I$(ROOTDIR)/middleware/MTK/verno/inc

# Pretend the symbol is undefined, to force linking it
LDFLAG += -u build_date_time_str -u sw_verno_str -u hw_verno_str 

# let "all" on top to be the default target.
all:

# generate verno information object and add verno.o to .elf prerequisite.
$(IMAGE): $(OUTDIR)/middleware/MTK/verno/verno.o

$(OUTDIR)/middleware/MTK/verno/verno.o: $(OUTDIR)/middleware/MTK/verno/verno.c
	@mkdir -p $(dir $@)
	echo Compiling... $^
	@mkdir -p $(dir $@)
	@echo $(CC) $(CCFLAG) $(INC:%=-I"$(ROOTDIR)/%") $(DEFINE:%=-D%) -MD -MF $(subst .o,.d,$@) -c -o $@ $< >> $(BUILD_LOG)
	@$(CC) $(CCFLAG) $(INC:%=-I"$(ROOTDIR)/%") $(DEFINE:%=-D%) -MD -MF $(subst .o,.d,$@) -c -o $@ $< 2>>$(ERR_LOG)

# generate verno.c based on template.
$(OUTDIR)/middleware/MTK/verno/verno.c: FORCE
	@mkdir -p $(dir $@); \
	$(ROOTDIR)/middleware/MTK/verno/gen_verno.sh $(ROOTDIR)/middleware/MTK/verno/verno.template $@ $(MTK_FW_VERSION) $(BOARD_CONFIG) $(ROOTDIR)

FORCE:

