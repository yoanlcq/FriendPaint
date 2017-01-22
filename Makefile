ifneq ($(os),win32)
ifneq ($(os),win64)
ifneq ($(os),macosx)
ifneq ($(os),linux32)
ifneq ($(os),linux64)
$(error Please set the 'os' variable to one of : win32 | win64 | macosx | linux32 | linux64. Example : 'make os=win32' )
endif
endif
endif
endif
endif

wcmd_rmdir_rf = if exist $(subst /,\\,$(1)) ( rmdir /q /s $(subst /,\\,$(1)) )
unix_rmdir_rf = rm -rf $(1)
rmdir_rf = $(call $(if $(filter win%,$(os)),wcmd_rmdir_rf,unix_rmdir_rf),$(1))
mkdir_p = mkdir $(if $(filter win%,$(os)),$(subst /,\\,$(1)),-p $(1))

dotexe   := $(if $(filter win%,$(os)),.exe)
cxx      := g++
cxxflags := -std=c++11 $(if $(filter %32,$(os)),-m32,-m64)
ldlibs   := -lm -lsfml-graphics -lsfml-network -lsfml-window -lsfml-system
clean    := $(call rmdir_rf,build/$(os))
fp_cltexe     := bin/$(os)/friendpaint$(dotexe)
fp_srvexe     := bin/$(os)/friendpaint_server$(dotexe)
minipaintexe  := bin/$(os)/minipaint$(dotexe)
tchatexe      := bin/$(os)/tchat$(dotexe)
ntchat_cltexe := bin/$(os)/new_tchat_clt$(dotexe)
ntchat_srvexe := bin/$(os)/new_tchat_srv$(dotexe)


.PHONY : all clean mrproper re

all : $(fp_cltexe) $(fp_srvexe) $(tchatexe) $(minipaintexe) $(tchatexe) $(ntchat_cltexe) $(ntchat_srvexe)


clean :
	$(clean)
mrproper : clean all
re : clean all



build/$(os)/tchat.o : legacy/examples/tchat.cpp
	@$(call mkdir_p, $(@D))
	$(cxx) $(cxxflags) -o $@ -c $<

$(tchatexe) : build/$(os)/tchat.o
	@$(call mkdir_p, $(@D))
	$(cxx) $(cxxflags) -o $@ $^ $(ldlibs)


build/$(os)/new_tchat_clt.o : src/Network/new_tchat_clt.cpp
	@$(call mkdir_p, $(@D))
	$(cxx) $(cxxflags) -o $@ -c $^

$(ntchat_cltexe) : build/$(os)/new_tchat_clt.o
	@$(call mkdir_p, $(@D))
	$(cxx) $(cxxflags) -o $@ $^ $(ldlibs)


build/$(os)/new_tchat_srv.o : src/Network/new_tchat_srv.cpp
	@$(call mkdir_p, $(@D))
	$(cxx) $(cxxflags) -o $@ -c $<

$(ntchat_srvexe) : build/$(os)/new_tchat_srv.o
	@$(call mkdir_p, $(@D))
	$(cxx) $(cxxflags) -o $@ $^ $(ldlibs)



build/$(os)/minipaint.o : legacy/examples/minipaint.cpp
	@$(call mkdir_p, $(@D))
	$(cxx) $(cxxflags) -o $@ -c $<

$(minipaintexe) : build/$(os)/minipaint.o
	@$(call mkdir_p, $(@D))
	$(cxx) $(cxxflags) -o $@ $^ $(ldlibs)



build/$(os)/prisma.o : src/prisma.c
	@$(call mkdir_p, $(@D))
	$(cxx) $(cxxflags) -o $@ -c $^

build/$(os)/GUI.o : src/GUI.cpp
	@$(call mkdir_p, $(@D))
	$(cxx) $(cxxflags) -o $@ -c $^

build/$(os)/PaintEngine.o : src/PaintEngine.cpp
	@$(call mkdir_p, $(@D))
	$(cxx) $(cxxflags) -o $@ -c $^

build/$(os)/interface.o : src/interface.cpp
	@$(call mkdir_p, $(@D))
	$(cxx) $(cxxflags) -o $@ -c $^

build/$(os)/main.o : src/main.cpp
	@$(call mkdir_p, $(@D))
	$(cxx) $(cxxflags) -o $@ -c $^

build/$(os)/PaintServer.o : src/PaintServer.cpp
	@$(call mkdir_p, $(@D))
	$(cxx) $(cxxflags) -o $@ -c $^

$(fp_cltexe) : build/$(os)/main.o build/$(os)/GUI.o build/$(os)/PaintEngine.o build/$(os)/interface.o build/$(os)/prisma.o
	@$(call mkdir_p, $(@D))
	$(cxx) $(cxxflags) -o $@ $^ $(ldlibs)

$(fp_srvexe) : build/$(os)/prisma.o build/$(os)/PaintServer.o build/$(os)/GUI.o build/$(os)/PaintEngine.o build/$(os)/interface.o
	@$(call mkdir_p, $(@D))
	$(cxx) $(cxxflags) -o $@ $^ $(ldlibs)
