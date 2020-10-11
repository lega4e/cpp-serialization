# Сериализация и десериализация объектов и сложных структур в C++

### Возможности библиотеки

Эта библиотека создана для сериализации фундаментаьлных типов C++ (char, int, long long, float, double...), строк (std::string, std::wstring), динамических и статических массивов, стандартных контейнеров (vector, set, map...) пользовательских float-data структур, пользовательских сложных иерархических структур; а так же пользовательских структур с разделяемыми указателям. Библиотека также поддерживает любые сложные комбинации вышеперечисленного, указатели на объекты, массивы пользовательских объектов и т.д.

Сериализация может происходить как в обычный поток, так и в строку std::string.

Поддерживаемые стандартные контейнеры:

- `vector`
- `list`
- `set`
- `multiset`
- `unordered_set`
- `unordered_multiset`
- `map`
- `multimap`
- `unordered_map`
- `unordered_multimap`

Чтобы было возможно сериализовать эти контейнеры, типы, который они содержат, должны быть также сериализуемы.




### Подключение бибилотеки

Библиотека является header-only, поэтому, чтобы начать использовать её, достаточно поместить файлы `serialization.hpp` и `serialization_implement.hpp` в папку вашего проекта и подключить заголовочный файл `serialization.hpp` там, где вы собираетесь пользоваться библиотекой.



### Общие принципы библиотеки

Любая сериализация происходит с помощью особого шаблонного класса — `archive<Stream>`. В качестве шаблона принимается поток ввода или вывода данных (ostream, istream, ofstream, ifstream, stringstream...).  Для создания архива необходимо указать шаблонный параметр и передать в конструктор указатель на поток и режим работы (режимы будут рассмотрены далее):

```C++
archive(&cout, lis::determine_shared_mode) arch;
```

Существует две основных функции, с помощью которых сериализуются разные объекты: `serialize(arch, &obj)` и `deserialize(arch, &obj)` — которые возвращают количество записанных/прочтённых байт. Обе этих функции принимают в качестве первого аргумента ссылку на архив, а в качестве второго — указатель на объект, который необходимо сериализавоть. (Запомните простое правило: во все функции всегда передаётся *указатель* на объект, который необходимо сериализовать; если этот объект сам является указателем, то, согласно правилу, в функцию передаётся указатель на указатель; есть лишь два исключения из этого правила: первое — статические массивы, которые передаются непосредственно, и второе — данные в виде `void *`, работа с которыми осуществляется низкоуровненовй функцией `serialize_plain(archive &arch, void *value, int size)`; впрочем, вам врядли придётся пользоваться этой ей.)



#### Сериализация единичных объектов.

Чтобы сериализовать любой *единичный* сериализуемый объект, вызывается либо функция `serialize(archive, &object)`, либо используется оператор побитового сдвига, применённый к архиву: `archive << &object`.  Стандартные контейнеры, строки и любые пользовательские структуры считаются единичными объектами.



*Пример 1. Простые единичные объекты*

```C++
using namespace lis;

int a = 4;
double e = 2.7;
string s = "Lis is fox";

ofstream fout("data.bin");
archive<ofstream>(&fout) << &a << &e << &s;
fout.close();

int newa;
double newe;
string news;

ifstream fin("data.bin");
archive<ifstream>(&fin) >> &newa >> &newe >> &news;
fin.close();

printf("a: %i, e: %g, s: %s\n", newa, newe, news.c_str());

/*
 * Вывод:
 * a: 4, e: 2.7, s: Lis is fox
 */
```



*Пример 2. Стандартные контейнеры*
```C++
using namespace lis;

vector<int> vec = { 3, 5, 7, 9, 11, 13 };
vector<vector<int>> vecvec = {
	{ 1, 3, 5 },
	{ 10, 30, 50 },
	{ -1, -3, -5 }
};
map<char, string> aset = {
	{ '3', "three"s },
	{ '4', "four"s },
	{ 'L', "fox"s }
};

stringstream ss;
archive<stringstream> arch(&ss);
int writebytes = serialize(arch, &vec);
writebytes += serialize(arch, &vecvec);
writebytes += serialize(arch, &aset);
cout << "writed: " << writebytes << endl;


vector<int> newvec;
vector<vector<int>> newvecvec;
map<char, string> newset;

int readbytes = deserialize_elements(
	arch, &newvec, &newvecvec, &newset
); 
cout << "read: " << readbytes << endl << endl;

cout << "newvec:" << endl;
for(auto b = newvec.begin(), e = newvec.end(); b != e; ++b)
	cout << *b << " ";
cout << endl << endl;

cout << "newvecvec:" << endl;
for(auto b = newvecvec.begin(), e = newvecvec.end(); b != e; ++b)
{
	for(auto bb = b->begin(), ee = b->end(); bb != ee; ++bb)
		cout << *bb << " ";
	cout << endl;
}
cout << endl;

cout << "newset:" << endl;
for(auto b = newset.begin(), e = newset.end(); b != e; ++b)
	cout << b->first << " : " << b->second << endl;
cout << endl;

/*
*  Вывод:
*   writed: 111
*   read: 111
* 
*   newvec:
*   3 5 7 9 11 13 
* 
*   newvecvec:
*   1 3 5 
*   10 30 50 
*   -1 -3 -5 
* 
*   newset:
*   3 : three
*   4 : four
*   L : fox
*/
```



