FTMSOURCES = $(shell find $(OBJDIR) -name *.ftxt)
FTMOBJECTS = $(FTMSOURCES:%.ftxt=%.o) 

# Find exported FTMs
vpath %ftxt $(foreach ftxt,$(FTMSOURCES),$(dir $ftxt))

# Add FamiTone2 sources to project make rules
SOURCES += $(FTMSOURCES)
OBJECTS += $(FTMOBJECTS)

# Build a FamiTone2 object file
%.o : %.ftxt
	text2data -ca65 $^ 
	$(ASSEMBLE) $(ASFLAGS) -o $@ $(^:%.ftxt=%.s)

# Add FamiTone2 debris to project clean rules
REMOVES += $(FTMSOURCES) $(FTMSOURCES:%.ftxt=%.s)
REMOVES += $(OBJDIR)/famitone2.o
