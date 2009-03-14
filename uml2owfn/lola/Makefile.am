
EXTRA_DIST = userconfig.H.statepredicate \
	userconfig.H.modelchecking \
	userconfig.H.liveprop \
	userconfig.H.full-limited

##########################################################################
# create LoLA binaries
##########################################################################

LOLA_SRC_DIR      = ../../lola/src

LOLA_SP           = lola-sp${EXEEXT}   # LoLA with STATEPREDICATE
LOLA_MC           = lola-mc${EXEEXT}   # LoLA with MODELCHECKING
LOLA_LP           = lola-lp${EXEEXT}   # LoLA with LIVEPROP
LOLA_FULL_LIMITED = lola-full-limited${EXEEXT}  # LoLA with FULL

all: lola

lola: ${LOLA_SP} ${LOLA_MC} ${LOLA_LP} ${LOLA_FULL_LIMITED}

${LOLA_SP}: userconfig.H.statepredicate
	@echo "-----------------------------------------------------"
	@echo "Creating $@"
	@echo "-----------------------------------------------------"
	-mv ${LOLA_SRC_DIR}/userconfig.H ${LOLA_SRC_DIR}/userconfig.H.by.uml2owfn
	-cp $< ${LOLA_SRC_DIR}/userconfig.H
	-(cd ${LOLA_SRC_DIR}/ && rm lola${EXEEXT} && make)
	-cp ${LOLA_SRC_DIR}/lola${EXEEXT} $@
	-mv ${LOLA_SRC_DIR}/userconfig.H.by.uml2owfn ${LOLA_SRC_DIR}/userconfig.H

${LOLA_MC}: userconfig.H.modelchecking
	@echo "-----------------------------------------------------"
	@echo "Creating $@"
	@echo "-----------------------------------------------------"
	-mv ${LOLA_SRC_DIR}/userconfig.H ${LOLA_SRC_DIR}/userconfig.H.by.uml2owfn
	-cp $< ${LOLA_SRC_DIR}/userconfig.H
	-(cd ${LOLA_SRC_DIR}/ && rm lola${EXEEXT} && make)
	-cp ${LOLA_SRC_DIR}/lola${EXEEXT} ${LOLA_MC}
	-mv ${LOLA_SRC_DIR}/userconfig.H.by.uml2owfn ${LOLA_SRC_DIR}/userconfig.H

${LOLA_LP}: userconfig.H.liveprop
	@echo "-----------------------------------------------------"
	@echo "Creating $@"
	@echo "-----------------------------------------------------"
	-mv ${LOLA_SRC_DIR}/userconfig.H ${LOLA_SRC_DIR}/userconfig.H.by.uml2owfn
	-cp $< ${LOLA_SRC_DIR}/userconfig.H
	-(cd ${LOLA_SRC_DIR}/ && rm lola${EXEEXT} && make)
	-cp ${LOLA_SRC_DIR}/lola${EXEEXT} $@
	-mv ${LOLA_SRC_DIR}/userconfig.H.by.uml2owfn ${LOLA_SRC_DIR}/userconfig.H

${LOLA_FULL_LIMITED}: userconfig.H.full-limited
	@echo "-----------------------------------------------------"
	@echo "Creating $@"
	@echo "-----------------------------------------------------"
	-mv ${LOLA_SRC_DIR}/userconfig.H ${LOLA_SRC_DIR}/userconfig.H.by.uml2owfn
	-cp $< ${LOLA_SRC_DIR}/userconfig.H
	-(cd ${LOLA_SRC_DIR}/ && rm lola${EXEEXT} && make)
	-cp ${LOLA_SRC_DIR}/lola${EXEEXT} $@
	-mv ${LOLA_SRC_DIR}/userconfig.H.by.uml2owfn ${LOLA_SRC_DIR}/userconfig.H
	
clean:
	rm -fr ${LOLA_SP} ${LOLA_MC} ${LOLA_LP} ${LOLA_FULL_LIMITED}
