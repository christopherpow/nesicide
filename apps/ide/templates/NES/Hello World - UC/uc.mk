UC65_HOME = $(CC65_HOME)/../uc65-release-0.5-rc6

vpath %uc $(foreach uc,$(SOURCES),$(dir $uc))

$(OBJDIR)/%.o : %.uc
	java -jar $(UC65_HOME)/uc65.jar -i src -o $(@:.o=.s) $<
	$(ASSEMBLE) $(ASFLAGS) -o $@ $(@:.o=.s)

# Get rid of intermediate .s files
REMOVES += $(OBJDIR)/*.s
