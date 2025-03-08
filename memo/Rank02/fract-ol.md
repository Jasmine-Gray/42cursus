https://zenn.dev/grigri_grin/articles/5d201067dedfdb

https://harm-smits.github.io/42docs/libs/minilibx/getting_started.html

https://medium.com/@leogaudin/fract-ol-creating-graphically-beautiful-fractals-6664b6b045b5

https://qst0.github.io/ft_libgfx/man_mlx.html

https://qst0.github.io/ft_libgfx/man_mlx_new_window.html

https://jun-networks.hatenablog.com/entry/2021/06/12/170030


-lは、 /usr/bin or /bin の中のライブラリファイルを参照するコマンド。
-lX11と記述すると、 /usr/binのX11ライブラリを参照するという意味になる。

-Lは、探索するライブラリディレクトリの位置を変更する。

-L$(MLX_DIR)は、MLX_DIRのpathを追加で探索することになる。 `-L$(MLX_DIR) -lmlx` は、 MLX_DIRのpathと標準ライブラリから mlxから始まるライブラリを探すことになる。