all: sample2D

sample2D: Sample_GL3_2D.cpp tile_info.h tile_info.cpp header.h
	g++ -std=c++11  -g -o sample2D Sample_GL3_2D.cpp tile_info.cpp  -lglfw -lGLEW -lGL -ldl -lpthread -lao -lmpg123

clean:
	rm sample2D
