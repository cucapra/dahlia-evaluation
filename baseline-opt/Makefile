include config.mk

CMD =
ifdef SOFTWARE
CMD += SOFTWARE=1

else
ifdef ESTIMATE
CMD += ESTIMATE=1
endif

ifdef DIRECTIVE
CMD += DIRECTIVE=1
endif
endif


.PHONY: build run submit clean

build:
	@( for b in $(BENCHMARKS); do $(MAKE) -C $$b; done )

# for software only
run:
	@( for b in $(BENCHMARKS); do $(MAKE) -C $$b run $(CMD); done )

# for buildbot only
submit:
	@( for b in $(BENCHMARKS); do $(MAKE) -C $$b submit $(CMD); done )

clean:
	@( for b in $(BENCHMARKS); do $(MAKE) -C $$b clean || exit ; done )
