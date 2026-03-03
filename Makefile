CXX = g++
CXXFLAGS = -Wall -fPIC
LDFLAGS = -shared

all: rgr_crypto

# компиляция общих файловых функций
file_io.o: file_io.cpp file_io.h
	$(CXX) $(CXXFLAGS) -c file_io.cpp

# создание трех независимых библиотек .so
libdh.so: dh.cpp dh.h
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o libdh.so dh.cpp

libtea.so: tea.cpp tea.h
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o libtea.so tea.cpp

libscytale.so: scytale.cpp scytale.h file_io.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o libscytale.so scytale.cpp file_io.o

# сборка головного модуля с привязкой трех библиотек
rgr_crypto: main.cpp libdh.so libtea.so libscytale.so file_io.o
	$(CXX) main.cpp file_io.o -L. -ldh -ltea -lscytale -Wl,-rpath,. -o rgr_crypto

clean:
	rm -f *.o *.so rgr_crypto
