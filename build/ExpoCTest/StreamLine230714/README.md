全般
C++言語標準: ISO C++17標準に変更

C/C++
追加のインクルードディレクトリ
プリプロセッサの定義

リンカー
追加の依存ファイル
サブシステム: コンソールをWindowsに変更

以上をbasic_triangleからコピー

donut_examples\build\x64\Debug\shaders
にプリコンパイルされたシェーダーをコピーする必要がある


## Streamline
HelloStreamline\streamline
にinclude, lib, binをコピー

streamline/includeをインクルードディレクトリに追加

streamline/lib/x64/sl.interposer.libを
リンカー/追加の依存ファイル: に追加

コンパイル時にエラーのでるところを修正
cmdbuffer = nullptr;

実行exeフォルダに
dllをコピー