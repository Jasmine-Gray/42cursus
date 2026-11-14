defmodule WordWolf.Accounts.AvatarImage do
  @moduledoc "透過・縦横比を保持するキャラクター画像ストレージ。"

  @max_bytes 5 * 1024 * 1024
  @extensions %{"image/png" => ".png", "image/jpeg" => ".jpg", "image/webp" => ".webp"}

  @spec store(Plug.Upload.t()) :: {:ok, String.t(), String.t()} | {:error, :invalid_payload}
  def store(%Plug.Upload{path: path}) do
    with {:ok, %{size: size}} when size > 0 and size <= @max_bytes <- File.stat(path),
         {:ok, bytes} <- File.read(path),
         true <- byte_size(bytes) <= @max_bytes,
         {mime, width, height, _variant} <- ExImageInfo.info(bytes),
         extension when is_binary(extension) <- Map.get(@extensions, mime),
         true <- width in 1..4096 and height in 1..4096,
         :ok <- File.mkdir_p(directory()) do
      filename = "avatar_#{Ecto.UUID.generate()}#{extension}"
      target = Path.join(directory(), filename)

      # 切り抜き・リサイズ・背景合成をしない。透過と縦横比は元ファイルが正本。
      case File.write(target, bytes, [:binary, :exclusive]) do
        :ok -> {:ok, "/uploads/avatars/#{filename}", target}
        {:error, _reason} -> {:error, :invalid_payload}
      end
    else
      _ -> {:error, :invalid_payload}
    end
  end

  defp directory do
    Path.join([to_string(:code.priv_dir(:word_wolf)), "static", "uploads", "avatars"])
  end
end
