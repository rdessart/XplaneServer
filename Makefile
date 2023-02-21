# XPLANE_SDK := $(xplane_sdk)
ARCH := arm64
XPLANE_SDK := /Users/romaindessart/SDKs/X-Plane/SDK400b1


FRAMEWORK := -F $(XPLANE_SDK)/Libraries/Mac -framework XPLM
DEFINES := -DAPL=1 -DXPLM200=1 -DXPLM210=1 -DXPLM300=1 -DXPLM301=1 -DXPLM303=1
PUBLISH_DIR := ./publish/XplaneServer
SRC_DIR := ./XplaneServer/src
INCLUDE := -I $(XPLANE_SDK)/CHeaders/ 
ifeq ($(ARCH), x64)
VCPKG_INCLUDES  := /Users/romaindessart/vcpkg/installed/x64-osx/include/
CXX_FLAGS := -Wno-int-to-void-pointer-cast -g --std=c++20 $(DEFINES) $(INCLUDE) -I$(VCPKG_INCLUDES) -fPIC -fvisibility=hidden -arch x86_64
BUILD_DIR := ./build/64/
else
VCPKG_INCLUDES  := /Users/romaindessart/vcpkg/installed/arm64-osx/include/
CXX_FLAGS := -Wno-int-to-void-pointer-cast -g --std=c++20 $(DEFINES) $(INCLUDE) -I$(VCPKG_INCLUDES) -fPIC -fvisibility=hidden -arch arm64
BUILD_DIR := ./build/arm64/
endif


$(BUILD_DIR)/mac.xpl : $(BUILD_DIR)/main.o $(BUILD_DIR)/UDPBeacon.o $(BUILD_DIR)/UDPServer.o $(BUILD_DIR)/NetworkUtils.o\
				$(BUILD_DIR)/Logger.o $(BUILD_DIR)/Dataref.o $(BUILD_DIR)/DatarefManager.o $(BUILD_DIR)/FFA320Dataref.o $(BUILD_DIR)/Utils.o | $(BUILD_DIR)

	clang++ $(CXX_FLAGS) -m64 -dynamiclib -Wl, $(BUILD_DIR)/main.o $(BUILD_DIR)/UDPBeacon.o $(BUILD_DIR)/UDPServer.o $(BUILD_DIR)/NetworkUtils.o \
	$(BUILD_DIR)/Logger.o $(BUILD_DIR)/Dataref.o $(BUILD_DIR)/DatarefManager.o $(BUILD_DIR)/FFA320Dataref.o $(BUILD_DIR)/Utils.o -o $(BUILD_DIR)mac.xpl $(FRAMEWORK)

$(BUILD_DIR)/main.o : $(SRC_DIR)/main.cpp | $(BUILD_DIR)
	clang++ -c $(CXX_FLAGS) $(SRC_DIR)/main.cpp -o $(BUILD_DIR)main.o

$(BUILD_DIR)/UDPBeacon.o : $(SRC_DIR)/Network/UDPBeacon.h $(SRC_DIR)/Network/UDPBeacon.cpp | $(BUILD_DIR)
	clang++ -c $(CXX_FLAGS) $(SRC_DIR)/Network/UDPBeacon.cpp -o $(BUILD_DIR)UDPBeacon.o

$(BUILD_DIR)/UDPServer.o : $(SRC_DIR)/Network/UDPServer.h $(SRC_DIR)/Network/UDPServer.cpp $(SRC_DIR)/Network/Message.h | $(BUILD_DIR)
	clang++ -c $(CXX_FLAGS) $(SRC_DIR)/Network/UDPServer.cpp -o $(BUILD_DIR)UDPServer.o

$(BUILD_DIR)/NetworkUtils.o : $(SRC_DIR)/Network/NetworkUtils.h $(SRC_DIR)/Network/NetworkUtils.cpp | $(BUILD_DIR)
	clang++ -c $(CXX_FLAGS) $(SRC_DIR)/Network/NetworkUtils.cpp -o $(BUILD_DIR)NetworkUtils.o

$(BUILD_DIR)/Logger.o : $(SRC_DIR)/Tools/Logger.cpp $(SRC_DIR)/Tools/Logger.h | $(BUILD_DIR)
	clang++ -c $(CXX_FLAGS) $(SRC_DIR)/Tools/Logger.cpp -o $(BUILD_DIR)Logger.o

$(BUILD_DIR)/Dataref.o : $(SRC_DIR)/Datarefs/Dataref.cpp $(SRC_DIR)/Datarefs/Dataref.h $(SRC_DIR)/Datarefs/AbstractDataref.h | $(BUILD_DIR)
	clang++ -c $(CXX_FLAGS) $(SRC_DIR)/Datarefs/Dataref.cpp -o $(BUILD_DIR)Dataref.o

$(BUILD_DIR)/DatarefManager.o : $(SRC_DIR)/Datarefs/DatarefManager.cpp $(SRC_DIR)/Datarefs/DatarefManager.h $(SRC_DIR)/Datarefs/AbstractDataref.h $(SRC_DIR)/Network/Message.h| $(BUILD_DIR)
	clang++ -c $(CXX_FLAGS) $(SRC_DIR)/Datarefs/DatarefManager.cpp -o $(BUILD_DIR)DatarefManager.o

$(BUILD_DIR)/FFA320Dataref.o : $(SRC_DIR)/Datarefs/FFA320Dataref.cpp $(SRC_DIR)/Datarefs/FFA320Dataref.h $(SRC_DIR)/Datarefs/AbstractDataref.h | $(BUILD_DIR)
	clang++ -c $(CXX_FLAGS) $(SRC_DIR)/Datarefs/FFA320Dataref.cpp -o $(BUILD_DIR)FFA320Dataref.o

$(BUILD_DIR)/Utils.o : $(SRC_DIR)/Tools/Utils.cpp $(SRC_DIR)/Tools/Utils.h | $(BUILD_DIR)
	clang++ -c $(CXX_FLAGS) $(SRC_DIR)/Tools/Utils.cpp -o $(BUILD_DIR)Utils.o

ifeq ($(ARCH), x64)
publish: $(BUILD_DIR)/mac.xpl
	mkdir -p $(PUBLISH_DIR)/64 $(PUBLISH_DIR)/mac_x64 $(PUBLISH_DIR)/mac_arm64
	cp $(BUILD_DIR)mac.xpl $(PUBLISH_DIR)/64
	cp $(BUILD_DIR)mac.xpl $(PUBLISH_DIR)/mac_x64/
	mv $(PUBLISH_DIR)/mac_x64/mac.xpl $(PUBLISH_DIR)/mac_x64/XplaneServer.xpl
else
publish: $(BUILD_DIR)/mac.xpl
	mkdir -p $(PUBLISH_DIR)/64 $(PUBLISH_DIR)/mac_x64 $(PUBLISH_DIR)/mac_arm64
	cp $(BUILD_DIR)mac.xpl $(PUBLISH_DIR)/mac_arm64/
	mv $(PUBLISH_DIR)/mac_arm64/mac.xpl $(PUBLISH_DIR)/mac_arm64/XplaneServer.xpl
endif

release:
	7zz a XplaneServer.zip $(PUBLISH_DIR)
	rm -r $(PUBLISH_DIR)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -r ./build/
