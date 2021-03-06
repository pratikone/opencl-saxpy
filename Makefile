OUT_DIR = ./bin
EXECUTABLES = cl-demo print-devices cl-mul0 cl-mul0-saxpy cl-mul0-blocks cl-mul1 cl-mul2-saxpy cl-mul3-blocks memory membench

all: $(EXECUTABLES:%=$(OUT_DIR)/%)

ifdef OPENCL_INC
  CL_CFLAGS = -I$(OPENCL_INC)
endif

ifdef OPENCL_LIB
  CL_LDFLAGS = -L$(OPENCL_LIB)
endif

$(OUT_DIR)/print-devices: print-devices.c cl-helper.c
	gcc $^ $(CL_CFLAGS) $(CL_LDFLAGS) -std=gnu99 -lrt -lOpenCL -o$@

$(OUT_DIR)/cl-demo: cl-demo.c cl-helper.c
	gcc $^ $(CL_CFLAGS) $(CL_LDFLAGS) -std=gnu99 -lrt -lOpenCL -o$@

$(OUT_DIR)/cl-mul0: cl-mul0.c
	gcc $^ $(CL_CFLAGS) $(CL_LDFLAGS) -std=gnu99 -lrt -lOpenCL -o$@

$(OUT_DIR)/cl-mul0-saxpy: cl-mul0-saxpy.c
	gcc $^ $(CL_CFLAGS) $(CL_LDFLAGS) -std=gnu99 -lrt -lOpenCL -o$@

$(OUT_DIR)/cl-mul0-blocks: cl-mul0-blocks.c
	gcc $^ $(CL_CFLAGS) $(CL_LDFLAGS) -std=gnu99 -lrt -lOpenCL -o$@

$(OUT_DIR)/cl-mul1: cl-mul1.c cl-helper.c
	gcc $^ $(CL_CFLAGS) $(CL_LDFLAGS) -std=gnu99 -lrt -lOpenCL -o$@

$(OUT_DIR)/cl-mul2-saxpy: cl-mul2-saxpy.c cl-helper.c
	gcc $^ $(CL_CFLAGS) $(CL_LDFLAGS) -std=gnu99 -lrt -lOpenCL -o$@

$(OUT_DIR)/cl-mul3-blocks: cl-mul3-blocks.c cl-helper.c
	gcc $^ $(CL_CFLAGS) $(CL_LDFLAGS) -std=gnu99 -lrt -lOpenCL -o$@

$(OUT_DIR)/memory: memory.c cl-helper.c
	gcc $^ $(CL_CFLAGS) $(CL_LDFLAGS) -std=gnu99 -lrt -lOpenCL -o$@

$(OUT_DIR)/membench: membench.c
	gcc $^ $(CL_CFLAGS) $(CL_LDFLAGS) -std=gnu99 -lrt -o$@

clean:
	rm -f $(EXECUTABLES:%=$(OUT_DIR)/%) *.o
