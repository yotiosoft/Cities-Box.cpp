Cities Box
=====================
![Version](https://img.shields.io/badge/Version-1.5.2-brightgreen.svg)
![ReleaseType](https://img.shields.io/badge/Release%20Type-%E6%AD%A3%E5%BC%8F%E7%89%88-orange.svg)
# 概要
Cities Box 正式版の公開用リポジトリです。

# 利用方法
Cities_Box.exeを実行するとご利用いただけます。  
または、Cities Box.hspをコンパイルしてお使いください。

#  操作方法
## キー操作
十字キー：マップ上のカメラ移動  
左クリック：道路の敷設・建物の建設
　　
## メニューのボタンの説明（左上から）　
道路ボタン : 道路の敷設  
線路ボタン : 線路の敷設・駅の建設  
住居地区ボタン : 住居区画の地区設定・住居の建設  
商業地区ボタン : 商業区画の地区設定・商業施設の建設  
オフィス地区ボタン : オフィス区画の地区設定・オフィスの建設  
工業地区ボタン : 工業区画の地区設定・工場の建設  
農業地区ボタン : 農業区画の地区設定・農地の建設  
公共施設ボタン : 公共施設（警察署、消防署、学校など）の建設  
公園ボタン : 公園の建設  

予算ボタン : 予算・税率の変更  
統計ボタン : 犯罪率や地価などの表示  
設定ボタン : 都市名や市長名の変更、BGMのON/OFFなど  
撤去ボタン : 建物や道路、線路などの撤去  
保存ボタン : ゲームの保存  

# ソースコードについて
ソースコードは是非改造にご利用ください。  
その際、ご一報いただけると幸いです。  
また、ソースコード上で何かアドバイスなどがありましたら  
コンタクトフォームなどで教えてくださるとありがたいです。  

# ゲーム上のルール
## 通貨について
通貨単位はシムオリオン（記号§）です。  
すべての建物・道路の建設には§5がかかります。  
警察・消防・郵便・教育への支出は1つの建物に付き§1000かかります。  
住民からの税収は1人あたり§10になります。  
商業・オフィス・工業地区からの税収は労働者1人あたり§10となります。  

## RCOIFメーターについて
RCOIFメーターは、都市内の住居、商業、オフィス、工業、農業の各需要度を示します。  
RCOIFメーターによって、いま都市が何を必要としているかがわかります。  

各メーターは示す内容は以下のとおりです。  
・R - 住居地区 (Residential) の需要度  
・C - 商業地区 (Commercial) の需要度  
・O - オフィス地区 (Office) の需要度  
・I - 工業地区 (Industrial) の需要度  
・F - 農業地区 (Farm) の需要度  

## 人口の増減について
人口の増減は、毎日0時0分に行われます。  
人口の増え具合は需要度（RCOIFメーターのR）によって異なります。  
このとき、住民の就職・就学も決まります。

# Addon Makerについて
ゲーム内で使用される建物や公園などはアドオンと呼ばれ、  
同梱のAddon Makerで作成できます。  
Addon Makerについて詳細は、Addon_Makerフォルダ内のReadMe.txtをご覧ください。

# アップデータについて
インターネットで接続された状態で、かつご利用のバージョンより新しいバージョンが公開されている場合、  
更新情報をメニュー画面にてお知らせいたします。  
「更新する」ボタンを押すと、アップデータを通して自動でアップデートされます。  
この更新通知は表示させない設定にすることもできます。

# 開発版について
正式版とは別に、最新の機能を試すことができる「開発版」があります。  
開発版では、正式版のような動作保証はできませんが、  
開発版を受け取りたい方は、設定から「開発版のアップデートを受け取る」に  
チェックを入れてください。

# エラー番号について
動作上で問題が発生したとき、エラーメッセージとともにエラー番号が表示されます。  
エラー番号の頭文字の意味は以下のとおりです。  
・A:アドオン関連のエラー（例：アドオン名が取得できない　など）  
・F:ファイル関連のエラー（例：About.txtが存在しない　など）  
・M:マップデータのエラー（例：name(x,y)の取得ができない　など）  
・S:システム上のエラー（例：マップサイズが0以下になっている　など）  
・U:原因不明のエラー

# バグや問題点、今後の予定について
## バグ・問題点
・水路の向きが正常に設定されないことがある（一部修正済み）

## 今後の予定
・隣町との接続ややり取りも可能にする  
・空港や港の建設も可能に  
・他の都市との交易

# 最終更新
## Ver.1.5.2 r128  
特定の条件下で学校を設置するとエラーが発生する問題を修正  
一部のアドオンの画像を修正  
    
※最新版以外の詳細は同梱のReadMe.txtを御覧ください。