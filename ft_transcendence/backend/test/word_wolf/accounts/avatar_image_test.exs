defmodule WordWolf.Accounts.AvatarImageTest do
  use ExUnit.Case, async: true

  alias WordWolf.Accounts.AvatarImage

  for {fixture, mime, dimensions} <- [
        {"portrait.png", "image/png", {32, 80}},
        {"wide.webp", "image/webp", {96, 32}},
        {"landscape.jpg", "image/jpeg", {60, 40}}
      ] do
    test "元の縦横比・透過・全バイトを保持する: #{fixture}" do
      path = Path.join("test/fixtures/avatars", unquote(fixture))
      bytes = File.read!(path)
      # クライアントが送る拡張子・Content-Typeには依存しない。
      upload = %Plug.Upload{
        path: path,
        filename: "unknown.bin",
        content_type: "application/octet-stream"
      }

      assert {:ok, url, saved} = AvatarImage.store(upload)
      on_exit(fn -> File.rm(saved) end)
      assert String.starts_with?(url, "/uploads/avatars/avatar_")
      assert File.read!(saved) == bytes
      {width, height} = unquote(Macro.escape(dimensions))
      assert {unquote(mime), ^width, ^height, _} = ExImageInfo.info(File.read!(saved))
    end
  end

  test "画像でないファイル・空ファイル・容量超過は保存しない" do
    for bytes <- ["<svg onload='alert(1)'></svg>", "", :binary.copy(<<0>>, 5 * 1024 * 1024 + 1)] do
      path = Path.join(System.tmp_dir!(), "invalid-avatar-#{Ecto.UUID.generate()}")
      File.write!(path, bytes)
      on_exit(fn -> File.rm(path) end)

      assert {:error, :invalid_payload} =
               AvatarImage.store(%Plug.Upload{
                 path: path,
                 filename: "fake.png",
                 content_type: "image/png"
               })
    end
  end

  test "4096pxを超える寸法を拒否する" do
    bytes = File.read!("test/fixtures/avatars/portrait.png")
    <<header::binary-size(16), _width::32, rest::binary>> = bytes
    path = Path.join(System.tmp_dir!(), "oversized-avatar-#{Ecto.UUID.generate()}.png")
    File.write!(path, <<header::binary, 4097::32, rest::binary>>)
    on_exit(fn -> File.rm(path) end)

    assert {:error, :invalid_payload} =
             AvatarImage.store(%Plug.Upload{
               path: path,
               filename: "oversized.png",
               content_type: "image/png"
             })
  end

  test "消えた一時ファイルもエラーとして扱う" do
    assert {:error, :invalid_payload} =
             AvatarImage.store(%Plug.Upload{
               path: "/nonexistent/avatar.png",
               filename: "avatar.png",
               content_type: "image/png"
             })
  end
end
