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
harness/        Arnés de experimentos: mide ambos algoritmos sobre las
                instancias generadas y reporta tiempo promedio, desviación
                estándar y cuartiles.
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
Chebyshev2/     Dataset real (networkrepository.com) para la sección de
                Floyd-Warshall sobre datasets reales.
```

## Compilar y correr

Abrir [notebooks/ejecutar_experimentos.ipynb](notebooks/ejecutar_experimentos.ipynb)
en Google Colab y correr las celdas en orden: clona el repo, compila con
`make`, genera las instancias, corre los 4 experimentos con >= 32
repeticiones y grafica los resultados en escala lineal junto a las curvas
teóricas ajustadas.

Los resultados quedan en `results/resultados_exp{1,2,3,4}.csv`.
