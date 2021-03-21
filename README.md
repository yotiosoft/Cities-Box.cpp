#  Cities Box.cpp
## 概要
完全無料の都市開発ゲーム「Cities Box」をC++で再現しています。

## 開発環境
- Windows版
	- OS : Windows 10 Home 64bit
	- Windows SDK : 10.0
	- C++ コンパイラ：C++ 17
	- 統合開発環境 : Visual Studio 2019 (v142)
	- ライブラリ：OpenSiv3D v0.4.3
- Mac版
	- OS : macOS 10.15.6 Catalina
	- C++ コンパイラ：C++ 17
	- 統合開発環境 : Xcode 12.3
	- ライブラリ：OpenSiv3D v0.4.3
	
## ブランチについて
ここで公開しているものはα版です。  
対処していない（今後修正予定の）バグや未知のバグが存在することをご了承ください。  
ブランチの大まかな分類は以下のとおりです。  
- master : ある程度動作を確認しているもの（動くもの）
- develop : 開発中の最新のもの（動くこともあるが動かないこともある）
- backup : 大きな仕様変更を行った際に、変更前のものを保持するためのブランチ
  
次の2つは古いブランチであり、現在は更新していません。
- mac_develop : Mac版の開発用ブランチ（developに統合）
- windows : Windows版の開発用ブランチ（developに統合）
