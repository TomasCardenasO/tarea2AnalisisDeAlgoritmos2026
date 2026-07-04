# Makefile del proyecto (Algoritmo Base vs Floyd-Warshall para el problema
# de Distancia Minima entre todo par de nodos).

CXX      := g++
STD      := -std=c++17
OPT      := -O2
CXXFLAGS := $(STD) $(OPT) -Wall -Isrc

BIN      := bin
INSTANCES:= instances
RESULTS  := results

SRC_LIB  := src/Graph.cpp src/WarshallLib.cpp src/BaseAPSPLib.cpp src/InstanceIO.cpp

RUNS     ?= 32

.PHONY: all generators harness demos instances chebyshev2 verificar \
        run-exp1 run-exp2 run-exp3 run-exp4 run-all clean

all: generators harness demos

$(BIN):
	mkdir -p $(BIN)

# --- Generadores de instancias ---
generators: $(BIN)
	$(CXX) $(STD) $(OPT) -o $(BIN)/generarInstanciasExp1 generators/generarInstanciasExp1.cpp
	$(CXX) $(STD) $(OPT) -o $(BIN)/generarInstanciasExp2 generators/generarInstanciasExp2.cpp
	$(CXX) $(STD) $(OPT) -o $(BIN)/generarInstanciasExp3 generators/generarInstanciasExp3.cpp
	$(CXX) $(STD) $(OPT) -o $(BIN)/generarInstanciasExp4 generators/generarInstanciasExp4.cpp

# Genera las instancias de los 4 experimentos (idempotente: sobreescribe).
instances: generators
	mkdir -p $(INSTANCES)/exp1_densos $(INSTANCES)/exp2_dispersos \
	         $(INSTANCES)/exp3_densidad $(INSTANCES)/exp4_topologia
	./$(BIN)/generarInstanciasExp1
	./$(BIN)/generarInstanciasExp2
	./$(BIN)/generarInstanciasExp3
	./$(BIN)/generarInstanciasExp4

# --- Arnes de experimentos (mide ambos algoritmos sobre las instancias) ---
harness: $(BIN)
	$(CXX) $(CXXFLAGS) -o $(BIN)/arnesExperimentos harness/arnesExperimentos.cpp $(SRC_LIB)

# --- Demos individuales de cada algoritmo (para inspeccion/depuracion) ---
demos: $(BIN)
	$(CXX) $(CXXFLAGS) -o $(BIN)/demoFloydWarshall demos/main.cpp src/Graph.cpp
	$(CXX) $(STD) $(OPT) -o $(BIN)/demoAlgoritmoBase demos/algoritmoBase.cpp

# Compara Floyd-Warshall contra el Algoritmo Base sobre una misma instancia
# real, para verificar que ambas implementaciones coinciden (uso:
# make verificar ARCHIVO=instances/exp1_densos/dense_n20.mtx).
ARCHIVO ?= instances/exp1_densos/dense_n20.mtx
verificar: $(BIN) instances
	$(CXX) $(CXXFLAGS) -o $(BIN)/verificarCorrectitud harness/verificarCorrectitud.cpp $(SRC_LIB)
	./$(BIN)/verificarCorrectitud $(ARCHIVO)

# --- Corridas de los 4 experimentos (RUNS >= 32 segun enunciado) ---
run-exp1: harness instances
	mkdir -p $(RESULTS)
	./$(BIN)/arnesExperimentos $(INSTANCES)/exp1_densos/manifest.csv $(RUNS) $(RESULTS)/resultados_exp1.csv

run-exp2: harness instances
	mkdir -p $(RESULTS)
	./$(BIN)/arnesExperimentos $(INSTANCES)/exp2_dispersos/manifest.csv $(RUNS) $(RESULTS)/resultados_exp2.csv

run-exp3: harness instances
	mkdir -p $(RESULTS)
	./$(BIN)/arnesExperimentos $(INSTANCES)/exp3_densidad/manifest.csv $(RUNS) $(RESULTS)/resultados_exp3.csv

run-exp4: harness instances
	mkdir -p $(RESULTS)
	./$(BIN)/arnesExperimentos $(INSTANCES)/exp4_topologia/manifest.csv $(RUNS) $(RESULTS)/resultados_exp4.csv

run-all: run-exp1 run-exp2 run-exp3 run-exp4

# --- Floyd-Warshall sobre el dataset real Chebyshev2 ---
chebyshev2: $(BIN)
	$(CXX) $(CXXFLAGS) -o $(BIN)/correrChebyshev2 real_datasets/correrChebyshev2.cpp src/Graph.cpp src/WarshallLib.cpp
	mkdir -p $(RESULTS)
	./$(BIN)/correrChebyshev2

clean:
	rm -rf $(BIN)

distclean: clean
	rm -rf $(RESULTS)