### Сериализация указателей

Чтобы сериализовать указатель, достаточно передать в функцию сериализации указатель на указатель. При этом, если происходит десериализация указателя, то память для объекта выделяется автоматически.

*Пример 3. Сериализация указателей*

```C++
using namespace lis;

typedef unsigned long long int ullong;

ullong *value = new ullong(329849238102);

ofstream fout("data.bin");
archive(&fout) << &value;
fout.close();


ullong *newvalue;
// ullong *newvalue = new ullong; // ОШИБКА! память потеряется

ifstream fin("data.bin");
archive(&fin) >> &newvalue;
fin.close();

cout << *newvalue << endl;

/*
 * Вывод:
 * 329849238102
 */
```



Точно так же, как сериализуются обычные указатели, можно сериализовать разделённые (shared) указатели.

*Пример 4. Сериализация разделённых указателей*

```C++
using namespace lis;

vector<shared_ptr<int>> vec = {
	make_shared<int>(4),
	make_shared<int>(10)
};

ofstream fout("data.bin");
archive(&fout) << &vec;
fout.close();


vector<shared_ptr<int>> newvec;

ifstream fin("data.bin");
archive(&fin) >> &newvec;
fin.close();

for(auto b = newvec.begin(), e = newvec.end(); b != e; ++b)
	cout << **b << " ";
cout << endl;

/*
 * Вывод:
 * 4 10 
 */
```



### Сериализация динамических массивов

Чтобы сериализовать динамечиский массив, существует дополнительная функция `serialize(arch, &arr, &size)`. Аргумент, сообщаюший о размере массива, должен иметь тип `int *`. При сериализации из него берётся информация о размере массива; при десериализации — вкладывается; память под массив выделяется автоматически с помощью оператора `new[]`. Элементами массива могут быть любые единичные сериализуемые объекты.

*Пример 5. Сериализация динамического массива*

```C++
using namespace lis;

int size = 3;
string *strarr = new string[size];
strarr[0] = "first string";
strarr[1] = "second string";
strarr[2] = "lis is fox";

ofstream fout("data.bin");
archive outarch(&fout);
serialize(outarch, &strarr, &size);
fout.close();


int newsize;
string *newstrarr;

ifstream fin("data.bin");
archive inarch(&fin);
deserialize(inarch, &newstrarr, &newsize);
fin.close();

cout << "newsize: " << newsize << endl;
for(auto *b = newstrarr, *e = newstrarr+newsize; b != e; ++b)
	cout << *b << endl;

/*
 * Вывод:
 * newsize: 3
 * first string
 * second string
 * lis is fox
 */
```



### Сериализация динамических массивов

Со статическими массивами дело обстоит несколько иначе, чем со всеми остальными объектами: во-первых, для них есть отдельная функция, называемая `serialize_static(arch, arr, size)`, во-вторых передаётся не указатель на массив, а сам массив (то есть указатель на первый элемент массива) и, в-третьих, передаётся не указатель на размер, а сам размер (преобразется в int). Элементами статического массива могут быть любые единичные сериализуемые объекты. Рассмотрим пример.

*Пример 6. Сериализация статического массива*

```C++
using namespace lis;

int arr[4] = { 4, 6, 1, -4 };

ofstream fout("data.bin");
archive outarch(&fout);
serialize_static(outarch, arr, sizeof(arr)/sizeof(int));
fout.close();


int newarr[4];

ifstream fin("data.bin");
archive inarch(&fin);
deserialize_static(inarch, newarr, sizeof(newarr)/sizeof(int));
fin.close();

for(int n : newarr)
	cout << n << endl;

/*
 * Вывод:
 * 4
 * 6
 * 1
 * -4
 */
```



### Сериализация пользовательских Flat-Data структур

Если вам необходимо сериализовать вашу собственную структуру и если она является плоской (другими словами, если все её данные находяется в ней самой, в том объёме, который выделяется оператором `new`; в ней нет указателей ни на какие объекты и структуры, которые не являются плоскими), — тогда, чтобы включить поддержку сериализации достаточно внутрь объявления структуры добавить макрос `LIS_SERIALIZABLE_PLAIN()`.

