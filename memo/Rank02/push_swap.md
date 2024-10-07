### 双方向循環リスト作成
- libft 連結リストの修正
- next の node に加えて prev
- 構造体のメンバにt_node型のprevを追加
```
1 2 3 4 5 6のリストのとき
1のnextが2、prevは6にならなければならない
6のnextが1、prevは5にならなければならない
```
- 構造体
		next
		prev

### コマンドを作成

`pa pb sa sb ss ra rb rr rra rrb rrr pa pb`

### 双方好循環リストのライブラリ作成
入ってきた分のnodeを作る
- 引数で受け取った値をnodeにする... 
t_node *ft_double_list_new(int value);
- listの最下層に新しいnodeを連結させる関数... 
t_stack *ft_double_list_add_back(t_node *new_node);
if stackがnullの場合. stackがtopだけの場合. topにprevの値が入っている場合

### ソートの実装

- quick sort
- merge sort
- radix sort
