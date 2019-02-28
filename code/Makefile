CFLAGS=-march=native -pipe -O2 -Wall -I/usr/include/opencv4
LDFLAGS=-lopencv_highgui -lopencv_imgproc -lopencv_imgcodecs -lopencv_core `fltk-config --ldflags --cxxflags` -lm $(DFLAGS)
BIN=fastCropper

all: $(BIN).cpp
	g++ $(CFLAGS) $(BIN).cpp $(LDFLAGS) -s -o $(BIN)

clean:
	$(RM) $(BIN)

install:
	cp ./fastCropper /usr/bin/fastCropper
	cp ./fastCropper.png /usr/share/pixmaps/fastCropper.png
	cp ./fastCropper.desktop /usr/share/applications/fastCropper.desktop

