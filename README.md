# Bitonic sort


## Краткое описание сути проекта:
В данной библиотекек реализован алгоритм сортировки под называнием `bitonic sort`. По ассимптотической слжоности он немного уступает классическим алгоритмам сортировки: O(N log<sup>2</sup>(N)), против классических O(N log(N)). Однако в силу оссобености его реализации он крайне хорошо поддается распараллеливанию и идеально подходит для вычислений на GPU.


## Установка:

```bash
git clone https://github.com/Maksim-Sebelev/BitonicSort.git;
chmod +x bootsrtap install;
./bootstrap # [OPTIONS];
./install
```

Для `bootstrap` можно почитать об опциях по флагу `--help`

Вероятно, для обычного пользования вам подойдет такой набор:
```bash
./bootstrap --build-inplace
```

Если хотите получить тестирование и провести бенчмарк на своем устройстве, то укажите следующие опции:

```bash
./bootstrap --build-inplace --build-tests --benchmark
```

Запуск тестов:
```bash
ctest --test-dir build
```

Бенчмарк (сравнение со std::sort):
```bash
build/run-benchmark
```

## Использование

sort::bitonic::sort реализует интерфейс почти аналогичный std::sort:
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

<br>

Чтобы использовать функцию в вашем проекте, используйте:

```cpp
import bitonic_sort;
```


## Сравнение с std::sort

На моем устройсте, результаты бенмарка таковы (время в ms, соотвествующие тесты одинаковы):
```log
Elements: 10
Bitonic: 
56 49 48 30 39 46 27 27 27 
std::sort: 
0 0 0 0 0 0 0 0 0 

Elements: 100
Bitonic: 
29 38 49 44 44 28 28 28 35 
std::sort: 
0 0 0 0 0 0 0 0 0 

Elements: 1000
Bitonic: 
40 43 48 48 47 49 31 42 51 
std::sort: 
0 0 0 0 0 0 0 0 0 

Elements: 10000
Bitonic: 
53 50 51 50 31 42 32 46 54 
std::sort: 
0 0 0 0 0 0 0 0 1 

Elements: 100000
Bitonic: 
36 37 39 38 37 34 39 37 38 
std::sort: 
6 10 6 7 7 9 6 7 7 

Elements: 1000000
Bitonic: 
50 51 50 51 55 50 53 51 50 
std::sort: 
75 73 75 76 74 75 74 75 75 

Elements: 10000000
Bitonic: 
680 680 700 679 677 672 686 677 678 
std::sort: 
877 882 885 867 885 883 877 883 875 

Elements: 100000000
Bitonic: 
687 680 6900 6970 7077 7067 6851 6978 6988 
std::sort: 
889 880 10130 10108 10098 10071 10090 10099 10094
```

Как видно, при количестве сортируемых элементов от 1000000, битонная сортировка обгоняет std::sort.