CFLAGS=-march=native -pipe -O2 -Wall
LDFLAGS=`pkg-config --cflags opencv --libs opencv` `fltk-config --ldflags --cxxflags` -lm $(DFLAGS)
BIN=fastCropper

all: $(BIN).cpp
	g++ $(CFLAGS) $(BIN).cpp $(LDFLAGS) -s -o $(BIN)

clean:
	$(RM) $(BIN)

install:
	cp ./fastCropper /usr/bin/fastCropper
	cp ./fastCropper.png /usr/share/pixmaps/fastCropper.png
	cp ./fastCropper.desktop /usr/share/applications/fastCropper.desktop

