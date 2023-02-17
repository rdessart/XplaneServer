VCPKG_INCLUDES  := /Users/romaindessart/vcpkg/installed/arm64-osx/include/
# INCLUDE := -I $(xplane_sdk)/CHeaders/ -I$(VCPKG_INCLUDES)
INCLUDE := -I /Users/romaindessart/SDKs/X-Plane/SDK400b1/CHeaders/ -I$(VCPKG_INCLUDES)
# FRAMEWORK := -F $(xplane_sdk)/Libraries/Mac -framework XPLM
FRAMEWORK := -F /Users/romaindessart/SDKs/X-Plane/SDK400b1/Libraries/Mac -framework XPLM
DEFINES := -DAPL=1 -DXPLM200=1 -DXPLM210=1 -DXPLM300=1 -DXPLM301=1 -DXPLM303=1
# CXX_FLAGS := -g --std=c++20 $(DEFINES) $(INCLUDE) -fPIC -fvisibility=hidden -arch x86_64 
CXX_FLAGS := -g --std=c++20 $(DEFINES) $(INCLUDE) -fPIC -fvisibility=hidden -arch arm64
BUILD_DIR := ./build
SRC_DIR := ./XplaneServer/src


$(BUILD_DIR)/mac.xpl : $(BUILD_DIR)/main.o $(BUILD_DIR)/UDPBeacon.o $(BUILD_DIR)/UDPServer.o $(BUILD_DIR)/NetworkUtils.o\
				$(BUILD_DIR)/Logger.o $(BUILD_DIR)/Dataref.o build/DatarefManager.o | $(BUILD_DIR)

	g++ $(CXX_FLAGS) -m64 -dynamiclib -Wl, $(BUILD_DIR)/main.o $(BUILD_DIR)/UDPBeacon.o $(BUILD_DIR)/UDPServer.o $(BUILD_DIR)/NetworkUtils.o \
	$(BUILD_DIR)/Logger.o $(BUILD_DIR)/Dataref.o build/DatarefManager.o -o build/mac.xpl $(FRAMEWORK)

$(BUILD_DIR)/main.o : $(SRC_DIR)/main.cpp | $(BUILD_DIR)
	g++ -c $(CXX_FLAGS) $(SRC_DIR)/main.cpp -o build/main.o

$(BUILD_DIR)/UDPBeacon.o : $(SRC_DIR)/Network/UDPBeacon.h $(SRC_DIR)/Network/UDPBeacon.cpp | $(BUILD_DIR)
	g++ -c $(CXX_FLAGS) $(SRC_DIR)/Network/UDPBeacon.cpp -o build/UDPBeacon.o

$(BUILD_DIR)/UDPServer.o : $(SRC_DIR)/Network/UDPServer.h $(SRC_DIR)/Network/UDPServer.cpp | $(BUILD_DIR)
	g++ -c $(CXX_FLAGS) $(SRC_DIR)/Network/UDPServer.cpp -o build/UDPServer.o

$(BUILD_DIR)/NetworkUtils.o : $(SRC_DIR)/Network/NetworkUtils.h $(SRC_DIR)/Network/NetworkUtils.cpp | $(BUILD_DIR)
	g++ -c $(CXX_FLAGS) $(SRC_DIR)/Network/NetworkUtils.cpp -o build/NetworkUtils.o

$(BUILD_DIR)/Logger.o : $(SRC_DIR)/Tools/Logger.cpp $(SRC_DIR)/Tools/Logger.h | $(BUILD_DIR)
	g++ -c $(CXX_FLAGS) $(SRC_DIR)/Tools/Logger.cpp -o build/Logger.o

$(BUILD_DIR)/Dataref.o : $(SRC_DIR)/Datarefs/Dataref.cpp $(SRC_DIR)/Datarefs/Dataref.h $(SRC_DIR)/Datarefs/AbstractDataref.h | $(BUILD_DIR)
	g++ -c $(CXX_FLAGS) $(SRC_DIR)/Datarefs/Dataref.cpp -o build/Dataref.o

$(BUILD_DIR)/DatarefManager.o : $(SRC_DIR)/Datarefs/DatarefManager.cpp $(SRC_DIR)/Datarefs/DatarefManager.h $(SRC_DIR)/Datarefs/AbstractDataref.h | $(BUILD_DIR)
	g++ -c $(CXX_FLAGS) $(SRC_DIR)/Datarefs/DatarefManager.cpp -o build/DatarefManager.o

# $(BUILD_DIR)/FFDataref.o : $(SRC_DIR)/Datarefs/FFDataref.cpp $(SRC_DIR)/Datarefs/FFDataref.h $(SRC_DIR)/Datarefs/AbstractDataref.h | $(BUILD_DIR)
# 	g++ -c $(CXX_FLAGS) $(SRC_DIR)/Datarefs/FFDataref.cpp -o build/FFDataref.o

publish:
	# cp build/mac.xpl "$(xplane_dir)/Resources/plugins/XPLMServer/64"
	cp build/mac.xpl "/Volumes/DESSROM_SSD/X-Plane_12/Resources/plugins/XPLMServer/64"

$(BUILD_DIR):
	mkdir build/

clean:
	rm -r ./build/