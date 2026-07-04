# tarea2AnalisisDeAlgoritmos2026

Implementación en C++ del problema de Distancia Mínima entre todo par de nodos
(APSP), comparando el **Algoritmo Base** (Bellman-Ford ejecutado una vez por
cada nodo origen) contra **Floyd-Warshall**.

## Estructura del proyecto

```
src/            Clase Graph (matriz de adyacencia + lectura de .mtx) y las
                librerías de ambos algoritmos (BaseAPSPLib, WarshallLib),
                más utilidades (Edge.h, InstanceIO).
demos/          Demostraciones independientes de cada algoritmo con casos de
                prueba pequeños y resultados esperados documentados.
generators/     Generadores de instancias para los 4 experimentos del diseño
                experimental (ver comentarios en cada archivo para la
                justificación de los tamaños de n/m elegidos).
harness/        Arnés de experimentos (mide ambos algoritmos sobre las
                instancias generadas y reporta tiempo promedio, desviación
                estándar y cuartiles) y el programa que verifica que ambas
                implementaciones coincidan sobre una misma instancia.
uhr/            Suite de medición de tiempos sugerida en el enunciado
                (copiada tal cual desde github.com/leonardlover/uhr); el
                arnés usa directamente su función quartiles().
instances/      Instancias generadas (.mtx, formato Matrix Market) y un
                manifest.csv por experimento. Deterministas (seeds fijas):
                se pueden regenerar con `make instances`.
results/        CSV con los tiempos medidos por el arnés (se generan al
                correr los experimentos; no se versionan salvo que se quiera
                dejar constancia de la corrida final del informe).
notebooks/      Notebook de Google Colab que clona el repo, compila, genera
                instancias, corre los 4 experimentos y grafica los
                resultados.
real_datasets/  Programa que corre Floyd-Warshall sobre el dataset real
                Chebyshev2 y escribe la solución en un CSV
                (nodo1,nodo2,distancia), como pide el enunciado.
Chebyshev2/     Dataset real (networkrepository.com) usado por
                real_datasets/.
```

## Compilar y correr

Abrir [notebooks/ejecutar_experimentos.ipynb](notebooks/ejecutar_experimentos.ipynb)
en Google Colab y correr las celdas en orden: clona el repo, compila con
`make`, reporta las especificaciones de la máquina, genera las instancias,
verifica que ambas implementaciones coincidan, corre los 4 experimentos con
>= 32 repeticiones y grafica los resultados en escala lineal junto a las
curvas teóricas ajustadas.

Los resultados quedan en `results/resultados_exp{1,2,3,4}.csv`.

`make verificar ARCHIVO=<ruta.mtx>` corre el Algoritmo Base y Floyd-Warshall
sobre la misma instancia y compara sus matrices de distancia, para verificar
que ambas implementaciones sean correctas entre sí (además de los casos de
prueba pequeños en `demos/`).

`make chebyshev2` corre Floyd-Warshall sobre el dataset real y escribe
`results/chebyshev2_distancias.csv` (una fila `nodo1,nodo2,distancia` por
cada par de nodos). Los pesos originales de Chebyshev2 no representan
distancias (es una matriz de álgebra lineal) y su diagonal tiene una
entrada negativa, así que el programa trata el grafo como no ponderado
(cada arista con peso 1) para obtener una solución con sentido.
