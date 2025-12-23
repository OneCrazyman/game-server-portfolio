## 프로젝트 구성
### Tree-RB-tree/
- 레드-블랙 트리를 구현한 프로젝트
- 삽입,삭제,검색 그리고 삽입시 밸런싱 및 회전 로직과 레드-블랙트리의 5가지 조건을 만족하는지 단위테스트를 포함한다.

### Tree-binary-tree/
- 기본적인 이진 탐색트리를 구현
- 밸런싱 로직이 없는 BST로 RBT와 성능을 비교하기 위해 구현, 똑같이 단위테스트를 포함한다.

### BST-vs-RBT-benchmark/
- 동일한 입력 조건에서 BST와 RBT의 Insert 및 Search 성능을 측정하고 비교하였다.
- 입력 크기 N, 입력 데이터(Random 및 Sorted) 에 따른 기준으로 성능을 비교하였다.
<br>

## 구현된 트리 시연

### 레드-블랙 트리
![Image](https://github.com/user-attachments/assets/14a32edc-45ac-42f7-89ef-4dbf97326865)
> 레드-블랙 트리 시연


![Animation](https://github.com/user-attachments/assets/a13ddb3b-1f17-42a0-825b-fe6479600bee)
> 단위테스트
<br> 

### 이진 트리
![Animation (2)](https://github.com/user-attachments/assets/121e9177-4c47-41d4-831f-d9f18153e200)
> 이진 트리 시연


<img width="939" height="617" alt="Image" src="https://github.com/user-attachments/assets/fa6953d2-ac87-4433-99b6-0ef93e69f2eb"/>

> 단위테스트
<br>

## 비교 테스트 (INSERT,SEARCH)
입력 크기 N을 **100, 500, 1'000, 5'000, 10'000**으로 증가시키며 성능 테스트를 진행하였다.

입력 데이터는 **Random Set**, **Sorted Set** 두가지로 구성하였으며, 각 경우 모두 **중복 없는 키 집합**을 사용하였다.

* `Random Set` : 1 ~ N 범위의 정수가 무작위로 섞인 입력
* `Sorted Set` : 1 ~ N 범위의 정수가 오름차순으로 정렬된 입력  
(이진 탐색 트리의 최악의 경우를 가정한 입력)
<br>

### `Random Set일때 Insert`

<img width="700" height="450" alt="n_sweep_Insert_random_loglog" src="https://github.com/user-attachments/assets/5477254c-647c-45f9-ae5a-b8f4ff52ebf6" />

- 모든 N 구간에서 두 트리는 **유사한 성능을** 보였다.
- 두 자료구조 모두 키 비교를 통해 삽입 위치를 탐색하는 과정은 동일하다.
- 다만, 레드-블랙 트리는 삽입 이후 **밸런스를 위한 색 변경, 회전 연산** 등이 추가되므로, 평균적으로 **근소한 오버헤드**가 발생한다.

### `Sorted Set일때 Insert`

<img width="700" height="450" alt="n_sweep_Insert_sorted_loglog" src="https://github.com/user-attachments/assets/2097743a-83f3-4dc4-9df1-1506c571533f" />

- 이진트리의 최악의 경우로, N이 증가할수록 성능 저하가 크게 나타난다.
- 반면, 레드-블랙 트리는 **밸런스를** 유지하므로,
입력 크기가 커져도 **로그 수준의 시간 증가**를 보인다.

### `Random Set일때 Search`

<img width="700" height="450" alt="n_sweep_Search_random_loglog" src="https://github.com/user-attachments/assets/4fdcfe3c-4513-462d-b920-9e7f9bc05e13" />

- 모든 N 구간에서 두 트리는 **거의 동일하게 나타난다.**
- **Random Set**으로 구성한 경우, 이진 탐색트리 역시 비교적 밸런스가 잘 잡힌 구조를 형성하여 평균 탐색 깊이가 레드-블랙 트리와 큰 차이를 보이지 않아 이러한 결과가 나타난다.

### `Sorted Set일때 Search`

<img width="700" height="450" alt="n_sweep_Search_sorted_loglog" src="https://github.com/user-attachments/assets/7dbf14f7-0a3b-448e-9015-9976f17d5b19" />

- 정렬된 데이터로 인해 이진 탐색트리는 한쪽으로 깊이가 치우쳐진 구조를 가지게 되어 탐색 경로가 선형에 가까워진다.
- 그 결과, 레드-블랙 트리와 비교했을때 **탐색 성능 차이가 크게 발생한다.**
<br>

## 결론
레드-블랙 트리는 **삽입 과정에서 밸런싱을** 하기 때문에 기존의 이진 탐색트리에서 발생할 수 있는 **최악의 케이스의 성능 저하를 방지하는** 자료구조이다.

즉, 평균 성능을 향상 시키기보다는 **최악의 케이스에도 성능 저점을 보장하기 위한** 자료구조로 판단했다.