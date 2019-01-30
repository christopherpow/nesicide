UC65_HOME = $(CC65_HOME)/../uc65-release-0.5-rc4

vpath %uc $(foreach uc,$(SOURCES),$(dir $uc))

$(OBJDIR)/%.o : %.uc
	java -jar $(UC65_HOME)/uc65.jar -i src -o $(@:.o=.s) $<
	$(ASSEMBLE) $(ASFLAGS) -o $@ $(@:.o=.s)

