
= FuelPHPのスパムフィルタパッケージ @<href>{https://twitter.com/web2citizen,@web2citizen}


@<href>{http://atnd.org/events/33753,FuelPHP Advent Calendar 2012} 8日目です。昨日は「FuelPHPのFieldsetクラスをこんな風に拡張してまーす、という話」(@<href>{https://twitter.com/@omoon,@omooon}さん)でした。@<br>{}


以前作成したFuelPHPで作成したWebサイト(ほぼ放置していますが…)にスパムが多々投稿されているので、FuelPHPでのパッケージ作成方法の勉強をかねて、スパム対策のためのフィルタを作成してみました。

…と作成を開始してから気がついたのですが、このパッケージには、スパム情報の管理画面も含めておいた方が便利なため、Advent Calendarでも@<href>{https://twitter.com/@mikakane_info,@mikakane_info}さんや@<href>{https://twitter.com/@sa2yasu,@sa2yasu}さんが記事にしている、モジュールとして開発した方がよかったかなあと少し後悔しています。

とはいえ、作成したものについては以下で公開しています。

 * @<href>{https://github.com/web2citizen/antispam,https://github.com/web2citizen/antispam}

まだドキュメント等が整備されていないので、以下で解説していきます。

== 今回作成するパッケージの機能


投稿時に呼び出すことで、下記の内容をチェックし、投稿の制限を行います。

 * 指定IPからの投稿禁止
 * 指定した文言を含む投稿の禁止



また、すでに投稿されているテーブルを検索し、過度な投稿を行うIPや投稿内容を自動的に制限対象に入れる機能も実装しました。ただし、投稿内容について、単語レベルで自動的にブラックリストに追加まではしておらず、投稿内容全体をみてブラックリストに追加しています。

例えば、「出会い系サイト”デアエルノ”へご案内！」という投稿が大量になされた場合でも、「デアエルノ」という単語単体が自動的にブラックリストに入るわけではありません。NGIP、NGワードの保存先DBへの接続情報は、Webアプリの接続先とはとは別途指定することができるので、複数のWebアプリで共通のNG情報を共有することができます。

== パッケージ構成


FuelPHPにおけるパッケージの基本構成(@<href>{http://fuelphp.com/docs/general/packages.html,公式ドキュメント}参照)を参考にして、下記の様に作成しました。

//quote{
/antispam　　@<strong>{パッケージルート fuel/packages に格納}@<br>{}
 　/bootstrap.php　　@<strong>{クラスファイル読込}@<br>{}
 　/readme.md　　@<strong>{マニュアル}@<br>{}
 　/classes@<br>{}
 　　/antispam.php　　@<strong>{基幹クラス}@<br>{}
 　　/antispam@<br>{}
 　　　/driver.php　　@<strong>{インスタンスクラス}@<br>{}
 　/config@<br>{}
 　　/antispam.php　　@<strong>{設定ファイル}@<br>{}
 　/sql@<br>{}
 　　/create_sample.sql　　@<strong>{スパムフィルタ用テーブル作成クエリ}@<br>{}
 　/tasks@<br>{}
 　　/spamcheck.php　　@<strong>{タスク設定例}@<br>{}
 　/test@<br>{}
 　　/antispam@<br>{}
 　　　/antispam_Test.php　　@<strong>{ユニットテスト}@<br>{}
 　　/fixture@<br>{}
 　　　/blackip_fixt.yml　　@<strong>{テスト用YAMLファイル}@<br>{}
 　　　/blackword_fixt.yml　　@<strong>{テスト用YAMLファイル}
//}

== 設置方法


パッケージルートの /antispam ディレクトリを fuel/packages ディレクトリに格納してください。/sqlディレクトリ、 /tasksディレクトリはサンプル、/testディレクトリはユニットテスト用ディレクトリなので、格納しなくてもよいです。

== 設定ファイル


下記の項目があるので、他のコンフィグファイルと同じ要領で編集し、fuel/app/configディレクトリに格納してください。

 * db_connection　DB接続情報。nullの場合、アプリケーション標準の接続情報を使用
 * black_ip_table_name　NGIPアドレスを格納するテーブル名
 * black_word_table_name　NGワードを格納するテーブル名
 * ip_column_name　NGIPを集計する際に集計するカラム名
 * post_check_term　NGIP、ワード集計時のチェック期間
 * post_limit_count　NGIP、ワード集計時のチェックカウント数


== 初期化・設定読み込み


設定ファイルの内容を読み込んで初期化するには、下記の様に記述します。

//emlist{
Antispam::forge()
//}


ファイルの内容と別の設定を行う場合は下記になります。

//emlist{
Antispam::forge(array('post_check_term' => 3600))
//}


設定していない項目に関しては設定ファイルの内容となります。

NGIP、NGワード格納用のテーブルは create_sample.sql のクエリを実行することで生成できます。

== 使用例　投稿制御


ユーザの投稿時のチェックメソッドは3つあります。全てのメソッドで、投稿がOKの場合はtrue、NGの場合はfalseを返します。

 * @<tt>{check_ip() // 投稿者のIPアドレスのチェック}
 * @<tt>{check_word($contents) // 投稿内容にNGワードが含まれているかチェック}
 * @<tt>{check_post($contents) // IPアドレス、NGワードの両方をチェック}



使用例：@<br>{}
 ユーザからの投稿をcontrollerの action(post)_ メソッドで受けた際に下記のように処理を行います。

//emlist{
$contents = Input::post('contents');
if (Antispam::forge()->check_post($contents))
    // 登録処理
else
    // NG処理
//}

== 使用例　NGIP、NGワード設定


コンフィグで指定した期間の間に指定した回数以上の投稿があったIP、投稿内容をチェックして、ブラックリストテーブルに登録します。

たとえば、コンフィグファイル標準の設定だと、1日に10回以上投稿のあったIPアドレス、投稿内容をNGとして登録します。  

 * @<tt>{set_black_ip($table_name, $active = 0) // NGIP登録}
 * @<tt>{set_black_word($table_name, $column_name, $active = 0) // NG投稿登録}



@<tt>{$active = 1} の時NGIP、NGワードとして登録されます。デフォルトでは0のため、自動でNGとまではならず、登録されたものを手動でチェックしてNGにしていくことを想定しています(そのため、モジュール化して管理ビューも作ればよかったと後悔)。


taskとして作成し、cronで実行させることで、自動的にNGIPやNG投稿内容を追加していくことができます。@<br>{}


使用例： tasks/spamcheck.php をoilコマンドによって実行するときの例です。

//cmd[commentテーブルのipカラムをチェック]{
$ php oil r spamcheck set_black_ips comment
//}

//cmd[commentテーブルのtextカラムをチェック]{
$ php oil r spamcheck set_black_ips comment text
//}

== 終わりに


本当はパッケージの作り方のような観点から記事を作りたかったのですが、ほとんど中身の解説だけになってしまいました。

とはいえ、今後パッケージを作る方の参考になれば幸いです。@<br>{}

明日は「FuelPHPで保守性の高いマルチデバイス対応を考える」(@<href>{https://twitter.com/fushiroyama,@fushiroyama}さん)です。お楽しみに！

//quote{
@<strong>{@web2citizen}


Twitter: @<href>{https://twitter.com/web2citizen,@web2citizen}


Blog: @<href>{http://www.web2citizen.info/blog/,http://www.web2citizen.info/blog/}
//}
