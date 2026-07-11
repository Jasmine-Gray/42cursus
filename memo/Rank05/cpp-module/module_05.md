
# C++ Module 05

### 例外処理
#### 1. 例外処理を構成する3つのキーワード

* **`throw`（投げる）:**
エラーが発生した際に、例外オブジェクトを生成して投げます。これが実行されると、現在の関数の処理は即座に中断されます。
* **`try`（試す）:**
例外が発生する「可能性がある」コードブロックを囲みます。

* **`catch`（捕まえる）:**
`try` ブロック内で `throw` された例外を受け取り、エラー復旧やエラーメッセージの表示など、適切な処理を行います。

**基本的なコード例:**

```cpp
try {
    // 例外が発生するかもしれない処理
    int grade = 151;
    if (grade > 150) {
        throw std::out_of_range("Grade is too low!"); // 例外を投げる
    }
    // throwされた場合、この下の行は実行されない
} catch (const std::out_of_range& e) {
    // out_of_range型の例外を捕まえた場合の処理
    std::cerr << "エラー: " << e.what() << std::endl;
} catch (const std::exception& e) {
    // その他の標準例外を捕まえるためのフォールバック
    std::cerr << "予期せぬエラー: " << e.what() << std::endl;
}

```

---

#### 2. 標準例外とカスタム例外

C++には `<exception>` ヘッダに定義された `std::exception` という基本クラスが存在します。

##### カスタム例外（Module 05での要件）

Module 05の `ex00` では、`std::exception` を継承して独自の例外クラス（`GradeTooHighException` など）を作成します。
親クラスである `std::exception` の `what()` メンバ関数をオーバーライド（上書き）することで、独自のエラーメッセージを定義できます。

> **Subjectの重要ルール:**
> すべてのクラスはOrthodox Canonical Form（デフォルトコンストラクタ、コピーコンストラクタ、代入演算子、デストラクタ）で設計する必要がありますが、**例外クラス（Exception classes）に関してはこのルールの適用外**となります。
>
>

また、作成したカスタム例外は、ベースとなる `std::exception` への参照（`std::exception & e`）を用いて `catch` ブロックで捕まえられるように設計する必要があります。

---

## ex00: Mommy, when I grow up, I want to be a bureaucrat!

### `main.cpp` の解説

このファイルは、作成した `Bureaucrat` クラスが正しく機能し、特に「例外処理（Exception）」が想定通りに動くかをテストするためのものです。

* **`try-catch` ブロックの使用:**
Module 05の要である例外処理のテストです。`try` の中でエラーが起こり得るコードを実行し、例外が投げられた（`throw` された）場合は直ちに処理を中断して `catch` ブロックに飛びます。これにより、プログラムがクラッシュすることなくエラーメッセージを出力できます。


* **Test 1: Normal instantiation（正常なインスタンス化）**
* グレード42の `John` を作成し、情報を出力します。
* `incrementGrade()` を呼び出します。グレードは1が最高なので、値は42から41に**減り**（昇進し）ます。


* `decrementGrade()` を呼び出し、41から42に戻る（降格する）ことを確認しています。


* **Test 2 & 3: Grade too high / low on creation（作成時のグレード異常）**
* Test 2ではグレード0、Test 3ではグレード151でインスタンス化を試みます。


* コンストラクタ内で例外が投げられるため、その直後の `std::cout` は実行されず、即座に `catch` ブロックに入りエラーメッセージを表示します。


* **Test 4 & 5: Incrementing / Decrementing out of bounds（境界値の変更異常）**
* Test 4では、すでに最高グレード（1）の官僚をさらに昇進（インクリメント）させようとします。
* Test 5では、最低グレード（150）の官僚をさらに降格（デクリメント）させようとします。
* どちらも範囲外になるため、関数内で例外が投げられ、`catch` で捕捉されます。



---

### `Bureaucrat.cpp` の解説

* **Orthodox Canonical Form（正統派カノニカルフォーム）**
* `Bureaucrat()`: デフォルトコンストラクタ。デフォルトの名前と最低グレード(150)で初期化します。
* `Bureaucrat(const Bureaucrat& other)`: コピーコンストラクタ。別のオブジェクトから名前とグレードをコピーします。
* `operator=`: コピー代入演算子。`name` は `const`（定数）であるため代入できず、`grade` のみをコピーしています。
* `~Bureaucrat()`: デストラクタ。オブジェクト破棄時の処理です。


* **パラメータ付きコンストラクタ**
* `Bureaucrat(const std::string& name, int grade)`
* 初期化リスト `: name(name)` で定数 `name` を初期化しています。
* 渡された `grade` が1より小さい（高すぎる）、または150より大きい（低すぎる）場合、即座に `throw` キーワードを使って自作の例外クラスを投げています。




* **Getters（ゲッター）**
* `getName()` と `getGrade()` は、外部からプライベート変数の値を安全に取得するための関数です。


