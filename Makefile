CXX=g++
CFLAGS=`sdl-config --cflags` -Werror -Wall -ggdb -Itools/include
LIBS=`sdl-config --libs` -L/usr/X11R6/lib -lX11


%.o: %.c
	@echo [CC] $<
	@$(CXX) $(CFLAGS) -o $@ -c $<
	
%.o: %.cpp
	@echo [CC] $<
	@$(CXX) $(CFLAGS) -o $@ -c $<
	
test: example/test.o example/spf.o example/font.o src/sdlinput.o
	@echo [LINK] $@
	@$(CXX) $(CFLAGS) example/test.o example/spf.o example/font.o src/sdlinput.o -o test $(LIBS)
