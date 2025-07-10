RED := \033[1;91m
GREEN := \033[1;92m
YELLOW := \033[1;93m
CYAN := \033[1;96m
OFF := \033[0m

CXX = g++
CFLAGS = -Wall -Wextra -pedantic -Werror
LDFLAGS = -lm -lcurl -lpugixml

MAKE_DIR := $(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))

NAME = towers
SRC_PREFIX = $(MAKE_DIR)/src/
O_PREFIX = $(SRC_PREFIX).o_files/

SRC = main.cpp extractor.cpp requests.cpp parser.cpp preprocess.cpp
HEADERS = main.hpp
OBJ = $(addprefix $(O_PREFIX), $(SRC:.cpp=.o))

.PHONY: build build-message debug profiling install docker-reinstall server-build server-run server-stop clean

build: $(NAME)
	@printf "${GREEN}[INFO] Compilation has been successful!\n${OFF}"

build-message:
	@printf "${YELLOW}[INFO] Starting application compilation...\n${OFF}"

$(NAME): $(OBJ)
	$(CXX) $(OBJ) -o $(NAME) $(LDFLAGS)

$(OBJ): | build-message $(O_PREFIX)

$(O_PREFIX):
	mkdir -p $(O_PREFIX)

$(O_PREFIX)%.o: $(SRC_PREFIX)%.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

debug: CFLAGS += -g
debug: build

profiling: CFLAGS += -pg
profiling: LDFLAGS += -pg
profiling: all

install:
	@printf "${YELLOW}[INFO] Checking Docker installation...\n${OFF}"
	@if ! command -v docker >/dev/null 2>&1; then \
		printf "${YELLOW}[INFO] Installing Docker...\n${OFF}"; \
		sudo apt update; \
		sudo apt install -y ca-certificates curl gnupg lsb-release; \
		sudo install -m 0755 -d /etc/apt/keyrings; \
		curl -fsSL https://download.docker.com/linux/ubuntu/gpg | \
		sudo gpg --dearmor -o /etc/apt/keyrings/docker.gpg; \
		echo "deb [arch=$$(dpkg --print-architecture) signed-by=/etc/apt/keyrings/docker.gpg] https://download.docker.com/linux/ubuntu $$(lsb_release -cs) stable" | \
		sudo tee /etc/apt/sources.list.d/docker.list > /dev/null; \
		sudo apt update; \
		sudo apt install -y docker-ce docker-ce-cli containerd.io docker-buildx-plugin docker-compose-plugin; \
		sudo systemctl start docker; \
		sudo systemctl enable docker; \
		printf "${GREEN}[INFO] Docker has been successfuly installed!\n${OFF}"; \
	else \
		printf "${CYAN}[INFO] Docker is already installed!\n${OFF}"; \
	fi
	@printf "${YELLOW}[INFO] Checking libcurl4-openssl-dev installation...\n${OFF}"
	@if ! dpkg -s libcurl4-openssl-dev >/dev/null 2>&1; then \
		printf "${YELLOW}[INFO] Installing libcurl4-openssl-dev...\n${OFF}"; \
		sudo apt update; \
		sudo apt install -y libcurl4-openssl-dev; \
		printf "${GREEN}[INFO] libcurl4-openssl-dev has been successfuly installed!\n${OFF}"; \
	else \
		printf "${CYAN}[INFO] libcurl4-openssl-dev is already installed!\n${OFF}"; \
	fi
	@printf "${YELLOW}[INFO] Checking pugixml installation...\n${OFF}"
	@if ! dpkg -s libpugixml-dev >/dev/null 2>&1; then \
		printf "${YELLOW}[INFO] Installing libpugixml-dev...\n${OFF}"; \
		sudo apt update; \
		sudo apt install -y libpugixml-dev; \
		printf "${GREEN}[INFO] libpugixml-dev has been successfully installed!\n${OFF}"; \
	else \
		printf "${CYAN}[INFO] libpugixml-dev is already installed!\n${OFF}"; \
	fi
	@printf "${YELLOW}[INFO] Checking nlohmann/json installation...\n${OFF}"
	@if ! dpkg -s nlohmann-json3-dev >/dev/null 2>&1; then \
		printf "${YELLOW}[INFO] Installing nlohmann-json3-dev...\n${OFF}"; \
		sudo apt update; \
		sudo apt install -y nlohmann-json3-dev; \
		printf "${GREEN}[INFO] nlohmann-json3-dev has been successfully installed!\n${OFF}"; \
	else \
		printf "${CYAN}[INFO] nlohmann-json3-dev is already installed!\n${OFF}"; \
	fi

docker-reinstall:
	@printf "${YELLOW}[INFO] Reinstalling Docker...\n${OFF}"; \
	sudo systemctl stop docker || true; \
	sudo systemctl stop containerd || true; \
	for pkg in docker.io docker-doc docker-compose docker-compose-v2 podman-docker containerd runc docker-ce docker-ce-cli containerd.io docker-buildx-plugin docker-compose-plugin; do \
		sudo apt-get -y purge $$pkg || true; \
	done; \
	sudo rm -rf /var/lib/docker /var/lib/containerd /etc/docker; \
	sudo rm -rf /etc/apt/keyrings/docker.gpg /etc/apt/sources.list.d/docker.list; \
	sudo apt update; \
	sudo apt install -y ca-certificates curl gnupg lsb-release; \
	sudo install -m 0755 -d /etc/apt/keyrings; \
	curl -fsSL https://download.docker.com/linux/ubuntu/gpg | sudo gpg --dearmor -o /etc/apt/keyrings/docker.gpg; \
	echo "deb [arch=$$(dpkg --print-architecture) signed-by=/etc/apt/keyrings/docker.gpg] https://download.docker.com/linux/ubuntu $$(lsb_release -cs) stable" | \
		sudo tee /etc/apt/sources.list.d/docker.list > /dev/null; \
	sudo apt update; \
	sudo apt install -y docker-ce docker-ce-cli containerd.io docker-buildx-plugin docker-compose-plugin; \
	sudo systemctl enable docker; \
	sudo systemctl start docker; \
	printf "${GREEN}[INFO] Docker has been successfully reinstalled!\n${OFF}";

OSM_URL=https://download.geofabrik.de/europe/czech-republic-latest.osm.pbf
OSM_FILE=czech-republic-latest.osm
OSRM_FILE=czech-republic-latest.osrm
OSRM_IMAGE=osrm/osrm-backend
PROFILE=/opt/foot.lua

server-build:
	@printf "${YELLOW}[INFO] Starting server build...\n${OFF}"; \
	DATA_DIR="$(MAKE_DIR)/server/data"; \
	OTD_DIR="$(MAKE_DIR)/server/opentopodata"; \
	mkdir -p "$$DATA_DIR"; \
	cd "$$DATA_DIR"; \
	if [ ! -f "$$DATA_DIR/$(OSM_FILE).pbf" ]; then \
		printf "${YELLOW}[INFO] Downloading OSM data...\n${OFF}"; \
		wget "$(OSM_URL)"; \
		printf "${GREEN}[INFO] OSM data has been downloaded...\n${OFF}"; \
	fi; \
	if [ ! -f "$$DATA_DIR/$(OSRM_FILE)" ]; then \
		printf "${YELLOW}[INFO] Running osrm-extract...\n${OFF}"; \
		sudo docker run -t -v "$$DATA_DIR:/data" $(OSRM_IMAGE) osrm-extract -p $(PROFILE) /data/$(OSRM_FILE).pbf; \
		printf "${GREEN}[INFO] osrm-extraction run has been successful!\n${OFF}"; \
	fi; \
	if [ ! -f "$$DATA_DIR/$(OSRM_FILE).partition" ]; then \
		printf "${YELLOW}[INFO] Running osrm-partition...\n${OFF}"; \
		sudo docker run -t -v "$$DATA_DIR:/data" $(OSRM_IMAGE) osrm-partition /data/$(OSRM_FILE); \
		printf "${GREEN}[INFO] osrm-partition run has been successful!\n${OFF}"; \
	fi; \
	if [ ! -f "$$DATA_DIR/$(OSRM_FILE).datasource_names" ]; then \
		printf "${YELLOW}[INFO] Running osrm-customize...\n${OFF}"; \
		sudo docker run -t -v "$$DATA_DIR:/data" $(OSRM_IMAGE) osrm-customize /data/$(OSRM_FILE); \
		printf "${GREEN}[INFO] osrm-customize run has been successful!\n${OFF}"; \
	fi; \
	mkdir -p "$$OTD_DIR"; \
	cd "$$OTD_DIR"; \
	if [ ! -d ".git" ]; then \
		printf "${YELLOW}[INFO] Cloning OpenTopodata repository...\n${OFF}"; \
		git clone https://github.com/ajnisbet/opentopodata.git .; \
		printf "${GREEN}[INFO] OpenTopodata repository has been cloned!\n${OFF}"; \
	fi; \
	sudo docker compose build; \
	printf "${GREEN}[INFO] Server has been built!\n${OFF}"

server-run:
	@printf "${YELLOW}[INFO] Running server..\n${OFF}."; \
	cd "$(MAKE_DIR)/server"; \
	sudo docker compose up -d; \
	printf "${GREEN}[INFO] Server is running!\n${OFF}"

server-stop:
	@printf "${YELLOW}[INFO] Stopping server..\n${OFF}."; \
	cd "$(MAKE_DIR)/server"; \
	sudo docker compose stop; \
	printf "${GREEN}[INFO] Server has been stopped!\n${OFF}"

clean:
	@printf "${YELLOW}[INFO] Cleaning object files...\n${OFF}";
	@echo rm -rf $(O_PREFIX) towers; \
	rm -rf $(O_PREFIX) towers; \
	printf "${GREEN}[INFO] Object files have been cleaned!\n${OFF}";