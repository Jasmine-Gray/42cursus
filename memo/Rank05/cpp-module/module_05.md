
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



## ex01: Form up, maggots

### `main.cpp` の解説

このファイルは、新しく作成した `Form`（書類）クラスの生成時の挙動と、`Bureaucrat`（官僚）との相互作用（サインの可否）を検証するためのテストコードです。

* **Test 1: Valid Form Creation（正常な書類の作成）**
* 「Tax Form」という名前で、サインに必要なグレードが50、実行に必要なグレードが20の書類を作成します。
* グレードの範囲（1〜150）に収まっているため、例外は発生せず正常にインスタンス化されます。


* **Test 2 & 3: Invalid Form Creation（異常な書類の作成）**
* Test 2ではサインに必要なグレードを `0`（高すぎる）、Test 3では実行に必要なグレードを `151`（低すぎる）として書類の作成を試みます。
* コンストラクタ内で例外（`GradeTooHighException` または `GradeTooLowException`）が投げられ、即座に `catch` ブロックで捕捉されます。これにより、不正な状態の書類がシステムに紛れ込むのを防いでいます。


* **Test 4: Bureaucrat signing a form successfully（サイン成功）**
* グレード40の官僚 `Manager` と、サインにグレード50が必要な書類 `Contract` を用意します。
* 官僚のグレードは数値が小さいほど上位です。グレード40は要求水準（50）を満たしているため、`manager.signForm(contract)` は成功し、書類のステータスが「Signed（サイン済み）」に変わります。




* **Test 5: Bureaucrat failing to sign a form（サイン失敗）**
* グレード100の官僚 `Intern` が、サインにグレード10が必要な書類 `Peace Treaty` にサインを試みます。
* 権限が足りないため `beSigned()` の内部で例外が投げられ、`catch` ブロックでエラー文が表示されます。



---

### `Form.cpp` の解説

こちらが書類クラスの実態です。Subjectの指示通り、すべてのメンバ変数が正しく命名されており（`_name` などになっていない）、かつ `private` 属性として実装されています。

### 1. Orthodox Canonical Form（正統派カノニカルフォーム）

* **コンストラクタ群**
* デフォルトコンストラクタとコピーコンストラクタでは、初期化リスト（`: name(...), isSigned(...)`）を使用しています。`name`、`gradeToSign`、`gradeToExecute` は `const` 変数であるため、コンストラクタの波括弧 `{}` の中で代入することはできず、必ず初期化リストで値を設定する必要があります。


* **コピー代入演算子 (`operator=`)**
* ここがC++のクラス設計において非常に重要なポイントです。
* 定数（`const`）である名前やグレードの数値は、インスタンス化された後に別の値で上書きすることができません。そのため、代入演算子の中では定数ではない `isSigned` （サイン済みかどうかのフラグ）のみをコピーしています。


* **デストラクタ**
* 今回は動的メモリ確保（`new`）を行っていないため、波括弧の中は空で問題ありません。



### 2. パラメータ付きコンストラクタと例外スロー

* `Bureaucrat` の時と同様に、渡されたグレードが1〜150の範囲外であった場合、`throw Form::GradeTooHighException();` などの例外を投げています。


* これにより、「サインにグレード999が必要なあり得ない書類」が作られるのを根本から防ぎます。

### 3. Getters（ゲッター）

* メンバ変数はすべて `private` であるため、外部（`Bureaucrat` クラスや `main` 関数）から書類の情報を読み取るためのゲッター関数を用意しています。


* すべて最後に `const` が付いており、「この関数はオブジェクトの状態を変更しない」ことを保証しています。

### 4. `beSigned()` メンバ関数

* 書類にサインをするための心臓部のロジックです。
* 引数として `Bureaucrat` オブジェクトの参照（`const Bureaucrat&`）を受け取ります。


* 官僚のグレード（`bureaucrat.getGrade()`）と、書類が求めるグレード（`this->gradeToSign`）を比較します。
* 数値が**大きい**（＝グレードが**低い**）場合は権限不足とみなし、`Form::GradeTooLowException` を投げます。


* 条件をクリアした場合は、`this->isSigned = true;` とし、書類を有効化します。

### 5. 挿入演算子 `<<` のオーバーロード

* `std::cout << form;` としたときに、書類の全情報（名前、サインの有無、必要なグレード2種）がわかりやすく出力されるようにフォーマットしています。
* 三項演算子 `(form.getIsSigned() ? "Signed" : "Not Signed")` を使って、ブール値を人間が読みやすい文字列に変換している点がスマートです。

---

## ex02: No, you need form 28B, not 28C...
## ex03: At least this beats coffee-making
