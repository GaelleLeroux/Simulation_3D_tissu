[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/ozgGqPZb)
# 5imi simulation
TP noté de simulation de tissu

La simulation du tissu se trouve dans le programme_2

## Compilation

Pour compiler et executer à partir du CMakeLists.txt

```sh
mkdir build
cd build
cmake ..
make
cd ..
./build/programme_2
```

ou 

```sh
mkdir build
cmake . -B build
make -C ./build && ./build/programme_2
```

**Note sur l'utilisation des IDE (QtCreator, etc)**

Le repertoire d'execution doit être ici.
C'est a dire que le repertoire data/ doit être accessible.
