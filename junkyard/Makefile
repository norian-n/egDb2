all: bytearray testdatafields testdatalayout testcontainer testlinks testflexlinks testdatabase testfingers testindexes makethelib
# clean



# CXXFLAGS+=-g -Wall
LDLIBS += -lstdc++fs
CPPFLAGS = -fPIC

#################################################################################################

EGDB_LIB = egdb
OBJ_FILES_SRC = tests/testDataFields.cpp service/egByteArray.cpp nodes/egDataNode.cpp \

OBJ_FILES_LIB = $(DATA_FIELDS_OBJ)

makethelib:
#	g++ -shared -Wl,-soname,lib$(EGDB_LIB).so.1 -o lib$(EGDB_LIB).so $(OBJ_FILES_LIB) $(LDLIBS)
	g++ $(LDLIBS) -shared $(OBJ_FILES_LIB) -o lib$(EGDB_LIB).so

# g++ main.cpp -L . -l hello_world -o out

######################################

bytearray:
# g++ -c service/egByteArray.cpp
# g++ -c nodes/egDataNode.cpp

DATA_FIELDS_SRC = tests/testDataFields.cpp service/egByteArray.cpp nodes/egDataNode.cpp
DATA_FIELDS_OBJ = testDataFields.o egByteArray.o egDataNode.o
DATA_FIELDS_BIN = testDataFields.exe
testdatafields:
	g++ -c $(CPPFLAGS) $(DATA_FIELDS_SRC)
	g++ -o $(DATA_FIELDS_BIN) $(DATA_FIELDS_OBJ)

######################################
DATA_LAYOUT_SRC = tests/testDataNodeLayout.cpp nodes/egDataNodeLayout.cpp
DATA_LAYOUT_OBJ = testDataNodeLayout.o egByteArray.o egDataNode.o egDataNodeLayout.o
DATA_LAYOUT_BIN = testDataNodeLayout.exe
testdatalayout:
	g++ -c $(DATA_LAYOUT_SRC)
	g++ -o $(DATA_LAYOUT_BIN) $(DATA_LAYOUT_OBJ)

######################################
LOCAL_FILE_SRC_PRE = service/egByteArray.cpp nodes/egDataNode.cpp nodes/egDataNodeLayout.cpp
LOCAL_FILE_SRC = tests/testDataNodesLocalFile.cpp nodes/egDataNodesLocalFile.cpp
LOCAL_FILE_OBJ = testDataNodesLocalFile.o egDataNodesLocalFile.o egByteArray.o egDataNode.o egDataNodeLayout.o
LOCAL_FILE_BIN = testDataNodesLocalFile.exe
testlocalfile:
	g++ -c $(LOCAL_FILE_SRC_PRE) -D EG_NODE_OFFSETS_DEBUG
	g++ -c $(LOCAL_FILE_SRC) -D EG_NODE_OFFSETS_DEBUG
	g++ -o $(LOCAL_FILE_BIN) $(LOCAL_FILE_OBJ)

######################################
CONTAINER_SRC = tests/testDataNodesContainer.cpp nodes/egDataNodesContainer.cpp nodes/egDataNodesLocalFile.cpp
CONTAINER_OBJ = testDataNodesContainer.o egDataNodesContainer.o egDataNodesLocalFile.o egByteArray.o egDataNode.o egDataNodeLayout.o
CONTAINER_BIN = testDataNodesContainer.exe
testcontainer:
	g++ -c $(CONTAINER_SRC)
	g++ -o $(CONTAINER_BIN) $(CONTAINER_OBJ)

######################################
LINKS_SRC = tests/testLinks.cpp links/egLinksType.cpp nodes/egDataNodesType.cpp
LINKS_OBJ = testLinks.o egLinksType.o egDataNodesType.o egDataNodesContainer.o egDataNodesLocalFile.o egByteArray.o egDataNode.o egDataNodeLayout.o
LINKS_BIN = testLinks.exe
testlinks:
	g++ -c $(LINKS_SRC)
	g++ -o $(LINKS_BIN) $(LINKS_OBJ)

######################################
FLEX_LINKS_SRC = tests/testFlexLinks.cpp links/egFlexLinksType.cpp
FLEX_LINKS_OBJ = testFlexLinks.o egFlexLinksType.o egDataNodesType.o egDataNodesContainer.o egDataNodesLocalFile.o egByteArray.o egDataNodeLayout.o egDataNode.o
FLEX_LINKS_BIN = testFlexLinks.exe
testflexlinks:
	g++ -c $(FLEX_LINKS_SRC)
	g++ -o $(FLEX_LINKS_BIN) $(FLEX_LINKS_OBJ)

######################################
DATABASE_SRC = tests/testDatabase.cpp
DATABASE_OBJ = testDatabase.o egByteArray.o egDataNodesContainer.o egDataNodesLocalFile.o egDataNodeLayout.o egDataNode.o
DATABASE_BIN = testDatabase.exe
testdatabase:
	g++ -c $(DATABASE_SRC)
	g++ -o $(DATABASE_BIN) $(DATABASE_OBJ)
# c++ -o testDatabase.exe service/egByteArray.cpp tests/testDatabase.cpp

######################################

FINGERS_SRC = indexes/egFingers.cpp indexes/egIndexes.cpp tests/testFingers.cpp
FINGERS_OBJ = testFingers.o egFingers.o egIndexes.o egByteArray.o egDataNodeLayout.o egDataNode.o
FINGERS_BIN = testFingers.exe

testfingers:
	g++ -c $(FINGERS_SRC)
	g++ -o $(FINGERS_BIN) $(FINGERS_OBJ) $(LDLIBS)

######################################

INDEXES_SRC = indexes/egFingers.cpp indexes/egIndexes.cpp tests/testIndexes.cpp
INDEXES_OBJ = testIndexes.o egFingers.o egIndexes.o egByteArray.o egDataNodeLayout.o egDataNode.o
INDEXES_BIN = testIndexes.exe

testindexes:
	g++ -c $(INDEXES_SRC)
	g++ -o $(INDEXES_BIN) $(INDEXES_OBJ) $(LDLIBS)

# gcc -shared bin/shared/add.o bin/shared/answer.o -o bin/shared/libtq84.so
# gcc  bin/main.o -Lbin/shared -ltq84 -o bin/use-shared-library
# LD_LIBRARY_PATH=$(pwd)/bin/shared bin/use-shared-library
# sudo mv lib$(EGDB_LIB).so /usr/lib
# sudo chmod 755 /usr/lib/lib$(EGDB_LIB).so

clean:
	rm *.o