*Пример 7. Сериализация пользовательских плоских структур*

```C++
using namespace lis;

struct Point
{
	int x, y;
	char label[16];

	LIS_SERIALIZABLE_PLAIN()
};

int main( int argc, char *argv[] )
{
	Point p { 3, 5 };
	strcpy(p.label, "fox is lis\0");

	ofstream fout("data.bin");
	archive(&fout) << &p;
	fout.close();


	Point newp;

	ifstream fin("data.bin");
	archive(&fin) >> &newp;
	fin.close();

	printf("x: %i, y: %i, label: %s\n", newp.x, newp.y, newp.label);

	/*
	 * Вывод:
	 * x: 3, y: 5, label: fox is lis
	 */

	return 0;
}
```



### Сериализация пользовательских объёмный структур

Точно так же, как для плоских структур использовался макрос `LIS_SERIALIZABLE_PLAIN()`, в объёмных структурах, где есть указатели или другие объёмные объекты (например, string, vector...), используется макрос `LIS_SERIALIZABLE(...)`.  Отличие состоит только в том, что в аргументах макроса необходимо указать все объекты, которые должны сериализоваться.

*Пример 8. Сериализация объёмных структур*

```C++
using namespace lis;

struct Node
{
	char op = '\0';
	int value = 0;

	Node *left = nullptr, *right = nullptr;

	string str() const
	{
		if(op == '\0')
			return to_string(value);
		return "(" + left->str() + " " + op + " " + right->str() + ")";
	}

	LIS_SERIALIZABLE(&op, &value, &left, &right);
};

int main( int argc, char *argv[] )
{
	Node *root = new Node {
		'*', 0,
		new Node {
			'+', 0,
			new Node { '\0', 2 },
			new Node { '\0', 3 }
		},
		new Node { '\0', 3 }
	};

	ofstream fout("data.bin");
	archive(&fout) << &root;
	fout.close();


	Node *newroot;

	ifstream fin("data.bin");
	archive(&fin) >> &newroot;
	fin.close();

	cout << newroot->str() << endl;

	/*
	 * Вывод:
	 * ((2 + 3) * 3)
	 */

	return 0;
}
```

Если нам необходимо выполнить какие-либо действия после сериализации или десериалзиации, например, вычислить какое-то значение, то необходимо объявить метод `after_serialization() const` либо `after_deserialication()`. Обратите внимание на то, что метод, вызываемый после сериализации, должен быть константным.

*Пример 9. Сериализация пользователских структур с последующим выполнением функций*

```C++
using namespace lis;

struct Node
{
	char op = '\0';
	int value = 0;

	Node *left = nullptr, *right = nullptr;
	int result = 0;

	string str() const
	{
		if(op == '\0')
			return to_string(value);
		return "(" + left->str() + " " + op + " " + right->str() + ")";
	}

	// будет выполняться после сериализации
	void after_serialization() const
	{
		cout << "serialize:\t" << str() << endl;
		return;
	}

	// будет выполняться после десериализации
	void after_deserialization()
	{
		result = op == '\0' ? value : 
			op == '*' ? left->result * right->result :
			left->result + right->result;

		cout << "deserialize:\t" << str() << ", result: " << result << endl;
		return;
	}

	LIS_SERIALIZABLE(&op, &value, &left, &right);
};

int main( int argc, char *argv[] )
{
	Node *root = new Node {
		'*', 0,
		new Node {
			'+', 0,
			new Node { '\0', 2 },
			new Node { '\0', 3 }
		},
		new Node { '\0', 3 }
	};

	ofstream fout("data.bin");
	archive(&fout) << &root;
	fout.close();


	Node *newroot;

	ifstream fin("data.bin");
	archive(&fin) >> &newroot;
	fin.close();

	/*
	 * Вывод:
	 * serialize:	2
	 * serialize:	3
	 * serialize:	(2 + 3)
	 * serialize:	3
	 * serialize:	((2 + 3) * 3)
	 * deserialize:	2, result: 2
	 * deserialize:	3, result: 3
	 * deserialize:	(2 + 3), result: 5
	 * deserialize:	3, result: 3
	 * deserialize:	((2 + 3) * 3), result: 15
	 */

	return 0;
}
```

Если пользовательская структура имеет среди членов динамический или статический массив, то при сериалзиации используется специальные макросы: `LIS_SERIALIZABLE_DINAMIC_ARRAY(ptr, sizeptr)` и `LIS_SERIALIZABLE_STATIC_ARRAY(ptr, size)`. Рассмотрим на примере, чтобы было понятнее. В макрос динамического массива передаётся указатель на массив (указатель на указатель на первый элемент массива) и указатель на размер массива (`int *`); память выделяется автоматически. В макрос статического массива передаётся сам массив (указатель на первый элемент) и целое число (преобразуемое в `int`).