* **Modifiers（インクリメント・デクリメント）**
* `incrementGrade()`: グレードの**数値**を減らすことで昇進を表します。事前に `grade <= 1` かどうかをチェックし、1なら例外を投げます。


* `decrementGrade()`: グレードの**数値**を増やすことで降格を表します。こちらも `grade >= 150` で例外を投げて保護しています。


* **Exception implementation（例外クラスの実装）**
* `what()` 関数をオーバーライド（上書き）しています。これにより、`catch` ブロックで `e.what()` と呼び出した際に、ここで定義したエラー文（"Bureaucrat grade is too high!" など）が返されます。


* **Insertion operator overload（挿入演算子 `<<` のオーバーロード）**
* `std::cout << john;` のように、オブジェクトを直接出力ストリームに渡した際の挙動を定義しています。
* Subjectの指定通り `<name>, bureaucrat grade <grade>.` というフォーマットで出力されるように実装されています。

---

### 1. 例外クラスの実装（Exception implementation）と `what()`

`Bureaucrat` クラスの中で、無効なグレードが指定された時に備えて `GradeTooHighException` などの例外クラスを定義しました。ここではC++の継承とポリモーフィズム（多態性）の仕組みが使われています。

#### `std::exception` クラスの継承

C++の標準ライブラリ（`<exception>`）には、すべての標準例外のベースとなる `std::exception` というクラスが用意されています。自作の例外クラスを作る際は、これを継承（`public` 継承）するのが一般的です。

```cpp
class GradeTooHighException : public std::exception {
public:
    virtual const char* what() const throw();
};

```

#### `what()` 関数のオーバーライド

`std::exception` クラスには、`what()` という**仮想関数（virtual function）** があらかじめ定義されています。この関数は、エラーの内容を説明する文字列（`const char*`型、つまりC言語スタイルの文字列）を返す役割を持っています。

自作の例外クラスでこの `what()` を上書き（オーバーライド）することで、自分専用のエラーメッセージを設定できます。

```cpp
const char* Bureaucrat::GradeTooHighException::what() const throw() {
    return "Bureaucrat grade is too high!";
}

```

#### なぜ `const throw()` がついているのか？（C++98の重要仕様）

* **`const`**: この関数を呼び出しても、オブジェクトの状態（メンバ変数など）を変更しないことを保証しています。
* **`throw()`**: これはC++98特有の「例外指定（Exception Specification）」と呼ばれる構文です。カッコの中が空であることは、「この `what()` 関数自身は、絶対に例外を投げない」ということをコンパイラに約束するものです。（※もしエラーメッセージを取得する処理自体がエラーを起こして例外を投げると、プログラムがパニックに陥ってしまうため、それを防ぐ目的があります）。

これにより、`main.cpp` などの `catch (const std::exception& e)` ブロックで、どの例外が飛んできても統一して `e.what()` を呼び出し、適切なエラー文を表示できるのです。

---

### 2. 挿入演算子 `<<` のオーバーロード（Insertion operator overload）

通常、`std::cout << 42;` や `std::cout << "Hello";` のように、標準のデータ型（intやstring）は出力できますが、自作の `Bureaucrat` オブジェクトを `std::cout << john;` と書いても、コンパイラはどう出力していいかわからずエラーになります。
これを解決するのが演算子のオーバーロードです。

#### なぜクラスの「外」で定義するのか？

ここが最大のポイントです。この関数は `Bureaucrat` クラスのメンバ関数としてではなく、クラスの外側の関数として定義します。

```cpp
std::ostream& operator<<(std::ostream& os, const Bureaucrat& bureaucrat) {
    os << bureaucrat.getName() << ", bureaucrat grade " << bureaucrat.getGrade() << ".";
    return os;
}

```

もしこれをメンバ関数として定義してしまうと、左辺にオブジェクトを置かなければならず、`john << std::cout;` という不自然な書き方になってしまいます。`std::cout << john;` と書くためには、左辺（第一引数）を `std::ostream` にする必要があるため、クラス外のグローバルな関数として定義します。

#### `friend` キーワードの禁止とGetterの活用

クラス外の関数は、通常 `Bureaucrat` のプライベート変数（`name` や `grade`）に直接アクセスできません。他人のコードを見ると、これを解決するために `friend` キーワードを使ってプライベート変数へのアクセス権を与える例がよくありますが、**42のSubjectでは `friend` の使用が明確に禁止（ペナルティ-42）されています**。

そのため、Subjectに則った正しいアプローチは、パブリックなメンバ関数であるGetter（`getName()` や `getGrade()`）を経由して値を取得し、出力ストリームに渡す方法です。

#### なぜ `std::ostream&` を返すのか？

最後に `return os;` として出力ストリームの参照を返しています。
これにより、`std::cout << john << " is here." << std::endl;` のように、`<<` を連続して繋げる（チェーンする）ことが可能になります。もし戻り値を `void` にしてしまうと、チェーンできなくなってしまいます。

---



## ex01: Form up, maggots!
## ex02: No, you need form 28B, not 28C...
## ex03: At least this beats coffee-making
