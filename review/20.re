
= FuelPHPのormを複数DBに対応させる方法


@<href>{http://atnd.org/events/33753,FuelPHP Advent Calendar 2012} 20日目の担当の@<href>{http://twitter.com/6q5,@6q5}です。


昨日は、@<href>{http://twitter.com/ttikitt,@ttikitt}の@<href>{http://www.cry-kit.com/?p=53,FuelPHPへのDoctrine2組み込み}でした。


今年は仕事で３つもFuelPHPを使った媒体に関わり、@<br>{}
 FuelPHPと共に過ごした１年でした。@<br>{}
 その経験で得た物の中から選りすぐりの１つ？を紹介します。

== ormでDBクラスのexecuteのようにDB接続先を指定することができません。


DBがMaster/Slave構成の時に、ormを使って参照だけの時はSlaveに向けたいなどしたい時に不便です。

==== 例 config/db.php こんな感じでconfigに2つのDB設定がされてるとします。

//emlist{
/**
 * The development database settings.
 */
return array(
    'default' => array(
        'connection' => array(
            'dsn' => 'mysql:host=xxx.xxx.xxx.xxx;dbname=BLOG_DEV',
            'username' => 'blog',
            'password' => 'xxxxxxxx',
        ),  
    ),
    'slave' => array(
        'connection' => array(
            'dsn' => 'mysql:host=xxx.xxx.xxx.xxx;dbname=BLOG_DEV',
            'username' => 'blog',
            'password' => 'xxxxxxxx',
        ),
    ),  
); 
//}

==== 例 DBクラス

//emlist{
$query = DB::select('user_id','exp')
    ->from('users')
    ->where('user_id', $user_id)
    ->execute('slave') // 引数で指定できる
    ->as_array();
//}

==== 例 ORM

//emlist{
$query = Model_User::find()
    ->where('user_id', $user_id)
    ->get(); // でっできないっっ
//}

== それを可能にするには以下のようにします。　

==== packages/orm/classes/query.phpを拡張するためapp以下にコピー

//emlist{
mkdir -p app/classes/core/orm/
cp ../packages/orm/classes/query.php app/classes/core/orm/
//}

==== app/classes/core/orm/query.phpに以下のメソッドを追加

//emlist{
+
+       /**
+        * Change Connection
+        *
+        * @param   String connection
+        * @return  Query
+        */
+       public function connection($connection='default')
+       {
+               $this->connection = $connection;
+               return $this;
+       }
//}

==== packages/orm/classes/bootstrap.phpを以下のように書き換え

//emlist{
-       'Orm\\Query'        => __DIR__.'/classes/query.php'
+       'Orm\\Query'        => APPPATH.'/classes/core/orm/query.php',
//}

==== 結果

//emlist{
$result = Model_User::find()
    ->where('user_id', $user_id)
    ->where('status', Model_User::STATUS_ACTIVE)
    ->connection('slave') // できたヽ(^o^)丿
    ->get_one();
//}


これでormを使って複数DBの切り替えが出来るようになりました。

== おまけ tasksをproductionモードでcronに設定する


fuel/app/tasks/batch.php をProductionモードでcronに設定する場合、

//emlist{
5 * * * * env FUEL_ENV=production /usr/local/bin/php oil r batch
//}


開発環境と本番環境を区別させて実行するための必須設定ですね


以上！


あすは、@<href>{http://twitter.com/konkon1234,@konkon1234}さんの『(仮)FuelPHPで１サイトを作ってみて気が付いた点など』です。@<br>{}
 お楽しみに！
