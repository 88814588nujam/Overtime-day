#include "MyPG.h"
#include "MyGameMain.h"

//外部ファイルより
namespace Game {//ゲームタスク
	extern int timeCnt;
}

//タイトル画面
namespace Title {
	//ゲーム情報
	int logoPosY;
	DG::Image::SP img;
	DG::Font::SP fontA;

	//--------------------------------------------------------------------------------------
	//初期化処理
	//機能概要：プログラム起動時に1回実行される（素材などの準備を行う）
	//--------------------------------------------------------------------------------------
	void Initialize() {
		img = DG::Image::Create("./data/image/b1.png");
		fontA = DG::Font::Create("HGS教科書", 10, 30);
		logoPosY = -270; //タイトル画像の初期位置を画面外に配置する
	}
	//--------------------------------------------------------------------------------------
	//解放処理
	//--------------------------------------------------------------------------------------
	void Finalize() {
		fontA.reset();
		img.reset();
	}
	//--------------------------------------------------------------------------------------
	//更新処理
	//機能概要：ゲームの１フレームに当たる処理
	//--------------------------------------------------------------------------------------
	TaskFlag UpDate() {
		auto inp = ge->in1->GetState();
		logoPosY += 9;
		if (logoPosY > 0) logoPosY = 0;
		//logoPosY = min(logoPosY + 9, 0);

		TaskFlag rtv = TaskFlag::Title;//とりあえず現在のタスクを指定
		if (logoPosY >= 0) {
			if (inp.ST.down) {
				rtv = TaskFlag::NewGame;//次のタスクをニューゲームへ
			}
			if (inp.SE.down && Game::timeCnt > 0) {
				rtv = TaskFlag::LoadGame;//次のタスクをロードゲームへ
			}
		}
		return TaskFlag::Game;
	}
	//--------------------------------------------------------------------------------------
	//描画処理
	//機能概要：ゲームの１フレームに当たる表示処理
	//--------------------------------------------------------------------------------------
	void Render() {
		ML::Box2D draw(0, 0, 480, 270);
		draw.Offset(0, logoPosY);
		ML::Box2D src(0, 0, 480, 270);
		img->Draw(draw, src);

		if(logoPosY == 0){
			ML::Box2D textBox(150, 150, 300, 200);
			string text = "Sキー：NewGame";
			if(Game::timeCnt > 0) text = "Aキー：LoadGame\nSキー：NewGame";
			fontA->Draw(textBox, text, ML::Color(0.8f, 0.5f, 0.0f, 0.2f));
		}
	}
}