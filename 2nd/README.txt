
./god_fib input (output_binary) (output_assembly)

argがinputだけの場合：
inputを実行します。
output_binaryもある場合：
inputを機械語にしてoutput_binaryに出します。mainがどこかを標準出力に吐きます。
output_assemblyまである場合：
一つ上に加えてさらに逆アセンブルしたものをoutput_assemblyに出します。デバッグに便利かなと思って。

メモ
・mainの先頭の#define size_memoryでメモリの大きさの調整ができます。signed32bitを超えない数字ならメモリを確保できる限り使えるはずです。命令数より十分大きい数字にしとくと安心。

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

制限
・まだ2^31命令までしか扱えません。
・ラベルには,(:#や空白文字は使わないで。
・immediateにsigned32bitで収まらない数字を入れないで。
・PCはtext領域内のアドレスを指すようにしているので、4の倍数じゃないとエラー吐きます。
・アセンブリを上から順にパースして変換していくので、ラベルを定義する行より前の行で使うとparse errorになります。
