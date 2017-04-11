#
# Generate the test list by asking the make file in each subdirectory what tests
# it has.
#
TESTLIST = $(addprefix executive/,$(shell cd executive ; make TARGET=LOCAL testlist))
TESTLIST += $(addprefix input/,$(shell cd input ; make TARGET=LOCAL testlist))
TESTLIST += $(addprefix control/,$(shell cd control ; make TARGET=LOCAL testlist))
TESTLIST += $(addprefix pixycam/,$(shell cd pixycam ; make TARGET=LOCAL testlist))
TESTLIST += $(addprefix util/,$(shell cd util ; make TARGET=LOCAL testlist))

#
# Generate the path to the actual make file for each test
#
TESTLISTMK = $(addsuffix .mk,$(TESTLIST))


all: robot test

robot:
	(cd input ; make TARGET=ROBOT)
	(cd control ; make TARGET=ROBOT)
	(cd executive ; make TARGET=ROBOT)
	(cd util ; make TARGET=ROBOT)
	(cd pixycam ; make TARGET=ROBOT)
	(cd roborio ; make TARGET=ROBOT)

test: locallibs runtests analyze

analyze:
	bash analysis.sh mybuild/LOCAL/logs


runtests:
	$(foreach THIS,$(TESTLISTMK),\
	(cd $(dir $(THIS)) ; make TARGET=LOCAL -f $(notdir $(THIS)));)

locallibs:
	(cd input ; make TARGET=LOCAL)
	(cd control ; make TARGET=LOCAL)
	(cd executive ; make TARGET=LOCAL)
	(cd util ; make TARGET=LOCAL)
	(cd pixycam ; make TARGET=LOCAL)