*Пример 10. Сериализация структуры, содержашей динамический и статический массивы*

```C++
using namespace lis;

struct S
{
	int size = 0;
	int *arr = nullptr;

	int statarr[4] = { 0 };

	LIS_SERIALIZABLE(
		LIS_SERIALIZABLE_DINAMIC_ARRAY(&arr, &size),
		LIS_SERIALIZABLE_STATIC_ARRAY(statarr, 4)
	);
};

int main( int argc, char *argv[] )
{
	S s { 4, new int[4] };
	for(int i = 0; i < 4; ++i)
		s.arr[i] = i, s.statarr[i] = (i+1)*2;

	ofstream fout("data.bin");
	lis::archive(&fout) << &s;
	fout.close();


	S news;

	ifstream fin("data.bin");
	lis::archive(&fin) >> &news;
	fin.close();

	for(auto *b = news.arr, *e = news.arr+news.size; b != e; ++b)
		cout << *b << " ";
	cout << endl;
	for(int n : news.statarr)
		cout << n << " ";
	cout << endl;

	/*
	 * Вывод:
	 * 0 1 2 3 
	 * 2 4 6 8 
	 */

	return 0;
}
```



### Сериализация в строку

Если вам необходимо сериализовать объект в строку, вы можете воспользоваться функцией `serialize(&obj)`, которая возвратит строку либо `serialize(string &src, &obj)`, которая вернёт кол-во записанных байтов и запишет объект в строку `src`. Точно также можно десериализовать объект используя функцию `deserialize(src, obj)`. Для (де)сериализации динамических и статических массивов есть соответствующие функции `serialize(&arr, &size)`, `serialize(string &src, &arr, &size)` и `serialize_static(arr, size)`, `serialize_static(src, arr, size)`. Осторожно! Если вы вызовите подряд две функции сериализации `serialize(src, &obj)`, то `src` будет содержать лишь *последний* сериализованный объект; данная функция каждый раз перезаписывает строку `src`.

*Пример 11. Сериализация объектов в строку*

```C++
using namespace lis;

int i = 43;

int n = 4;
int *arr = new int[n];
for(int i = 0; i < n; ++i)
	arr[i] = i;

int sarr[4] = { 3, 5, -3, -5 };

string isrc = serialize(&i);
string arrsrc = serialize(&arr, &n);
string sarrsrc;
cout << "sarr bytes: " << serialize_static(sarrsrc, sarr, 4) << endl;

int newi;
int newn;
int *newarr;
int newsarr[4];

deserialize(isrc, &newi);
deserialize(arrsrc, &newarr, &newn);
deserialize_static(sarrsrc, newsarr, 4);

cout << newi << endl;
for(auto *b = newarr, *e = newarr+newn; b != e; ++b)
	cout << *b << " ";
cout << endl;

for(int n : newsarr)
	cout << n << " ";
cout << endl;

/*
 * Вывод:
 * sarr bytes: 16
 * 43
 * 0 1 2 3 
 * 3 5 -3 -5 
 */
```



### Сериализация с разделяемыми указателями

Вместо сложных объяснений, в которых легко запутаться, намного лучше послужит простой пример:

```C++
using namespace lis;

shared_ptr<int> p1(new int(4));
shared_ptr<int> p2(p1);

ofstream fout("data.bin");
archive(&fout) << &p1 << &p2;
fout.close();

shared_ptr<int> newp1, newp2;
ifstream fin("data.bin");
archive(&fin) >> &newp1 >> &newp2;
fin.close();

cout << *newp1 << ", " << *newp2 << endl;
*newp1 = 10;
cout << *newp1 << ", " << *newp2 << endl;
cout << newp1.get() << ", " << newp2.get() << endl;

/*
 * Вывод:
 * 4, 4
 * 10, 10
 * 0x5612811bb180, 0x5612811bb180
 */

return 0;
```

Как видите, после десериализации указатели хранят один и тот же объект — так же, как это было перед сериализацией.  Если бы вместо `shared_ptr` были обыкновенные указатели, то десериализованные указатели указывали бы на разные объекты. Если вы хотите, чтобы и обычные указатели при десериализации восстанавливали разделённую структуру, то необходимо указать режим `determine_pointers_mode` в конструкторе `archive`:

```C++
lis::archive<ofstream> arch( &fout,	lis::determine_pointers_mode | lis::determine_shared_mode );
```

Не забудьте, что если вы не укажете также `lis::determine_shared_mode`, то `shared_ptr` не будут развёртываться в разделённую структуру.

Если в режиме развёртывания обычных указателей в разделённую структуру нет необходимости, то его стоит избегать, так как из-за него сильно разрастается объём сериализуемой информации: на каждый сериализуемый указатель потребуется дополнительные 8 байт.





