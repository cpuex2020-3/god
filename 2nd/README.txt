
makeするとgod_fibという実行ファイルができます。使い方は次の通り。
また機能が増えてきたらオプションで変更するようにします。

./god_fib input (output_binary) (output_assembly)

argがinputだけの場合：
inputを実行します。
output_binaryもある場合：
inputを機械語にしてoutput_binaryに出します。min_caml_startがどこかを標準出力に吐きます。
output_assemblyまである場合：
output_binaryの機械語は人間には読みづらいので、もう一度アセンブリに戻してoutput_assemblyに出します。入力したアセンブリが内部でどう処理されているのか見えるので、きっとデバッグに便利です。


メモ
・mainの先頭の#define size_memoryでメモリの大きさの調整ができます。signed32bitを超えない数字ならメモリを確保できる限り使えるはずです。命令数より十分大きい数字にしとくと安心。
・parse errorが出たときは、parser.cのparseという関数においてtを取得した直後のところにprintf("%s\n",t);を入れると、どの行の処理中にerrorを吐いたかわかるので幸せになれるかも。


制限
・まだ2^29命令までしか扱えません。
・ラベルには,(:#や空白文字は使わないで。
・immediateにsigned32bitで収まらない数字を入れないで。
・PCはtext領域内のアドレスを指すようにしているので、4の倍数じゃないとエラー吐きます。
・アセンブリを上から順にパースして変換していくので、ラベルを定義する行より前の行で使うとparse errorになります。
・t1レジスタは擬似命令の解決の際に触ることがあるので注意しましょう。詳しくは次を読んでね。


擬似命令の解決
・jal label_text
	labelが指す位置と現在読んでる命令が格納される予定の位置の差distanceを取得。
	distanceがsigned21bitで収まるなら
	jal ra, distance。
	収まらないならdistanceをdistance-4に更新して、
	auipc t1, (distance>>12)
	jalr rd, t1, (distance-((distance>>12)<<12))
	に展開している。
	本来なら収まらない場合はjal label_textではなくcall label_textの仕事らしい。
・li rd, imm[31:0]
	immがsigned12bitで収まるなら
	addi rd, zero, imm。
	収まらないなら
	lui rd, imm>>12
	addi rd, rd, imm-((imm>>12)<<12)
	に展開している。
・la rd, label_data
	labelが指すアドレスaddを取得して、li rd, immと同じ処理。
・mv rd, rs1
	addi rd, rs1, 0
・ret
	jalr zero, ra, 0


各ファイルの説明

god_fibに使うやつ。
main.c：はい。メモリの大きさはここで決まってます。エラー表示が雑でごめんなさい。
instruction：命令を表す構造体instructionを定義しています。
data：レジスタとメモリを抽象化しています。pc以外のレジスタやメモリの初期値はここで決まってます。
parser：入力されたアセンブリをラベルやら擬似命令やら解決して、instructionとしてtext領域に格納するえらいやつ。一番大変なとこ。
execution：text領域からpcが指す命令を持ってきて実行します。
assembler：instructionは機械語と1対1対応するので、出力するだけの簡単なお仕事。
assembly：instructonからアセンブリに戻します。

god_fibに使わないやつ。
test.s：動作確認に使ったアセンブリ。みんなも試しに動かしてみよう！
binary_to_hex.c：標準入力から2進数の列をもらって16進数に直してくれる子です。おまけ。Makefileに書いてないので自分でgcc binary_to_hex.cしような。
