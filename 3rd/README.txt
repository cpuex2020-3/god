
makeするとgod_fibという実行ファイルができます。使い方は次の通り。
また機能が増えてきたらオプションで変更するようにします。

./god_fib input (-option) (output_binary,output_assembly)

argがinputだけの場合：
inputを初めてhaltを実行するまで実行します。txbuされた8bitはcharとしてprintfされます。

optionとして使えるのは「b」「a」「s」の三つです。
bを指定：output_binaryにバイナリを吐きます。min_caml_startのアドレス(=PCの初期値)を標準出力に吐きます。
aを指定：output_assemblyにinputから変換された実際に実行される命令列を吐きます。min_caml_startが何命令目を指すか(=実行が開始される位置)を標準出力に吐きます。
sを指定：inputをステップ実行します。

オプションは複数同時に指定できます。
・bとaの片方のみを指定した場合、outputファイルの指定は1つで構いません。
・bまたはaを指定した状態で、かつsを指定しなければ、実行はされません。
・オプションの処理は指定された順に行い、outputファイルも順に用います。そのためsは最後にするのがオススメです。


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
・branch rs1, rs2, label_text
	jal label_text に同じ。
・jal label_text
	labelが指す位置と現在読んでる命令が格納される予定の位置の差distanceを取得。
	distanceがsigned21bitで収まるなら
	jal ra, distance。
	収まらないならdistanceをdistance-4に更新して、
	auipc t1, (distance>>12)
	jalr ra, t1, (distance-((distance>>12)<<12))
	に展開している。
	本来なら収まらない場合はjal label_textではなくcall label_textの仕事らしい。
・j label_text
	jal label_textの、raではなくzero版。
・jalr rs1
	jalr ra, rs1, 0
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
external.s：外部関数の中身。もらったアセンブリファイルの先頭にこれを付け足して処理します。

god_fibに使わないやつ。
binary_to_hex.c：標準入力から2進数の列をもらって16進数に直してくれる子です。おまけ。Makefileに書いてないので自分でgcc binary_to_hex.cしような。
