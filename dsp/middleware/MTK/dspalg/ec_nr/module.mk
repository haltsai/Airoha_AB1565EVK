
ifeq ($(AIR_VOICE_NR_ENABLE),y)

	###################################################
	ECNR_FUNC_PATH = middleware/MTK/dspalg/ec_nr


	C_SRC += $(ECNR_FUNC_PATH)/aec_nr_interface.c
	ifeq ($(MTK_3RD_PARTY_NR),y)
		C_SRC += $(ECNR_FUNC_PATH)/tx_eq_interface.c
	endif

	ifeq ($(AIR_HFP_DNN_PATH_ENABLE), y)
		C_SRC += $(ECNR_FUNC_PATH)/dnn_nr_interface.c
	endif

	#C_SRC += $(ECNR_FUNC_PATH)/dsp_para_ecnr.c

	ifneq ($(MTK_BT_A2DP_ECNR_USE_PIC),y)
		ifeq ($(MTK_INEAR_ENHANCEMENT), y)
		else ifeq ($(MTK_DUALMIC_INEAR), y)
		else ifeq ($(MTK_3RD_PARTY_NR), y)
		else
			LIBS += $(strip $(LIBDIR2))/ec_nr/$(IC_CONFIG)/libecnr50.a
		endif
	else
		ifeq ($(MTK_INEAR_ENHANCEMENT), y)
			PIC += $(strip $(LIBDIR2))/ec_nr/$(IC_CONFIG)/pisplit/pisplit_ecnr_inear.o
		else ifeq ($(MTK_DUALMIC_INEAR), y)
			PIC += $(strip $(LIBDIR2))/ec_nr/$(IC_CONFIG)/pisplit/pisplit_ecnr_inear_v2.o
		else ifeq ($(MTK_3RD_PARTY_NR), y)
			PIC += $(strip $(LIBDIR2))/ec_nr/$(IC_CONFIG)/pisplit/pisplit_ec_rxnr.o
			PIC += $(strip $(LIBDIR2))/ec_nr/$(IC_CONFIG)/pisplit/pisplit_fir_eq.o
			ifeq ($(AIR_IGO_NR_ENABLE), y)
				PIC += $(strip $(LIBDIR3))/igo_nr/pisplit/pisplit_igo_txnr.o
			endif
			ifeq ($(AIR_IGO_NR_PLUS_1_ENABLE), y)
				PIC += $(strip $(LIBDIR3))/igo_nr/pisplit/pisplit_igo_plus_1_txnr.o
			endif
		else
			PIC += $(strip $(LIBDIR2))/ec_nr/$(IC_CONFIG)/pisplit/pisplit_ecnr.o
		endif
		C_SRC += $(ECNR_FUNC_PATH)/portable/ecnr_portable.c

		ifeq ($(MTK_3RD_PARTY_NR), y)
			C_SRC += $(ECNR_FUNC_PATH)/portable/tx_eq_portable.c
		endif

		ifeq ($(AIR_HFP_DNN_PATH_ENABLE), y)
			ifeq ($(AIR_DNN_LIB_ENABLE), y)
				PIC += $(strip $(LIBDIR2))/ec_nr/$(IC_CONFIG)/pisplit/pisplit_igo_txnr.o
				C_SRC += $(ECNR_FUNC_PATH)/portable/dnn_nr_portable.c
			endif
		endif

		ifeq ($(AIR_EC120_ENABLE), y)
			PIC += $(strip $(LIBDIR2))/ec_nr/$(IC_CONFIG)/pisplit/pisplit_ec120.o
			C_SRC += $(ECNR_FUNC_PATH)/portable/ec120_portable.c
		endif

		ifeq ($(AIR_ECNR_POST_PART_USE_PIC_ENABLE), y)
			PIC += $(strip $(LIBDIR2))/ec_nr/$(IC_CONFIG)/pisplit/pisplit_post_ec.o
		endif

	endif

	###################################################
	# include path


	INC += $(ECNR_FUNC_PATH)
	INC += $(ECNR_FUNC_PATH)/portable

endif
