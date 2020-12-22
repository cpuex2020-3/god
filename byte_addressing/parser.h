#ifndef PARSER_H_INCLUDED
#define PARSER_H_INCLUDED

signed char parse(char *file_name);
// ファイルは1行255文字以下にしてください。
// ファイルを読んで機械語に対応する構造体にしてtext領域やdata領域にポイ。
// raの初期値=0番地は終了時の呼び出し元=停止条件としてreserved。
// pcは実行開始地点であるラベルmin_caml_startの位置にする。

#endif
