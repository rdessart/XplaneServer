# XPLANE_SDK := $(xplane_sdk)
XPLANE_SDK := /Users/romaindessart/SDKs/X-Plane/SDK400b1

VCPKG_INCLUDES  := /Users/romaindessart/vcpkg/installed/arm64-osx/include/
INCLUDE := -I $(XPLANE_SDK)/CHeaders/ -I$(VCPKG_INCLUDES)
FRAMEWORK := -F $(XPLANE_SDK)/Libraries/Mac -framework XPLM
DEFINES := -DAPL=1 -DXPLM200=1 -DXPLM210=1 -DXPLM300=1 -DXPLM301=1 -DXPLM303=1
CXX_FLAGS := -Wno-int-to-void-pointer-cast -g --std=c++20 $(DEFINES) $(INCLUDE) -fPIC -fvisibility=hidden -arch arm64
BUILD_DIR := ./build
SRC_DIR := ./XplaneServer/src


$(BUILD_DIR)/mac.xpl : $(BUILD_DIR)/main.o $(BUILD_DIR)/UDPBeacon.o $(BUILD_DIR)/UDPServer.o $(BUILD_DIR)/NetworkUtils.o\
				$(BUILD_DIR)/Logger.o $(BUILD_DIR)/Dataref.o $(BUILD_DIR)/DatarefManager.o $(BUILD_DIR)/FFA320Dataref.o $(BUILD_DIR)/Utils.o | $(BUILD_DIR)

	clang++ $(CXX_FLAGS) -m64 -dynamiclib -Wl, $(BUILD_DIR)/main.o $(BUILD_DIR)/UDPBeacon.o $(BUILD_DIR)/UDPServer.o $(BUILD_DIR)/NetworkUtils.o \
	$(BUILD_DIR)/Logger.o $(BUILD_DIR)/Dataref.o $(BUILD_DIR)/DatarefManager.o $(BUILD_DIR)/FFA320Dataref.o $(BUILD_DIR)/Utils.o -o build/mac.xpl $(FRAMEWORK)

$(BUILD_DIR)/main.o : $(SRC_DIR)/main.cpp | $(BUILD_DIR)
	clang++ -c $(CXX_FLAGS) $(SRC_DIR)/main.cpp -o build/main.o

$(BUILD_DIR)/UDPBeacon.o : $(SRC_DIR)/Network/UDPBeacon.h $(SRC_DIR)/Network/UDPBeacon.cpp | $(BUILD_DIR)
	clang++ -c $(CXX_FLAGS) $(SRC_DIR)/Network/UDPBeacon.cpp -o build/UDPBeacon.o

$(BUILD_DIR)/UDPServer.o : $(SRC_DIR)/Network/UDPServer.h $(SRC_DIR)/Network/UDPServer.cpp | $(BUILD_DIR)
	clang++ -c $(CXX_FLAGS) $(SRC_DIR)/Network/UDPServer.cpp -o build/UDPServer.o

$(BUILD_DIR)/NetworkUtils.o : $(SRC_DIR)/Network/NetworkUtils.h $(SRC_DIR)/Network/NetworkUtils.cpp | $(BUILD_DIR)
	clang++ -c $(CXX_FLAGS) $(SRC_DIR)/Network/NetworkUtils.cpp -o build/NetworkUtils.o

$(BUILD_DIR)/Logger.o : $(SRC_DIR)/Tools/Logger.cpp $(SRC_DIR)/Tools/Logger.h | $(BUILD_DIR)
	clang++ -c $(CXX_FLAGS) $(SRC_DIR)/Tools/Logger.cpp -o build/Logger.o

$(BUILD_DIR)/Dataref.o : $(SRC_DIR)/Datarefs/Dataref.cpp $(SRC_DIR)/Datarefs/Dataref.h $(SRC_DIR)/Datarefs/AbstractDataref.h | $(BUILD_DIR)
	clang++ -c $(CXX_FLAGS) $(SRC_DIR)/Datarefs/Dataref.cpp -o build/Dataref.o

$(BUILD_DIR)/DatarefManager.o : $(SRC_DIR)/Datarefs/DatarefManager.cpp $(SRC_DIR)/Datarefs/DatarefManager.h $(SRC_DIR)/Datarefs/AbstractDataref.h | $(BUILD_DIR)
	clang++ -c $(CXX_FLAGS) $(SRC_DIR)/Datarefs/DatarefManager.cpp -o build/DatarefManager.o

$(BUILD_DIR)/FFA320Dataref.o : $(SRC_DIR)/Datarefs/FFA320Dataref.cpp $(SRC_DIR)/Datarefs/FFA320Dataref.h $(SRC_DIR)/Datarefs/AbstractDataref.h | $(BUILD_DIR)
	clang++ -c $(CXX_FLAGS) $(SRC_DIR)/Datarefs/FFA320Dataref.cpp -o build/FFA320Dataref.o

$(BUILD_DIR)/Utils.o : $(SRC_DIR)/Tools/Utils.cpp $(SRC_DIR)/Tools/Utils.h | $(BUILD_DIR)
	clang++ -c $(CXX_FLAGS) $(SRC_DIR)/Tools/Utils.cpp -o build/Utils.o

publish:
	mv build/mac.xpl "$(xplane_dir)/Resources/plugins/XPLMServer/64"

$(BUILD_DIR):
	mkdir build/

clean:
	rm -r ./build/
