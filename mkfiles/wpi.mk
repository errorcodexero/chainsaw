WPIHOME=$(shell echo ~/wpilib)
CTREHOME=$(shell echo ~/CTRE_FRCLib)

$(info -------------------------------------------------------)
$(info Operating System is $(OS))

ifdef WPIOVERRIDE
WPIHOME=$(WPIOVERRIDE)
$(warning Default location for WPI has been overridden)
$(warning Using $(WPIHOME) for WPI library)
else
$(info Using default WPI library location, $(WPIHOME))
endif

ifdef CTREOVERRIDE
CTREHOME=$(CTREOVERRIDE)
$(warning Default location for CTRE has been overridden)
$(warning Using $(CTREHOME) for CTRE library)
else
$(info Using default WPI library location, $(CTREHOME))
endif

WPIINC=$(WPIHOME)/cpp/current/include
WPILIBINC=$(WPIHOME)/cpp/current/lib
CTREINC=$(CTREHOME)/cpp/include
CTRELIBINC=$(CTREHOME)/cpp/lib


ifeq ($(OS),Windows_NT)
WPIINCFINAL=$(shell cygpath --windows $(WPIINC))
WPILIBINCFINAL=$(shell cygpath --windows $(WPILIBINC))
CTREINCFINAL=$(shell cygpath --windows $(CTREINC))
CTRELIBINCFINAL=$(shell cygpath --windows $(CTRELIBINC))
$(info Translated library paths for cygwin)
$(info    $(WPIINCFINAL))
$(info    $(WPILIBINCFINAL))
$(info    $(CTREINCFINAL))
$(info    $(CTRELIBINCFINAL))
else
WPIINCFINAL=$(WPIINC)
WPILIBINCFINAL=$(WPILIBINC)
CTREINCFINAL=$(CTREINC)
CTRELIBINCFINAL=$(CTRELIBINC)
endif

CXXFLAGS += -I$(WPIINCFINAL) -I$(CTREINCFINAL)
LDFLAGS += -L$(WPILIBINCFINAL) -L$(CTRELIBINCFINAL) -lCTRLib -lwpi

$(info -------------------------------------------------------)

