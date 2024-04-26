# Компилятор
CXX = g++

# Флаги компиляции
CXXFLAGS = -std=c++17 -Wall -Wextra -g3

# Исходные файлы
SRCS = main.cpp ooplloc.cpp atomicStructs.cpp mutexLock.cpp

# Объектные файлы
OBJS = main.o ooplloc.o atomicStructs.o mutexLock.o

# Исполняемый файл
TARGET = /Users/enmanish/Desktop/study/OOPlloc/allocator/main

# Правило для компиляции объектных файлов
%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<

# Правило для компиляции и компоновки исполняемого файла
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Правило для очистки объектных файлов и исполняемого файла
clean:
	rm -f $(OBJS) $(TARGET)
