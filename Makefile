CXX = g++
CXXFLAGS = -I./include
LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system
TARGET = build/cppathfinder
SRC = src/main.cpp
ASSETS = arial.ttf maps LICENSE README.md




all: $(TARGET)



$(TARGET): $(SRC)
	mkdir -p build
	$(CXX) $(SRC) $(CXXFLAGS) $(LDFLAGS) -o $(TARGET)


install:
	install -D $(TARGET) $(DESTDIR)/usr/bin/cppathfinder
	mkdir -p $(DESTDIR)/usr/share/cppathfinder
	cp -r $(ASSETS) $(DESTDIR)/usr/share/cppathfinder/


clean:
	rm -rf build