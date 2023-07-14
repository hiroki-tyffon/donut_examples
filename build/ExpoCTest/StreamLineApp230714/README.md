全般
C++言語標準: ISO C++17標準に変更

C/C++
追加のインクルードディレクトリ
C:\Tyffon\Repositories\expoc-test\donut_examples\donut\include;C:\Tyffon\Repositories\expoc-test\donut_examples\donut\thirdparty\jsoncpp\src\lib_json\..\..\include;C:\Tyffon\Repositories\expoc-test\donut_examples\build\donut\thirdparty\jsoncpp\include\json;C:\Tyffon\Repositories\expoc-test\donut_examples\donut\thirdparty\lz4\lib;C:\Tyffon\Repositories\expoc-test\donut_examples\donut\thirdparty\miniz;C:\Tyffon\Repositories\expoc-test\donut_examples\build\donut\thirdparty\miniz;C:\Tyffon\Repositories\expoc-test\donut_examples\donut\nvrhi\include;C:\Tyffon\Repositories\expoc-test\donut_examples\donut\thirdparty\stb;C:\Tyffon\Repositories\expoc-test\donut_examples\donut\thirdparty\tinyexr;C:\Tyffon\Repositories\expoc-test\donut_examples\donut\thirdparty\cgltf;C:\Tyffon\Repositories\expoc-test\donut_examples\donut\ShaderMake\include;C:\Tyffon\Repositories\expoc-test\donut_examples\donut\thirdparty\taskflow;C:\Tyffon\Repositories\expoc-test\donut_examples\donut\thirdparty\glfw\include;C:\Tyffon\Repositories\expoc-test\donut_examples\donut\thirdparty\imgui;C:\Tyffon\Repositories\expoc-test\donut_examples\donut\nvrhi\thirdparty\Vulkan-Headers\include;.\streamline\include;%(AdditionalIncludeDirectories)
プリプロセッサの定義
%(PreprocessorDefinitions);WIN32;_WINDOWS;_ITERATOR_DEBUG_LEVEL=1;USE_DX11=1;USE_DX12=1;USE_VK=1;NOMINMAX;_CRT_SECURE_NO_WARNINGS;DONUT_WITH_LZ4;DONUT_WITH_MINIZ;MINIZ_STATIC_DEFINE;DONUT_WITH_TASKFLOW;DONUT_WITH_TINYEXR;CMAKE_INTDIR="Debug";USE_DX12

リンカー
追加の依存ファイル
..\..\donut\Debug\donut_render.lib;..\..\donut\Debug\donut_app.lib;..\..\donut\Debug\donut_engine.lib;..\..\donut\ShaderMake\Debug\ShaderMakeBlob.lib;..\..\donut\Debug\donut_core.lib;..\..\lib\Debug\jsoncpp.lib;..\..\donut\thirdparty\Debug\lz4.lib;..\..\donut\thirdparty\miniz\Debug\miniz.lib;..\..\donut\thirdparty\glfw\src\Debug\glfw3.lib;..\..\donut\thirdparty\Debug\imgui.lib;..\..\donut\nvrhi\Debug\nvrhi_d3d11.lib;dxgi.lib;..\..\donut\nvrhi\Debug\nvrhi_d3d12.lib;.\streamline\lib\x64\sl.interposer.lib;d3d12.lib;dxguid.lib;d3d11.lib;..\..\donut\nvrhi\Debug\nvrhi_vk.lib;..\..\donut\nvrhi\Debug\nvrhi.lib;kernel32.lib;user32.lib;gdi32.lib;winspool.lib;shell32.lib;ole32.lib;oleaut32.lib;uuid.lib;comdlg32.lib;advapi32.lib
サブシステム: コンソールをWindowsに変更


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