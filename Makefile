
all: robot test

robot:
	(cd input ; make TARGET=ROBOT)
	(cd control ; make TARGET=ROBOT)
	(cd executive ; make TARGET=ROBOT)
	(cd util ; make TARGET=ROBOT)
	(cd pixycam ; make TARGET=ROBOT)
	(cd roborio ; make TARGET=ROBOT)

test: locallibs
	(cd executive ; make -f exectest.mk TARGET=LOCAL)
	(cd executive ; make -f timetest.mk TARGET=LOCAL)
	(cd executive ; make -f steptest.mk TARGET=LOCAL)
	(cd executive ; make -f teleoptest.mk TARGET=LOCAL)

locallibs:
	(cd input ; make TARGET=LOCAL)
	(cd control ; make TARGET=LOCAL)
	(cd executive ; make TARGET=LOCAL)
	(cd util ; make TARGET=LOCAL)
	(cd pixycam ; make TARGET=LOCAL)
