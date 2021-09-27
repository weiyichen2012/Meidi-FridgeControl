## The status of unit
- There are four status of unit:(in C++ code: `enum unitStages{unitNull, unitCar, unitContainer, unitFull};`)
  - unitNull(0): no car, empty
  - unitCar(1): only car that has no container
  - unitContainer(2): car with container, but no item in it
  - unitFull(3): car with container filled with item

## Initial Stage
- X: unitNull, C: unitCar, T: unitContainer
- Layer0(the lowest):
```
C T T
T X T
T T T
``` 
- Layer0's Id of each Unit is arranged like this corresponding to above
```
0 1 2
3 4 5
6 7 8
```
- Layer1:
```
T C T
T X T
T T T
```
- Layer1's Id of each Unit
```
9  10 11
12 13 14
15 16 17
```
- Layer2:
```
C T T
T X T
T T T
```
- Layer2's Id of each Unit
```
18 19 20
21 22 23
24 25 26
```
- Inparticular the bottom line of Layer0 and Layer1 will be the place where user input their items, when using command "store", we need to numeralize these 6 units, give them a storeId, they are sequenced as:
- Layer0:
```
C T T
T X T
T T T
^ ^ ^
| | |
1 2 3
``` 
- Layer1:
```
T C T
T X T
T T T
^ ^ ^
| | |
4 5 6
``` 
## Commands
- init: init all 27. For each unit, the format is (unitId unitStatus cargo):
  - unitId: since units will move, the place won't always hold corresponding unit, like place(id = 8) holds unit(id = 17), so on the 8th line of input, you should input (17 unitStatus cargo).
  - unitStatus: corresponding to the "Status of unit"
  - cargo: a string with no space, store the name of cargo
- print: print all 27 units.
- prepare: make sure all 6 input unit is cleared
- store N storeId1 storeId2 ... storeIdN cargo1 cargo2 ... cargoN: store items into fridge
  - N: numbers of items stored
  - storeIdi: where this i th item is stored
  - cargoi: the name of i th item
- get N cargo1 cargo2 ... cargoN: get items from fridge
  - N: numbers of items taken
  - cargoi: the name of i th item
- lift Dir cargo
  - Dir: 1 means move this cargo one level higher, -1 means move this cargo one level lower
  - cargo: the name of cargo