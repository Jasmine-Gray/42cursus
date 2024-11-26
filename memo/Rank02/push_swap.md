# push_swap
### 双方向循環リスト作成
- libft 連結リストの修正
- next の node に加えて prev
- 構造体のメンバにt_node型のprevを追加
```
"1 2 3 4 5 6"のとき
1のnextが2、prevは6にならなければならない
6のnextが1、prevは5にならなければならない
```
### 構造体
```c
typedef struct s_node
{
	struct s_node    *next;
	struct s_node    *prev;
	int				 value;
	int				 index;

}           		 t_node;


typedef struct s_linked_list
{
	t_node          *top;

} 	                t_linked_list
```
### 違い
partition.value
partition->valueの違い

.は構造体が実体で、member変数にアクセスしたい場合
->は構造体がアドレスで、member変数にアクセスしたい場合

### 双方好循環リストのライブラリ作成
入ってきた分のnodeを作る
- 引数で受け取った値をnodeにする... 
	```c
	t_node *new_node(int value);
	```
- stackの一番下にあるnodeを探す関数... 
	```c
	t_node *find_last_node(t_linked_list *stack);
	```
- listの最下層に新しいnodeを連結させる関数... 
	```c
	t_linked_list *double_list_add_back(t_node *new_node);
	```
- listのtopに新しいnodeを連結させる関数... 
	```c
	t_linked_list *double_list_add_front(t_node *new_node);
	```
- listのtopのnodeをとりだす関数... 
	```c
	void *detach_node(t_node *new_node);
	```
- stack_aにあるnodeの数を数える関数...
	```c
	int count_node(t_linked_list *stack)
	```
if stackがnullの場合. stackがtopだけの場合. topにprevの値が入っている場合
### コマンドを作成

`pa pb sa sb ss ra rb rr rra rrb rrr pa pb`

### ソートの実装
##### quick sort

- ソート済みか否かを確認する。ソート済みでない場合のみwhile文に入る。
- left, right, pivotの場所を設定する。
- indexをふる。
- leftとrightが正しい位置関係の間ソートを続ける。
	```c
	if (left < pivot)
        left = left->next;
    if (right > pivot)
        right = right->prev;
    if (left > pivot && right < pivot)
        swap_left_right(stack_a, stack_b, &left, &right);
	```
- ソートが終わったら、left->prevにpivotをinsertする。
- 作成する関数
	```c
	is_sort(stack_a);
	position_pivot(stack_a, pivot);
	swap_left_right(stack_a, stack_b, left, right);
	insert_pivot(stack_a, stack_b, left, right);
	```

