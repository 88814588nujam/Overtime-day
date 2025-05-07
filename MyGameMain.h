#pragma once
#include "MyPG.h"

extern  void  MyGameMain_Finalize( );
extern  void  MyGameMain_Initialize( );
extern  void  MyGameMain_UpDate( );
extern  void  MyGameMain_Render2D( );

enum class TaskFlag
{
	Non,		//タスクが未選択状態
	Title,		//タイトル
	Game,		//ゲーム本編
	Ending,     //エンディン
	NewGame,    //ニューゲーム
	LoadGame,   //ロードゲーム

	//以下必要に応じて追加
};
