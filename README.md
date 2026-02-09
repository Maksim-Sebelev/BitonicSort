# Всем import std!

# Bitonic sort

## Краткое описание сути проекта:
В данной библиотекек реализован алгоритм сортировки под называнием `bitonic sort`. По ассимптотической слжоности он немного уступает классическим алгоритмам сортировки: O(N log<sup>2</sup>(N)), против классических O(N log(N)). Однако в силу оссобености его реализации он крайне хорошо поддается распараллеливанию и идеально подходит для вычислений на GPU.

Доступны 2 версии проекта: modules-based и header-only

## Установка:

```bash
git clone https://github.com/Maksim-Sebelev/BitonicSort.git;
cd BitonicSort;
```

## Для modules-based и если в header-only версии вам нужны тесты и бенчмарак:

```bash
chmod +x bootsrtap install;
./bootstrap # [OPTIONS];
./install
```

Для modules-based добавьте опцию `--cxx-20-support`

Для `bootstrap` можно почитать об опциях по флагу `--help`

Вероятно, для обычного пользования вам подойдет такой набор:
```bash
./bootstrap --build-inplace --build-tests --benchmark --cmake-export-compile-commands
```

Если хотите получить тестирование и провести бенчмарк на своем устройстве, то укажите следующие опции: `--build-tests` и  `--benchmark`

```bash
./bootstrap --build-inplace 
```

## Если вам просто достаточно hader-only библиотеки:

Чтобы добавить сортировку к вашему проекту, то достаточно указать путь до include директории проекта и слинковаться с OpenCL в вашем проекте.


# Рекомендованные опции уставновки:

Для modules-based:

```bash
./bootstrap --build-inplace --build-tests --benchmark --cmake-export-compile-commands --cxx-20-support
```

Для header-only:
```bash
./bootstrap --build-inplace --build-tests --bechmark
```

<br>
<br>




Запуск тестов:
```bash
ctest --test-dir build
```

Бенчмарк (сравнение со std::sort):
```bash
build/run-benchmark
```

## Использование

sort::bitonic::sort в modules-based версии реализует интерфейс почти аналогичный std::sort:
```cpp
template<typename T>
concept BitonicSortSupportedType = 
    std::same_as<T, int> || 
    std::same_as<T, float> || 
    std::same_as<T, double>;

template <typename It>
concept BitonicSortIteratorConcept =
    std::random_access_iterator<It> && 
    BitonicSortSupportedType<typename It::value_type>;

template <BitonicSortIteratorConcept It>
void sort(It begin, It end)
```

Как видно, не все типы поддерижаются, это связано со сложностью обеспечения работы с произвольным типом в OpenCL.

Для header-only все аналогично, только отсуствуют концепты, потому будьте аккуратны с типами. Не забывайте, что сортировка ваших типов точно не будет работать.

<br>

Чтобы использовать функцию в вашем проекте, используйте:

```cpp
import bitonic_sort;
```
или
```cpp
#include "sort/bitonic/sort.hpp"
```

## Сравнение с std::sort и бенчмарк

Об это можно почитать [тут](./tests/benchmark/results/README.md)
