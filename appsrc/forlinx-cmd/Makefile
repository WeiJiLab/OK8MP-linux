dirs := $(shell find . -maxdepth 1 -type d)
dirs := $(basename $(patsubst ./%,%,$(dirs)))
dirs := $(filter-out $(exclude_dirs),$(dirs))
SUBDIRS := $(dirs)

all:
	for dir in $(SUBDIRS);\
    do \
		$(MAKE) -C $$dir; \
    done

.PHONY: clean install

clean:
	for dir in $(SUBDIRS);\
    do $(MAKE) -C $$dir clean;\
    done

install:
	for dir in $(SUBDIRS);\
	do $(MAKE) -C $$dir install;\
	done
