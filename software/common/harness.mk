# ======================================================== Software Settings ====================================================== #

# sw tools
C_CXX   = $(CC)

# compiler flags
SW_FLAGS ?= -O3 -Wall -Wno-unused-label -v

# ==================================================== Data collection settings =================================================== #
DATA_FOLDER = /data/sachille/`date +'%Y%m%d_%H%M'`_$(KERNEL)_$(ALGORITHM_NAME)_$(VERSION_NAME)

# ========================================================= Rules ================================================================= #

.PHONY: all run clean

all: run

# software rules
$(KERNEL): $(FILES)
	$(C_CXX) $(SW_FLAGS) -I../../common -o $(KERNEL) $(SRCS) $(HOST_SRC)

run: $(KERNEL) input.data check.data
	./$(KERNEL) input.data check.data
	mkdir $(DATA_FOLDER)_sw
	cp output.data $(DATA_FOLDER)_sw

# cleanup
clean:
	rm -f $(KERNEL) generate output.data
