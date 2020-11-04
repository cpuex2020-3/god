
makeするとgod_fibという実行ファイルができます。使い方は次の通り。
また機能が増えてきたらオプションで変更するようにします。

./god_fib input (output_binary) (output_assembly)

argがinputだけの場合：
inputをstep実行します。
output_binaryもある場合：
inputを機械語にしてoutput_binaryに出します。min_caml_startのアドレス(=PCの初期値)を標準出力に吐きます。
output_assemblyまである場合：
output_binaryの機械語は人間には読みづらいので、アセンブリに戻してoutput_assemblyに出します。入力したアセンブリが内部でどう処理されているのか見えるので、きっとデバッグに便利です。アセンブリしかいらないのなら、同じ名前のファイルを指定してください。


メモ
・mainの先頭の#define size_memoryでメモリの大きさの調整ができます。特に機械語を吐かせる前には確認しよう。signed32bitを超えない数字ならメモリを確保できる限り使えるはずです。命令数より十分大きい数字にしとくと安心。
・parse errorが出たときは、parser.cのparseという関数においてtを取得した直後のところにprintf("%s\n",t);を入れると、どの行の処理中にerrorを吐いたかわかるので幸せになれるかも。
・関数呼出規約からのリマインドですが、TemporaryはArgumentに使わないでね。詳しく知りたい場合は擬似命令の解決の項を眺めましょう。


制限
・ラベルには,(:#や空白文字は使わないで。
・text領域のimmediateはsigned32bitで収まる10進数しか対応してないです。
・data領域のimmediateは先頭に0xがついた8桁の16進数をsigned32bitとして処理します。
・PCはtext領域内のアドレスを指すようにしているので、4の倍数じゃないとエラー吐きます。
・.dataでのラベルに関しては、定義する行より前の行で使うとparse errorになります。


擬似命令の解決
・jal min_caml_print_int
	先頭に0xがついた8桁の16進数が表示されるように10個のasciiを吐きます。signed32bitだと思うとa0の値に一致します。具体的には次のように展開されています。
	li   t1, 48
	txbu t1
	li   t1, 120
	txbu t1
	mv   t1, a0
	li   t2, 28
	li   t3, 10
	srl  a0, a0, t2
	andi a0, a0, 15
	blt  a0, t3, 8
	addi a0, a0, 7
	addi a0, a0, 48
	txbu a0
	beq  t2, zero, 16
	addi t2, t2, -4
	mv   a0, t1
	jal  zero, -36
・jal label_text
	labelが指す位置と現在読んでる命令が格納される予定の位置の差distanceを取得。
	distanceがsigned21bitで収まるなら
	jal ra, distance。
	収まらないならdistanceをdistance-4に更新して、
	auipc t1, (distance>>12)
	jalr rd, t1, (distance-((distance>>12)<<12))
	に展開している。
	本来なら収まらない場合はjal label_textではなくcall label_textの仕事らしい。
・jalr rs1
	jalr ra, rs1, 0
・branch rs1, rs2, label_text
	jal label_text に同じ。
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
・nop
	addi zero, zero, 0


各ファイルの説明

god_fibに使うやつ。
main.c：はい。メモリの大きさはここで決まってます。エラー表示が雑でごめんなさい。
instruction：命令を表す構造体instructionを定義しています。
data：レジスタとメモリを抽象化しています。pc以外のレジスタやメモリの初期値はここで決まってます。
label：.textのラベルに関して、定義される行より前の行で使えるようにするやつ。
parser：入力されたアセンブリをラベルやら擬似命令やら解決して、instructionとしてtext領域に格納するえらいやつ。一番大変なとこ。
execution：text領域からpcが指す命令を持ってきて実行します。
assembler：instructionは機械語と1対1対応するので、出力するだけの簡単なお仕事。
assembly：instructonからアセンブリに戻します。

god_fibに使わないやつ。
test.s：動作確認に使ったアセンブリ。みんなも試しに動かしてみよう！
print_int.s：jal min_caml_print_intはこんな感じで処理されています。ちゃんとprint_intしてるか確認してみよう！
binary_to_hex.c：標準入力から2進数の列をもらって16進数に直してくれる子です。おまけ。Makefileに書いてないので自分でgcc binary_to_hex.cしような。
