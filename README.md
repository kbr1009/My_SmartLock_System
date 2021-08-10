# LINEとawsとマイコンでスマートロックを自作してみた。
## 駆動設計
 フロントエンドはLINEBot、バックエンドはawsのサーバレスアーキテクチャ、マイコンはesp32を使用。
<div align="center">
 <img src="https://readme-img.s3.ap-northeast-1.amazonaws.com/img/SmartLock_%E8%A8%AD%E8%A8%88%E5%9B%B3.png">
</div>

## LINEによる鍵の操作画面とメッセージ
 LINEで用意されているリッチメニューに「施錠」,「解錠」のメッセージを割当ておき、ボタンを押すと送信される仕組み。
<div align="center">
 <img src="https://readme-img.s3.ap-northeast-1.amazonaws.com/img/line_%E6%93%8D%E4%BD%9C.PNG" width="300px">
</div>

## マイコンとサーボモータと鍵
 このハリボテ感がイケてる。(改修の予定あり)
<div align="center">
 <img src="https://readme-img.s3.ap-northeast-1.amazonaws.com/img/IMG_6626.jpg" width="400px">
</div>
 

