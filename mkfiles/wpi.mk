WPIHOME=$(shell echo ~/wpilib)
CTREHOME=$(shell echo ~/CTRE_FRCLib)

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

CXXFLAGS += -I$(WPIHOME)/cpp/current/include -I$(CTREHOME)/cpp/include
LDFLAGS += -L$(WPIHOME)/cpp/current/lib -L$(CTREHOME)/cpp/lib -lCTRLib -lwpi

