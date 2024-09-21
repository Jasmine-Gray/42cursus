### コマンドを作成

`pa pb sa sb ss ra rb rr rra rrb rrr pa pb`

### 双方向循環リスト作成
- libft 連結リストの修正
- next の node に加えて prev
- 構造体のメンバにt_node型のprevを追加
```
1 2 3 4 5 6のリストのとき
1のnextが2、prevは6にならなければならない
6のnextが1、prevは5にならなければならない
```
### 