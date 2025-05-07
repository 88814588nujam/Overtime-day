#include "MyPG.h"
#include "MyGameMain.h"

namespace Game {
	//--------------------------------------------------------------------------------------
	//未解決の問題：
	//①敵の攻撃頻度
	//②敵はプレイヤーを攻撃できる範囲内のランダム位置に移動し、ゲームの限界を越えてしまう
	//--------------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------------
	//変数宣言
	//--------------------------------------------------------------------------------------
	//グローバルタイマー
	int timeCnt;
	//ウィンドウサイズ
	const int windowW = 640;
	const int windowH = 360;
	//オブジェクト変数
	XI::GamePad::SP gamePadIn;
	DG::Font::SP fontTest;
	DG::Image::SP bg_black, bg_wall, bg_grass, fb_Eletric;
	DG::Image::SP healthImg0, healthImg1;
	DG::Image::SP enemyImg0;
	DG::Image::SP enemyImg1;
	DG::Image::SP playerImg0;
	DG::Image::SP playerImg1;
	DG::Image::SP shotImg0, shotImg1;
	DM::Sound::SP playerHit0, playerHit1;
	DM::Sound::SP playerMage;
	//キャラクターの属性
	enum class CharaState { Stand, Walk, Dead, Non};
	enum class AttackState { Normal, Non };
	enum class CharaFaceDirect { Left, Right };
	enum class CharaMoveDirect { Left, UpLeft, Up, UpRight, Right, DownRight, Down, DownLeft };
	//構造体の定義
	struct Object {
		CharaState charaState;
		AttackState attackState;
		CharaFaceDirect charaFaceDirect;
		CharaMoveDirect charaMoveDirect;
		float x, y;
		int w, h;
		int rank;
		int typeNum;
		int charaIdx;
		int attackTypeNum;
		//未解決：①敵の攻撃頻度
		int attackFrequency;
		int standCnt, walkCnt, deadCnt, dissipateCnt, commonCnt;
		float speed;
		float angle;
		float healthCnt;
		float healthNow;
		float injuryRadius;
		float damageMin, damageMax;
		float attackMinRadius, attackMaxRadius;
		float lstTarX, lstTarY;
		bool displayHealth;
	};
	//敵 (0:近距離攻撃剣士 1:遠距離攻撃魔法士)
	const int soldierNum = 10;
	const int wizardNum = 15;
	int deadSoldierNum;
	int deadWizardNum;
	Object enemyObjs[soldierNum + wizardNum];
	Object enemyHealthObjs[soldierNum + wizardNum];
	Object wizardShotObjs[wizardNum][1];//未解決：①敵の攻撃頻度、とりあえず一発だけ発射する
	//プレイヤータイプ (0:近距離攻撃スケルトン 1:遠距離攻撃スライム)
	int playerType;
	Object playerObjs[2];
	Object playerHealthObjs[2];
	Object slimeShotObjs[10];
	//カメラ
	Object camera;
	//競技場のサイズを調整する
	const int blockRow = 1; //水平にblockRow個のウィンドウの幅
	const int blockCol = 2; //垂直にblockCol個のウィンドウの高さ
	const int sizeW = (windowW * blockRow) - windowW;
	const int sizeH = (windowH * blockCol) - windowH;
	const float cameraX = 0.0f - sizeW / 2.0f;
	const float cameraY = 0.0f - sizeH / 2.0f;
	//--------------------------------------------------------------------------------------
	//関数のプロトタイプ宣言
	//主な対象関数
	//--------------------------------------------------------------------------------------
	void EnemyObj_Initialize(Object& obj_, int idx_, int rank_, int type_, float x_, float y_);
	void EnemyObj_UpDate(Object& obj_);
	void EnemyObj_Draw(Object& obj_);
	void EnemyObj0_Initialize(Object& obj_, int idx_, int rank_, float x_, float y_);
	void EnemyObj0_UpDate(Object& obj_);
	void EnemyObj0_Draw(Object& obj_);
	void EnemyObj1_Initialize(Object& obj_, int idx_, int rank_, float x_, float y_);
	void EnemyObj1_UpDate(Object& obj_);
	void EnemyObj1_Draw(Object& obj_);
	void PlayerObj_Initialize(Object& obj_, int idx_, int type_, float x_, float y_);
	void PlayerObj_UpDate(Object& obj_);
	void PlayerObj_Draw(Object& obj_);
	void PlayerObj0_Initialize(Object& obj_, int idx_, float x_, float y_);
	void PlayerObj0_UpDate(Object& obj_);
	void PlayerObj0_Draw(Object& obj_);
	void PlayerObj1_Initialize(Object& obj_, int idx_, float x_, float y_);
	void PlayerObj1_UpDate(Object& obj_);
	void PlayerObj1_Draw(Object& obj_);
	void ShotObj_Initialize(Object& obj_, int tn_, Object& cObj_);
	void ShotObj_UpDate(Object& obj_);
	void ShotObj_Draw(Object& obj_);
	void ShotObj0_Initialize(Object& obj_, Object& cObj_);
	void ShotObj0_UpDate(Object& obj_);
	void ShotObj0_Draw(Object& obj_);
	void ShotObj1_Initialize(Object& obj_, Object& cObj_);
	void ShotObj1_UpDate(Object& obj_);
	void ShotObj1_Draw(Object& obj_);
	void HealthObj_Initialize(Object& obj_, int type_, int cObjIdx_, bool dis_);
	void HealthObj_UpDate(Object& obj_);
	void HealthObj_Draw(Object& obj_);
	void Camera_Initialize(Object& obj_, int br_, int bc_, float x_, float y_);
	void Camera_UpDate(Object& obj_);
	void Camera_Draw(Object& obj_);
	//--------------------------------------------------------------------------------------
	//他の関数
	//--------------------------------------------------------------------------------------
	void EnemyHealthObj_Appear(Object& obj_, int cObjIdx_);
	void PlayerHealthObj_Appear(Object& obj_, int cObjIdx_);
	void SlimeShotObj_Appear(Object& cObj_);
	void WizardShotObj_Appear(Object& cObj_);
	void EnemyWalkToTar_Determine(Object& obj_, Object& tarObj_);
	float* EnemyTarCrd_GetResult(Object& obj_, Object& tarObj_);
	void CharaMove_Calculate_Absolute(Object& obj_, float xSpeed_, float ySpeed_);
	void CharaMove_Calculate_Relative(Object& obj_, Object& cObj_, float xSpeed_, float ySpeed_);
	void EnemyDamage_Calculate(Object& obj_, bool isAbs_);
	void PlayerDamage_Calculate(Object& obj_, bool isAbs_);
	//--------------------------------------------------------------------------------------
	//カメラの初期化
	//--------------------------------------------------------------------------------------
	void Camera_Initialize(Object& obj_, int br_, int bc_, float x_, float y_) {
		obj_.x = x_;
		obj_.y = y_;
		obj_.w = br_ * windowW;
		obj_.h = bc_ * windowH;
	}
	//--------------------------------------------------------------------------------------
	//カメラの更新
	//--------------------------------------------------------------------------------------
	void Camera_UpDate(Object& obj_) {
	}
	//--------------------------------------------------------------------------------------
	//カメラの描画
	//--------------------------------------------------------------------------------------
	void Camera_Draw(Object& obj_) {
		//レイヤー
		int backBlockW = 64;
		int backBlockH = 64;
		for (int i = 0; i < ceil(obj_.w / backBlockW) + 1; ++i) {
			for (int j = 0; j < ceil(obj_.h / backBlockH) + 1; ++j) {
				int printW = i == ceil(obj_.w / backBlockW) ? obj_.w % backBlockW : backBlockW;
				int printH = j == ceil(obj_.h / backBlockH) ? obj_.h % backBlockH : backBlockH;
				ML::Box2D draw(i * backBlockW, j * backBlockH, printW, printH);
				draw.Offset(obj_.x, obj_.y);
				ML::Box2D src(0, i % 2 == 0 ? 128 : 64, printW, printH);
				bg_grass->Draw(draw, src);
			}
		}
		int printW = (int) ceil(obj_.w / backBlockW) * backBlockW + obj_.w % backBlockW;
		int printH = (int) ceil(obj_.h / backBlockH) * backBlockH + obj_.h % backBlockH;
		//左右の壁
		int wallWX = 8;
		int wallHX = 32;
		for (int j = 0; j < ceil(obj_.h / wallHX) + 1; ++j) {
			int printH = j == ceil(obj_.h / wallHX) ? obj_.h % wallHX : wallHX;
			ML::Box2D drawLeft(0 - wallWX, j * wallHX, wallWX, printH);
			drawLeft.Offset(obj_.x, obj_.y);
			ML::Box2D srcLeft(32, 64, wallWX, printH);
			bg_wall->Draw(drawLeft, srcLeft);
			ML::Box2D drawRight(printW, j * wallHX, wallWX, printH);
			drawRight.Offset(obj_.x, obj_.y);
			ML::Box2D srcRight(32, 64, wallWX, printH);
			bg_wall->Draw(drawRight, srcRight);
		}
		//上下の壁
		int wallWY = 32;
		int wallHY = 8;
		for (int i = 0; i < ceil(obj_.w / wallWY) + 1; ++i) {
			int printW = i == ceil(obj_.w / wallWY) ? obj_.w % wallWY : wallWY;
			ML::Box2D drawUp(i * wallWY, 0 - wallHY, printW, wallHY);
			drawUp.Offset(obj_.x, obj_.y);
			ML::Box2D srcUp(40, 32, printW, wallHY);
			bg_wall->Draw(drawUp, srcUp);
			ML::Box2D drawDown(i * wallWY, printH, printW, wallHY);
			drawDown.Offset(obj_.x, obj_.y);
			ML::Box2D srcDown(40, 32, printW, wallHY);
			bg_wall->Draw(drawDown, srcDown);
		}
		//四つ角
		int corner = 8;
		ML::Box2D draw0(0 - corner, 0 - corner, corner, corner);
		draw0.Offset(obj_.x, obj_.y);
		ML::Box2D src0(32, 32, corner, corner);
		bg_wall->Draw(draw0, src0);
		ML::Box2D draw1(printW, 0 - corner, corner, corner);
		draw1.Offset(obj_.x, obj_.y);
		ML::Box2D src1(120, 32, corner, corner);
		bg_wall->Draw(draw1, src1);
		ML::Box2D draw2(0 - corner, printH, corner, corner);
		draw2.Offset(obj_.x, obj_.y);
		ML::Box2D src2(152, 128, corner, corner);
		bg_wall->Draw(draw2, src2);
		ML::Box2D draw3(printW, printH, corner, corner);
		draw3.Offset(obj_.x, obj_.y);
		ML::Box2D src3(256, 128, corner, corner);
		bg_wall->Draw(draw3, src3);
	}
	//--------------------------------------------------------------------------------------
	//敵の初期化
	//--------------------------------------------------------------------------------------
	void EnemyObj_Initialize(Object& obj_, int idx_, int rank_, int type_, float x_, float y_) {
		switch (type_) {
			case 0:
				EnemyObj0_Initialize(obj_, idx_, rank_, x_, y_);
				break;
			case 1:
				EnemyObj1_Initialize(obj_, idx_, rank_, x_, y_);
				break;
		}
		EnemyHealthObj_Appear(enemyHealthObjs[idx_], idx_);
	}
	//--------------------------------------------------------------------------------------
	//敵の更新
	//--------------------------------------------------------------------------------------
	void EnemyObj_UpDate(Object& obj_) {
		switch (obj_.typeNum) {
			case 0:
				EnemyObj0_UpDate(obj_);
				break;
			case 1:
				EnemyObj1_UpDate(obj_);
				break;
		}
	}
	//--------------------------------------------------------------------------------------
	//敵の描画
	//--------------------------------------------------------------------------------------
	void EnemyObj_Draw(Object& obj_) {
		switch (obj_.typeNum) {
			case 0:
				EnemyObj0_Draw(obj_);
				break;
			case 1:
				EnemyObj1_Draw(obj_);
				break;
		}
	}
	//--------------------------------------------------------------------------------------
	//敵1剣士の初期化
	//--------------------------------------------------------------------------------------
	void EnemyObj0_Initialize(Object& obj_, int idx_, int rank_, float x_, float y_) {
		obj_.charaState = CharaState::Stand;
		obj_.attackState = AttackState::Non;
		obj_.charaFaceDirect = CharaFaceDirect::Right;
		obj_.charaMoveDirect = CharaMoveDirect::Right;
		obj_.x = x_;
		obj_.y = y_;
		obj_.w = 100;
		obj_.h = 100;
		obj_.rank = rank_;
		obj_.typeNum = 0;
		obj_.charaIdx = idx_;
		obj_.standCnt = 0;
		obj_.walkCnt = 0;
		obj_.deadCnt = 0;
		obj_.commonCnt = 0;
		obj_.speed = 0.7f;
		obj_.healthCnt = 2000.0f;
		obj_.healthNow = obj_.healthCnt;
		obj_.injuryRadius = 10.0f;
		obj_.damageMin = 50.0f;
		obj_.damageMax = 70.0f;
		obj_.attackMinRadius = 1.5f;
		obj_.attackMaxRadius = 13.5f;
		obj_.lstTarX = 0.0f;
		obj_.lstTarY = 0.0f;
	}
	//--------------------------------------------------------------------------------------
	//敵1剣士の更新
	//--------------------------------------------------------------------------------------
	void EnemyObj0_UpDate(Object& obj_) {
		if(obj_.charaState == CharaState::Stand) {
			obj_.walkCnt = 0;
			EnemyWalkToTar_Determine(obj_, playerObjs[playerType]);
			obj_.standCnt++;
			//プレイヤーが死亡すると攻撃が停止する
			if (playerObjs[playerType].charaState == CharaState::Dead || playerObjs[playerType].charaState == CharaState::Non) obj_.attackState = AttackState::Non;
			if (obj_.attackState == AttackState::Normal) {
				obj_.standCnt = 0;
				obj_.commonCnt++;
				if ((obj_.commonCnt / 5) % 6 == 5) {
					EnemyDamage_Calculate(obj_, true);
					obj_.commonCnt = 0;
					obj_.attackState = AttackState::Non;
				}
			}
		} else if (obj_.charaState == CharaState::Walk) {
			obj_.standCnt = 0;
			EnemyWalkToTar_Determine(obj_, playerObjs[playerType]);
			obj_.walkCnt++;
		} else if (obj_.charaState == CharaState::Dead) {
			obj_.standCnt = 0;
			obj_.walkCnt = 0;
			obj_.deadCnt++;
			if ((obj_.deadCnt / 15) % 4 == 3) {
				obj_.deadCnt = 0;
				obj_.charaState = CharaState::Non;
			}
		}
	}
	//--------------------------------------------------------------------------------------
	//敵1剣士の描画
	//--------------------------------------------------------------------------------------
	void EnemyObj0_Draw(Object& obj_) {
		ML::Box2D draw(0, 0, obj_.w, obj_.h);
		draw.w = obj_.charaFaceDirect == CharaFaceDirect::Left ? 0 - draw.w : draw.w;
		draw.Offset(obj_.x - (obj_.charaFaceDirect == CharaFaceDirect::Left ? draw.w : 0), obj_.y);
		if (obj_.charaState == CharaState::Stand) {
			if (obj_.attackState == AttackState::Normal) {
				int animTable[] = { 0, 1, 2, 3, 4, 5 };
				ML::Box2D src(animTable[(obj_.commonCnt / 5) % 6] * obj_.w, 200, obj_.w, obj_.h);
				enemyImg0->Draw(draw, src);
			} else {
				int animTable[] = { 0, 1, 2, 3, 4, 5 };
				ML::Box2D src(animTable[(obj_.standCnt / 5) % 6] * obj_.w, 0, obj_.w, obj_.h);
				enemyImg0->Draw(draw, src);
			}
		} else if (obj_.charaState == CharaState::Walk) {
			if (obj_.attackState == AttackState::Normal) {
				int animTable[] = { 0, 1, 2, 3, 4, 5 };
				ML::Box2D src(animTable[(obj_.commonCnt / 5) % 6] * obj_.w, 200, obj_.w, obj_.h);
				enemyImg0->Draw(draw, src);
			} else {
				int animTable[] = { 0, 1, 2, 3, 4, 5, 6, 7 };
				ML::Box2D src(animTable[(obj_.walkCnt / 5) % 8] * obj_.w, 100, obj_.w, obj_.h);
				enemyImg0->Draw(draw, src);
			}
		} else if (obj_.charaState == CharaState::Dead) {
			int animTable[] = { 0, 1, 2, 3 };
			ML::Box2D src(animTable[(obj_.deadCnt / 15) % 4] * obj_.w, 700, obj_.w, obj_.h);
			enemyImg0->Draw(draw, src);
		}
	}
	//--------------------------------------------------------------------------------------
	//敵2魔法士の初期化
	//--------------------------------------------------------------------------------------
	void EnemyObj1_Initialize(Object& obj_, int idx_, int rank_, float x_, float y_) {
		obj_.charaState = CharaState::Stand;
		obj_.attackState = AttackState::Non;
		obj_.charaFaceDirect = CharaFaceDirect::Right;
		obj_.charaMoveDirect = CharaMoveDirect::Right;
		obj_.x = x_;
		obj_.y = y_;
		obj_.w = 100;
		obj_.h = 100;
		obj_.rank = rank_;
		obj_.typeNum = 1;
		obj_.charaIdx = idx_;
		obj_.standCnt = 0;
		obj_.walkCnt = 0;
		obj_.deadCnt = 0;
		obj_.commonCnt = 0;
		obj_.speed = 0.6f;
		obj_.healthCnt = 1200.0f;
		obj_.healthNow = obj_.healthCnt;
		obj_.injuryRadius = 8.5f;
		obj_.damageMin = 60.0f;
		obj_.damageMax = 80.0f;
		obj_.attackMinRadius = 50.5f;
		obj_.attackMaxRadius = 200.0f;
		obj_.lstTarX = 0.0f;
		obj_.lstTarY = 0.0f;
	}
	//--------------------------------------------------------------------------------------
	//敵2魔法士の更新
	//--------------------------------------------------------------------------------------
	void EnemyObj1_UpDate(Object& obj_) {
		if (obj_.charaState == CharaState::Stand) {
			obj_.walkCnt = 0;
			EnemyWalkToTar_Determine(obj_, playerObjs[playerType]);
			obj_.standCnt++;
			//プレイヤーが死亡すると攻撃が停止する
			if (playerObjs[playerType].charaState == CharaState::Dead || playerObjs[playerType].charaState == CharaState::Non) obj_.attackState = AttackState::Non;
			if (obj_.attackState == AttackState::Normal) {
				obj_.standCnt = 0;
				obj_.commonCnt++;
				WizardShotObj_Appear(obj_);
				if ((obj_.commonCnt / 5) % 6 == 5) {
					obj_.commonCnt = 0;
					obj_.attackState = AttackState::Non;
				}
			}
		} else if (obj_.charaState == CharaState::Walk) {
			obj_.standCnt = 0;
			EnemyWalkToTar_Determine(obj_, playerObjs[playerType]);
			obj_.walkCnt++;
		} else if(obj_.charaState == CharaState::Dead){
			obj_.standCnt = 0;
			obj_.walkCnt = 0;
			obj_.deadCnt++;
			if ((obj_.deadCnt / 15) % 4 == 3) {
				obj_.deadCnt = 0;
				obj_.charaState = CharaState::Non;
			}
		}
	}
	//--------------------------------------------------------------------------------------
	//敵2魔法士の描画
	//--------------------------------------------------------------------------------------
	void EnemyObj1_Draw(Object& obj_) {
		ML::Box2D draw(0, 0, obj_.w, obj_.h);
		draw.w = obj_.charaFaceDirect == CharaFaceDirect::Left ? 0 - draw.w : draw.w;
		draw.Offset(obj_.x - (obj_.charaFaceDirect == CharaFaceDirect::Left ? draw.w : 0), obj_.y);
		if (obj_.charaState == CharaState::Stand) {
			if (obj_.attackState == AttackState::Normal) {
				int animTable[] = { 0, 1, 2, 3, 4, 5 };
				ML::Box2D src(animTable[(obj_.commonCnt / 5) % 6] * obj_.w, 200, obj_.w, obj_.h);
				enemyImg1->Draw(draw, src);
			} else {
				int animTable[] = { 0, 1, 2, 3, 4, 5 };
				ML::Box2D src(animTable[(obj_.standCnt / 5) % 6] * obj_.w, 0, obj_.w, obj_.h);
				enemyImg1->Draw(draw, src);
			}
		} else if (obj_.charaState == CharaState::Walk) {
			if (obj_.attackState == AttackState::Normal) {
				int animTable[] = { 0, 1, 2, 3, 4, 5 };
				ML::Box2D src(animTable[(obj_.commonCnt / 5) % 6] * obj_.w, 200, obj_.w, obj_.h);
				enemyImg1->Draw(draw, src);
			} else {
				int animTable[] = { 0, 1, 2, 3, 4, 5, 6 ,7 };
				ML::Box2D src(animTable[(obj_.walkCnt / 5) % 8] * obj_.w, 100, obj_.w, obj_.h);
				enemyImg1->Draw(draw, src);
			}
		} else if (obj_.charaState == CharaState::Dead) {
			int animTable[] = { 0, 1, 2, 3 };
			ML::Box2D src(animTable[(obj_.deadCnt / 15) % 4] * obj_.w, 700, obj_.w, obj_.h);
			enemyImg1->Draw(draw, src);
		}
	}
	//--------------------------------------------------------------------------------------
	//プレーヤーの初期化
	//--------------------------------------------------------------------------------------
	void PlayerObj_Initialize(Object& obj_, int idx_, int type_, float x_, float y_) {
		switch (type_) {
			case 0:
				PlayerObj0_Initialize(obj_, idx_, x_, y_);
				break;
			case 1:
				PlayerObj1_Initialize(obj_, idx_, x_, y_);
				break;
		}
		PlayerHealthObj_Appear(playerHealthObjs[playerType], playerType);
	}
	//--------------------------------------------------------------------------------------
	//プレーヤーの更新
	//--------------------------------------------------------------------------------------
	void PlayerObj_UpDate(Object& obj_) {
		switch (obj_.typeNum) {
			case 0:
				PlayerObj0_UpDate(obj_);
				break;
			case 1:
				PlayerObj1_UpDate(obj_);
				break;
		}
	}
	//--------------------------------------------------------------------------------------
	//プレーヤーの描画
	//--------------------------------------------------------------------------------------
	void PlayerObj_Draw(Object& obj_) {
		switch (obj_.typeNum) {
			case 0:
				PlayerObj0_Draw(obj_);
				break;
			case 1:
				PlayerObj1_Draw(obj_);
				break;
		}
	}
	//--------------------------------------------------------------------------------------
	//プレーヤー1スケルトンの初期化
	//--------------------------------------------------------------------------------------
	void PlayerObj0_Initialize(Object& obj_, int idx_, float x_, float y_) {
		obj_.charaState = CharaState::Stand;
		obj_.attackState = AttackState::Non;
		obj_.charaFaceDirect = CharaFaceDirect::Right;
		obj_.charaMoveDirect = CharaMoveDirect::Right;
		obj_.x = x_;
		obj_.y = y_;
		obj_.w = 100;
		obj_.h = 100;
		obj_.commonCnt = 0;
		obj_.typeNum = 0;
		obj_.charaIdx = idx_;
		obj_.standCnt = 0;
		obj_.walkCnt = 0;
		obj_.deadCnt = 0;
		obj_.commonCnt = 0;
		obj_.speed = 3.2f;
		obj_.healthCnt = 40000.0f;
		obj_.healthNow = obj_.healthCnt;
		obj_.injuryRadius = 10.0f;
		obj_.damageMin = 150.0f;
		obj_.damageMax = 180.0f;
		obj_.attackMinRadius = 0.0f;
		obj_.attackMaxRadius = 28.5f;
	}
	//--------------------------------------------------------------------------------------
	//プレーヤー1スケルトンの更新
	//--------------------------------------------------------------------------------------
	void PlayerObj0_UpDate(Object& obj_) {
		if (obj_.charaState != CharaState::Dead) {
			auto gpi = gamePadIn->GetState();
			if (obj_.charaState == CharaState::Stand) {
				obj_.walkCnt = 0;
				auto gpi = gamePadIn->GetState();
				if (gpi.LStick.BL.on || gpi.LStick.BU.on || gpi.LStick.BR.on || gpi.LStick.BD.on) obj_.charaState = CharaState::Walk;
				if (gpi.SE.on && obj_.attackState == AttackState::Non) {
					obj_.attackTypeNum = rand() % 2;
					if (obj_.attackTypeNum == 0) playerHit0->Play_Normal(false);
					else playerHit1->Play_Normal(false);
					obj_.attackState = AttackState::Normal;
				}
				if (obj_.attackState == AttackState::Normal) {
					obj_.commonCnt++;
					if ((obj_.commonCnt / 5) % 6 == 5) {
						PlayerDamage_Calculate(obj_, true);
						obj_.commonCnt = 0;
						obj_.attackState = AttackState::Non;
					}
				} else obj_.standCnt++;
			} else if (obj_.charaState == CharaState::Walk) {
				obj_.standCnt = 0;
				float realSpeed = 0.0f;
				if (gpi.LStick.BL.on) {
					obj_.charaFaceDirect = CharaFaceDirect::Left;
					realSpeed = obj_.speed;
					obj_.charaMoveDirect = CharaMoveDirect::Left;
					if (gpi.LStick.BU.on) {
						realSpeed = sqrt((obj_.speed * obj_.speed) / 2);
						obj_.charaMoveDirect = CharaMoveDirect::UpLeft;
					}
					if (gpi.LStick.BD.on) {
						realSpeed = sqrt((obj_.speed * obj_.speed) / 2);
						obj_.charaMoveDirect = CharaMoveDirect::DownLeft;
					}
				}
				if (gpi.LStick.BR.on) {
					obj_.charaFaceDirect = CharaFaceDirect::Right;
					realSpeed = obj_.speed;
					obj_.charaMoveDirect = CharaMoveDirect::Right;
					if (gpi.LStick.BU.on) {
						realSpeed = sqrt((obj_.speed * obj_.speed) / 2);
						obj_.charaMoveDirect = CharaMoveDirect::UpRight;
					}
					if (gpi.LStick.BD.on) {
						realSpeed = sqrt((obj_.speed * obj_.speed) / 2);
						obj_.charaMoveDirect = CharaMoveDirect::DownRight;
					}
				}
				if (gpi.LStick.BU.on) {
					if (gpi.LStick.BL.off && gpi.LStick.BR.off) {
						realSpeed = obj_.speed;
						obj_.charaMoveDirect = CharaMoveDirect::Up;
					}
					if (gpi.LStick.BD.on) {
						realSpeed = 0.0f;
						obj_.charaMoveDirect = CharaMoveDirect::Down;
					}
				}
				if (gpi.LStick.BD.on) {
					if (gpi.LStick.BL.off && gpi.LStick.BR.off) {
						realSpeed = obj_.speed;
						obj_.charaMoveDirect = CharaMoveDirect::Down;
					}
					if (gpi.LStick.BU.on) {
						realSpeed = 0.0f;
						obj_.charaMoveDirect = CharaMoveDirect::Up;
					}
				}
				float realSpeedX = realSpeed;
				float realSpeedY = realSpeed;
				//プレイヤーの移動範囲を制限する
				if (camera.x >= windowW / 2.0f - 5.6f) {
					camera.x = windowW / 2.0f - 5.6f;
					if (obj_.charaMoveDirect == CharaMoveDirect::Left
						|| obj_.charaMoveDirect == CharaMoveDirect::UpLeft
						|| obj_.charaMoveDirect == CharaMoveDirect::DownLeft) {
						realSpeedX = 0.0f;
					}
				}
				if (camera.x <= (blockRow - 1) * -640.0f + (windowW / -2.0f + 4.6f)) {
					camera.x = (blockRow - 1) * -640.0f + (windowW / -2.0f + 4.6f);
					if (obj_.charaMoveDirect == CharaMoveDirect::Right
						|| obj_.charaMoveDirect == CharaMoveDirect::UpRight
						|| obj_.charaMoveDirect == CharaMoveDirect::DownRight) {
						realSpeedX = 0.0f;
					}
				}
				if (camera.y >= windowH / 2.0f + 3.3f) {
					camera.y = windowH / 2.0f + 3.3f;
					if (obj_.charaMoveDirect == CharaMoveDirect::Up
						|| obj_.charaMoveDirect == CharaMoveDirect::UpLeft
						|| obj_.charaMoveDirect == CharaMoveDirect::UpRight) {
						realSpeedY = 0.0f;
					}
				}
				if (camera.y <= (blockCol - 1) * -360.0f + (windowH / -2.0f + 8.1f)) {
					camera.y = (blockCol - 1) * -360.0f + (windowH / -2.0f + 8.1f);
					if (obj_.charaMoveDirect == CharaMoveDirect::Down
						|| obj_.charaMoveDirect == CharaMoveDirect::DownLeft
						|| obj_.charaMoveDirect == CharaMoveDirect::DownRight) {
						realSpeedY = 0.0f;
					}
				}
				for (int ccr = 0; ccr < _countof(enemyObjs); ++ccr) {
					CharaMove_Calculate_Relative(obj_, enemyObjs[ccr], realSpeedX, realSpeedY);
				}
				for (int ccr = 0; ccr < _countof(slimeShotObjs); ++ccr) {
					CharaMove_Calculate_Relative(obj_, slimeShotObjs[ccr], realSpeedX, realSpeedY);
				}
				for (int wsox = 0; wsox < wizardNum; ++wsox) {
					for (int wsoy = 0; wsoy < _countof(wizardShotObjs[wsox]); ++wsoy) {
						CharaMove_Calculate_Relative(obj_, wizardShotObjs[wsox][wsoy], realSpeedX, realSpeedY);
					}
				}
				CharaMove_Calculate_Relative(obj_, camera, realSpeedX, realSpeedY);
				if (gpi.LStick.BL.up || gpi.LStick.BU.up || gpi.LStick.BR.up || gpi.LStick.BD.up) obj_.charaState = CharaState::Stand;
				if (gpi.SE.on && obj_.attackState == AttackState::Non) {
					obj_.attackTypeNum = rand() % 2;
					if (obj_.attackTypeNum == 0) playerHit0->Play_Normal(false);
					else playerHit1->Play_Normal(false);
					obj_.attackState = AttackState::Normal;
				}
				if (obj_.attackState == AttackState::Normal) {
					obj_.commonCnt++;
					if ((obj_.commonCnt / 5) % 6 == 5) {
						PlayerDamage_Calculate(obj_, true);
						obj_.commonCnt = 0;
						obj_.attackState = AttackState::Non;
					}
				} else obj_.walkCnt++;
			}
		} else if (obj_.charaState == CharaState::Dead) {
			obj_.standCnt = 0;
			obj_.walkCnt = 0;
			obj_.deadCnt++;
			if ((obj_.deadCnt / 15) % 4 == 3) {
				obj_.deadCnt = 0;
				obj_.charaState = CharaState::Non;
			}
		}
	}
	//--------------------------------------------------------------------------------------
	//プレーヤー1スケルトンの描画
	//--------------------------------------------------------------------------------------
	void PlayerObj0_Draw(Object& obj_) {
		ML::Box2D draw(0, 0, obj_.w, obj_.h);
		draw.w = obj_.charaFaceDirect == CharaFaceDirect::Left ? 0 - draw.w : draw.w;
		draw.Offset(obj_.x - (obj_.charaFaceDirect == CharaFaceDirect::Left ? draw.w : 0), obj_.y);
		if (obj_.charaState == CharaState::Stand) {
			if (obj_.attackState == AttackState::Normal) {
				if (obj_.attackTypeNum == 0) {
					int animTable[] = { 0, 1, 2, 3, 4, 5 };
					ML::Box2D src(animTable[(obj_.commonCnt / 5) % 6] * obj_.w, 300, obj_.w, obj_.h);
					playerImg0->Draw(draw, src);
				} else {
					int animTable[] = { 0, 1, 2, 3, 4, 5 };
					ML::Box2D src(animTable[(obj_.commonCnt / 5) % 6] * obj_.w, 200, obj_.w, obj_.h);
					playerImg0->Draw(draw, src);
				}
			} else {
				int animTable[] = { 0, 1, 2, 3, 4, 5 };
				ML::Box2D src(animTable[(obj_.standCnt / 5) % 6] * obj_.w, 0, obj_.w, obj_.h);
				playerImg0->Draw(draw, src);
			}
		} else if (obj_.charaState == CharaState::Walk) {
			if (obj_.attackState == AttackState::Normal) {
				if (obj_.attackTypeNum == 0) {
					int animTable[] = { 0, 1, 2, 3, 4, 5 };
					ML::Box2D src(animTable[(obj_.commonCnt / 5) % 6] * obj_.w, 300, obj_.w, obj_.h);
					playerImg0->Draw(draw, src);
				} else {
					int animTable[] = { 0, 1, 2, 3, 4, 5 };
					ML::Box2D src(animTable[(obj_.commonCnt / 5) % 6] * obj_.w, 200, obj_.w, obj_.h);
					playerImg0->Draw(draw, src);
				}
			} else {
				int animTable[] = { 0, 1, 2, 3, 4, 5, 6, 7 };
				ML::Box2D src(animTable[(obj_.walkCnt / 5) % 8] * obj_.w, 100, obj_.w, obj_.h);
				playerImg0->Draw(draw, src);
			}
		} else if (obj_.charaState == CharaState::Dead) {
			int animTable[] = { 0, 1, 2, 3 };
			ML::Box2D src(animTable[(obj_.deadCnt / 15) % 4] * obj_.w, 600, obj_.w, obj_.h);
			playerImg0->Draw(draw, src);
		}
	}
	//--------------------------------------------------------------------------------------
	//プレーヤー2スライムの初期化
	//--------------------------------------------------------------------------------------
	void PlayerObj1_Initialize(Object& obj_, int idx_, float x_, float y_) {
		obj_.charaState = CharaState::Stand;
		obj_.attackState = AttackState::Non;
		obj_.charaFaceDirect = CharaFaceDirect::Right;
		obj_.charaMoveDirect = CharaMoveDirect::Right;
		obj_.x = x_;
		obj_.y = y_;
		obj_.w = 100;
		obj_.h = 100;		
		obj_.commonCnt = 0;
		obj_.typeNum = 1;
		obj_.charaIdx = idx_;
		obj_.standCnt = 0;
		obj_.walkCnt = 0;
		obj_.deadCnt = 0;
		obj_.commonCnt = 0;
		obj_.speed = 2.8f;
		obj_.healthCnt = 40000.0f;
		obj_.healthNow = obj_.healthCnt;
		obj_.injuryRadius = 10.0f;
		obj_.damageMin = 200.0f;
		obj_.damageMax = 250.0f;
	}
	//--------------------------------------------------------------------------------------
	//プレーヤー2スライムの更新
	//--------------------------------------------------------------------------------------
	void PlayerObj1_UpDate(Object& obj_) {
		if (obj_.charaState != CharaState::Dead) {
			auto gpi = gamePadIn->GetState();
			if (obj_.charaState == CharaState::Stand) {
				obj_.walkCnt = 0;
				auto gpi = gamePadIn->GetState();
				if (gpi.LStick.BL.on || gpi.LStick.BU.on || gpi.LStick.BR.on || gpi.LStick.BD.on) obj_.charaState = CharaState::Walk;
				if (gpi.SE.on && obj_.attackState == AttackState::Non) {
					playerMage->Play_Normal(false);
					obj_.attackState = AttackState::Normal;
					SlimeShotObj_Appear(obj_);
				}
				if (obj_.attackState == AttackState::Normal) {
					obj_.commonCnt++;
					if ((obj_.commonCnt / 5) % 6 == 5) {
						obj_.commonCnt = 0;
						obj_.attackState = AttackState::Non;
					}
				} else obj_.standCnt++;
			} else if (obj_.charaState == CharaState::Walk) {
				obj_.standCnt = 0;
				float realSpeed = 0.0f;
				if (gpi.LStick.BL.on) {
					obj_.charaFaceDirect = CharaFaceDirect::Left;
					realSpeed = obj_.speed;
					obj_.charaMoveDirect = CharaMoveDirect::Left;
					if (gpi.LStick.BU.on) {
						realSpeed = sqrt((obj_.speed * obj_.speed) / 2);
						obj_.charaMoveDirect = CharaMoveDirect::UpLeft;
					}
					if (gpi.LStick.BD.on) {
						realSpeed = sqrt((obj_.speed * obj_.speed) / 2);
						obj_.charaMoveDirect = CharaMoveDirect::DownLeft;
					}
				}
				if (gpi.LStick.BR.on) {
					obj_.charaFaceDirect = CharaFaceDirect::Right;
					realSpeed = obj_.speed;
					obj_.charaMoveDirect = CharaMoveDirect::Right;
					if (gpi.LStick.BU.on) {
						realSpeed = sqrt((obj_.speed * obj_.speed) / 2);
						obj_.charaMoveDirect = CharaMoveDirect::UpRight;
					}
					if (gpi.LStick.BD.on) {
						realSpeed = sqrt((obj_.speed * obj_.speed) / 2);
						obj_.charaMoveDirect = CharaMoveDirect::DownRight;
					}
				}
				if (gpi.LStick.BU.on) {
					if (gpi.LStick.BL.off && gpi.LStick.BR.off) {
						realSpeed = obj_.speed;
						obj_.charaMoveDirect = CharaMoveDirect::Up;
					}
					if (gpi.LStick.BD.on) {
						realSpeed = 0.0f;
						obj_.charaMoveDirect = CharaMoveDirect::Down;
					}
				}
				if (gpi.LStick.BD.on) {
					if (gpi.LStick.BL.off && gpi.LStick.BR.off) {
						realSpeed = obj_.speed;
						obj_.charaMoveDirect = CharaMoveDirect::Down;
					}
					if (gpi.LStick.BU.on) {
						realSpeed = 0.0f;
						obj_.charaMoveDirect = CharaMoveDirect::Up;
					}
				}
				float realSpeedX = realSpeed;
				float realSpeedY = realSpeed;
				//プレイヤーの移動範囲を制限する
				if (camera.x >= windowW / 2.0f - 5.6f) {
					camera.x = windowW / 2.0f - 5.6f;
					if (obj_.charaMoveDirect == CharaMoveDirect::Left
						|| obj_.charaMoveDirect == CharaMoveDirect::UpLeft
						|| obj_.charaMoveDirect == CharaMoveDirect::DownLeft) {
						realSpeedX = 0.0f;
					}
				}
				if (camera.x <= (blockRow - 1) * -640.0f + (windowW / -2.0f + 4.6f)) {
					camera.x = (blockRow - 1) * -640.0f + (windowW / -2.0f + 4.6f);
					if (obj_.charaMoveDirect == CharaMoveDirect::Right
						|| obj_.charaMoveDirect == CharaMoveDirect::UpRight
						|| obj_.charaMoveDirect == CharaMoveDirect::DownRight) {
						realSpeedX = 0.0f;
					}
				}
				if (camera.y >= windowH / 2.0f + 3.3f) {
					camera.y = windowH / 2.0f + 3.3f;
					if (obj_.charaMoveDirect == CharaMoveDirect::Up
						|| obj_.charaMoveDirect == CharaMoveDirect::UpLeft
						|| obj_.charaMoveDirect == CharaMoveDirect::UpRight) {
						realSpeedY = 0.0f;
					}
				}
				if (camera.y <= (blockCol - 1) * -360.0f + (windowH / -2.0f + 8.1f)) {
					camera.y = (blockCol - 1) * -360.0f + (windowH / -2.0f + 8.1f);
					if (obj_.charaMoveDirect == CharaMoveDirect::Down
						|| obj_.charaMoveDirect == CharaMoveDirect::DownLeft
						|| obj_.charaMoveDirect == CharaMoveDirect::DownRight) {
						realSpeedY = 0.0f;
					}
				}
				for (int ccr = 0; ccr < _countof(enemyObjs); ++ccr) {
					CharaMove_Calculate_Relative(obj_, enemyObjs[ccr], realSpeedX, realSpeedY);
				}
				for (int ccr = 0; ccr < _countof(slimeShotObjs); ++ccr) {
					CharaMove_Calculate_Relative(obj_, slimeShotObjs[ccr], realSpeedX, realSpeedY);
				}
				for (int wsox = 0; wsox < wizardNum; ++wsox) {
					for (int wsoy = 0; wsoy < _countof(wizardShotObjs[wsox]); ++wsoy) {
						CharaMove_Calculate_Relative(obj_, wizardShotObjs[wsox][wsoy], realSpeedX, realSpeedY);
					}
				}
				CharaMove_Calculate_Relative(obj_, camera, realSpeedX, realSpeedY);
				if (obj_.x < -23.0f) obj_.x = -23.0f;
				else if (obj_.x > windowW - 47.0f) obj_.x = windowW - 47.0f;
				if (obj_.y < -25.0f) obj_.y = -25.0f;
				else if (obj_.y > windowH - 43.0f) obj_.y = windowH - 43.0f;
				if (gpi.LStick.BL.up || gpi.LStick.BU.up || gpi.LStick.BR.up || gpi.LStick.BD.up) obj_.charaState = CharaState::Stand;
				if (gpi.SE.on && obj_.attackState == AttackState::Non) {
					playerMage->Play_Normal(false);
					obj_.attackState = AttackState::Normal;
					SlimeShotObj_Appear(obj_);
				}
				if (obj_.attackState == AttackState::Normal) {
					obj_.commonCnt++;
					if ((obj_.commonCnt / 5) % 6 == 5) {
						obj_.commonCnt = 0;
						obj_.attackState = AttackState::Non;
					}
				} else obj_.walkCnt++;
			}
		} else if (obj_.charaState == CharaState::Dead) {
			obj_.standCnt = 0;
			obj_.walkCnt = 0;
			obj_.deadCnt++;
			if ((obj_.deadCnt / 15) % 4 == 3) {
				obj_.deadCnt = 0;
				obj_.charaState = CharaState::Non;
			}
		}
	}
	//--------------------------------------------------------------------------------------
	//プレーヤー2スライムの描画
	//--------------------------------------------------------------------------------------
	void PlayerObj1_Draw(Object& obj_) {
		ML::Box2D draw(0, 0, obj_.w, obj_.h);
		draw.w = obj_.charaFaceDirect == CharaFaceDirect::Left ? 0 - draw.w : draw.w;
		draw.Offset(obj_.x - (obj_.charaFaceDirect == CharaFaceDirect::Left ? draw.w : 0), obj_.y);
		if (obj_.charaState == CharaState::Stand) {
			if (obj_.attackState == AttackState::Normal) {
				int animTable[] = { 0, 1, 2, 3, 4, 5 };
				ML::Box2D src(animTable[(obj_.commonCnt / 3) % 6] * obj_.w, 200, obj_.w, obj_.h);
				playerImg1->Draw(draw, src);
			} else {
				int animTable[] = { 0, 1, 2, 3, 4, 5 };
				ML::Box2D src(animTable[(obj_.standCnt / 5) % 6] * obj_.w, 0, obj_.w, obj_.h);
				playerImg1->Draw(draw, src);
			}
		} else if (obj_.charaState == CharaState::Walk) {
			if (obj_.attackState == AttackState::Normal) {
				int animTable[] = { 0, 1, 2, 3, 4, 5 };
				ML::Box2D src(animTable[(obj_.commonCnt / 3) % 6] * obj_.w, 200, obj_.w, obj_.h);
				playerImg1->Draw(draw, src);
			} else {
				int animTable[] = { 0, 1, 2, 3, 4, 5 };
				ML::Box2D src(animTable[(obj_.walkCnt / 5) % 6] * obj_.w, 100, obj_.w, obj_.h);
				playerImg1->Draw(draw, src);
			}
		} else if (obj_.charaState == CharaState::Dead) {
			int animTable[] = { 0, 1, 2, 3 };
			ML::Box2D src(animTable[(obj_.deadCnt / 15) % 4] * obj_.w, 400, obj_.w, obj_.h);
			playerImg1->Draw(draw, src);
		}
	}
	//--------------------------------------------------------------------------------------
	//弾の初期化
	//--------------------------------------------------------------------------------------
	void ShotObj_Initialize(Object& obj_, int tn_, Object& cObj_) {
		switch (tn_) {
			case 0:
				ShotObj0_Initialize(obj_, cObj_);
				break;
			case 1:
				ShotObj1_Initialize(obj_, cObj_);
				break;
		}
	}
	//--------------------------------------------------------------------------------------
	//弾の更新
	//--------------------------------------------------------------------------------------
	void ShotObj_UpDate(Object& obj_) {
		switch (obj_.typeNum) {
			case 0:
				ShotObj0_UpDate(obj_);
				break;
			case 1:
				ShotObj1_UpDate(obj_);
				break;
		}
	}
	//--------------------------------------------------------------------------------------
	//弾の描画
	//--------------------------------------------------------------------------------------
	void ShotObj_Draw(Object& obj_) {
		switch (obj_.typeNum) {
			case 0:
				ShotObj0_Draw(obj_);
				break;
			case 1:
				ShotObj1_Draw(obj_);
				break;
		}
	}
	//--------------------------------------------------------------------------------------
	//スライム弾の初期化
	//--------------------------------------------------------------------------------------
	void ShotObj0_Initialize(Object& obj_, Object& cObj_) {
		obj_.charaState = cObj_.charaState;
		obj_.attackState = AttackState::Normal;
		obj_.charaFaceDirect = cObj_.charaFaceDirect;
		obj_.charaMoveDirect = cObj_.charaMoveDirect;
		obj_.x = cObj_.x + cObj_.w / 2;
		obj_.y = cObj_.y + cObj_.h / 2;
		obj_.w = 100;
		obj_.h = 100;
		obj_.typeNum = 0;
		obj_.dissipateCnt = 0;
		obj_.commonCnt = 0;
		obj_.speed = 4.8f;
		obj_.angle = 0.0f;
		obj_.damageMin = cObj_.damageMin;
		obj_.damageMax = cObj_.damageMax;
		obj_.attackMinRadius = 18.5f;
		obj_.attackMaxRadius = 20.5f;
	}
	//--------------------------------------------------------------------------------------
	//スライム弾の更新
	//--------------------------------------------------------------------------------------
	void ShotObj0_UpDate(Object& obj_) {
		if (obj_.attackState == AttackState::Normal) {
			if (obj_.charaState == CharaState::Stand) {
				if(obj_.charaFaceDirect == CharaFaceDirect::Right) obj_.charaMoveDirect = CharaMoveDirect::Right;
				else obj_.charaMoveDirect = CharaMoveDirect::Left;
			}
			//8方向に弾を発射できるが、操作が変すぎるため、左右のみ弾を発射できると決めた。
			/*float realSpeed = 0.0f;
			switch (obj_.charaMoveDirect) {
				case CharaMoveDirect::Left:
				case CharaMoveDirect::Up:
				case CharaMoveDirect::Right:
				case CharaMoveDirect::Down:
					realSpeed = obj_.speed;
					break;
				default:
					realSpeed = sqrt((obj_.speed * obj_.speed) / 2);
					break;
			}
			CharaMove_Calculate_Absolute(obj_, obj_.speed, obj_.speed);*/
			switch (obj_.charaMoveDirect) {
			case CharaMoveDirect::Left:
			case CharaMoveDirect::UpLeft:
			case CharaMoveDirect::DownLeft:
				obj_.x -= obj_.speed;
				break;
			case CharaMoveDirect::UpRight:
			case CharaMoveDirect::Right:
			case CharaMoveDirect::DownRight:
				obj_.x += obj_.speed;
				break;
			case CharaMoveDirect::Up:
			case CharaMoveDirect::Down:
				if (obj_.charaFaceDirect == CharaFaceDirect::Right) obj_.x += obj_.speed;
				else obj_.x -= obj_.speed;
				break;
			}
			obj_.commonCnt++;
			PlayerDamage_Calculate(obj_, false);
			obj_.dissipateCnt++;
			if (obj_.dissipateCnt % (2 * 60) == 0) {
				obj_.attackState = AttackState::Non;
			}
		}
	}
	//--------------------------------------------------------------------------------------
	//スライム弾の描画
	//--------------------------------------------------------------------------------------
	void ShotObj0_Draw(Object& obj_) {
		if (obj_.attackState == AttackState::Normal) {
			ML::Box2D draw(0 - obj_.w / 2, 0 - obj_.h / 2, obj_.w, obj_.h);
			draw.w = obj_.charaFaceDirect == CharaFaceDirect::Left ? 0 - draw.w : draw.w;
			draw.Offset(obj_.x - (obj_.charaFaceDirect == CharaFaceDirect::Left ? draw.w : 0), obj_.y);
			int animTable[] = { 0, 1, 2, 3, 4, 5 };
			ML::Box2D src(animTable[(obj_.commonCnt / 5) % 6] * obj_.w, 0, obj_.w, obj_.h);
			shotImg0->Draw(draw, src);
		}
	}
	//--------------------------------------------------------------------------------------
	//魔法士弾の初期化
	//--------------------------------------------------------------------------------------
	void ShotObj1_Initialize(Object& obj_, Object& cObj_) {
		obj_.charaState = cObj_.charaState;
		obj_.attackState = AttackState::Normal;
		obj_.charaFaceDirect = cObj_.charaFaceDirect;
		obj_.charaMoveDirect = cObj_.charaMoveDirect;
		obj_.x = cObj_.x + cObj_.w / 2;
		obj_.y = cObj_.y + cObj_.h / 2;
		obj_.w = 100;
		obj_.h = 100;
		obj_.typeNum = 1;
		obj_.dissipateCnt = 0;
		obj_.commonCnt = 0;
		obj_.speed = 1.8f;
		obj_.angle = atan2((playerObjs[playerType].y + playerObjs[playerType].h / 2) - obj_.y, (playerObjs[playerType].x + playerObjs[playerType].w / 2) - obj_.x);
		obj_.damageMin = cObj_.damageMin;
		obj_.damageMax = cObj_.damageMax;
		obj_.attackMinRadius = 12.5f;
		obj_.attackMaxRadius = 12.5f;
	}
	//--------------------------------------------------------------------------------------
	//魔法士弾の更新
	//--------------------------------------------------------------------------------------
	void ShotObj1_UpDate(Object& obj_) {
		if (obj_.attackState == AttackState::Normal) {
			float mx = cos(obj_.angle);
			float my = sin(obj_.angle);
			obj_.x += mx * obj_.speed;
			obj_.y += my * obj_.speed;
			if (obj_.x <= -60.0f || (obj_.x >= windowW + 60.0f) ||
				obj_.y <= -60.0f || (obj_.y >= windowH + 60.0f)) {
				obj_.attackState = AttackState::Non;
			}
			obj_.commonCnt++;
			EnemyDamage_Calculate(obj_, false);
			obj_.dissipateCnt++;
			if (obj_.dissipateCnt % (2 * 60) == 0) {
				obj_.attackState = AttackState::Non;
			}
		}
	}
	//--------------------------------------------------------------------------------------
	//魔法士弾の描画
	//--------------------------------------------------------------------------------------
	void ShotObj1_Draw(Object& obj_) {
		if (obj_.attackState == AttackState::Normal) {
			ML::Box2D draw(0 - obj_.w / 2, 0 - obj_.h / 2, obj_.w, obj_.h);
			draw.Offset(obj_.x, obj_.y);
			int animTable[] = { 0, 1, 2, 3, 4, 5 };
			ML::Box2D src(animTable[(obj_.commonCnt / 5) % 6] * obj_.w, 0, obj_.w, obj_.h);
			shotImg1->Rotation(obj_.angle, ML::Vec2(obj_.w / 2.0f, obj_.h / 2.0f));
			shotImg1->Draw(draw, src);
		}
	}
	//--------------------------------------------------------------------------------------
	//ヘルスバーの初期化
	//--------------------------------------------------------------------------------------
	void HealthObj_Initialize(Object& obj_, int type_, int cObjIdx_, bool dis_) {
		obj_.x = type_ == 0 ? playerHealthObjs[cObjIdx_].x : enemyHealthObjs[cObjIdx_].x;
		obj_.y = type_ == 0 ? playerHealthObjs[cObjIdx_].y : enemyHealthObjs[cObjIdx_].y;
		obj_.w = 60;
		obj_.h = 20;
		obj_.typeNum = type_;
		obj_.charaIdx = cObjIdx_;
		obj_.commonCnt = 0;
		obj_.healthCnt = 0.0f;
		obj_.healthNow = 0.0f;
		obj_.displayHealth = dis_;
	}
	//--------------------------------------------------------------------------------------
	//ヘルスバーの更新
	//--------------------------------------------------------------------------------------
	void HealthObj_UpDate(Object& obj_) {
		if (obj_.displayHealth) {
			obj_.commonCnt++;
		}
	}
	//--------------------------------------------------------------------------------------
	//ヘルスバーの描画
	//--------------------------------------------------------------------------------------
	void HealthObj_Draw(Object& obj_) {
		if (obj_.displayHealth) {
			if (obj_.typeNum == 0) {
				if (playerObjs[obj_.charaIdx].charaState != CharaState::Non) {
					ML::Box2D draw(0, 0, obj_.w, obj_.h);
					draw.Offset(playerObjs[obj_.charaIdx].x + (playerObjs[obj_.charaIdx].w - obj_.w) / 2, playerObjs[obj_.charaIdx].y + 20);
					obj_.healthCnt = playerObjs[obj_.charaIdx].healthCnt;
					obj_.healthNow = playerObjs[obj_.charaIdx].healthNow;
					float healthPercentF = ceil(obj_.healthNow / obj_.healthCnt * 5.0f);
					int healthPercent = (int) healthPercentF;
					ML::Box2D src(0, (5 - healthPercent) * obj_.h, obj_.w, obj_.h);
					healthImg0->Draw(draw, src);
				}
			} else if (obj_.typeNum == 1) {
				if (enemyObjs[obj_.charaIdx].charaState != CharaState::Non) {
					ML::Box2D draw(0, 0, obj_.w, obj_.h);
					draw.Offset(enemyObjs[obj_.charaIdx].x + (enemyObjs[obj_.charaIdx].w - obj_.w) / 2, enemyObjs[obj_.charaIdx].y + 20);
					obj_.healthCnt = enemyObjs[obj_.charaIdx].healthCnt;
					obj_.healthNow = enemyObjs[obj_.charaIdx].healthNow;
					float healthPercentF = ceil(obj_.healthNow / obj_.healthCnt * 5.0f);
					int healthPercent = (int) healthPercentF;
					ML::Box2D src(0, (5 - healthPercent) * obj_.h, obj_.w, obj_.h);
					healthImg1->Draw(draw, src);
				}
			}
		}
	}
	//--------------------------------------------------------------------------------------
	//敵のヘルスバーを現す
	//--------------------------------------------------------------------------------------
	void EnemyHealthObj_Appear(Object& obj_, int cObjIdx_) {
		HealthObj_Initialize(obj_, 1, cObjIdx_, true);
	}
	//--------------------------------------------------------------------------------------
	//プレーヤーのヘルスバーを現す
	//--------------------------------------------------------------------------------------
	void PlayerHealthObj_Appear(Object& obj_, int cObjIdx_) {
		HealthObj_Initialize(obj_, 0, cObjIdx_, true);
	}
	//--------------------------------------------------------------------------------------
	//スライムの弾を現す
	//--------------------------------------------------------------------------------------
	void SlimeShotObj_Appear(Object& cObj_) {
		for (int sso = 0; sso < _countof(slimeShotObjs); ++sso) {
			if (slimeShotObjs[sso].attackState == AttackState::Non) {
				ShotObj_Initialize(slimeShotObjs[sso], 0, cObj_);
				break;
			}
		}
	}
	//--------------------------------------------------------------------------------------
	//魔法士弾の弾を現す
	//--------------------------------------------------------------------------------------
	void WizardShotObj_Appear(Object& cObj_) {
		for (int wso = 0; wso < _countof(wizardShotObjs[cObj_.rank]); ++wso) {
			if (wizardShotObjs[cObj_.rank][wso].attackState == AttackState::Non) {
				ShotObj_Initialize(wizardShotObjs[cObj_.rank][wso], 1, cObj_);
				break;
			}
		}
	}
	//--------------------------------------------------------------------------------------
	//敵がプレーヤーに移動する
	//--------------------------------------------------------------------------------------
	void EnemyWalkToTar_Determine(Object& obj_, Object& tarObj_) {
		float playerCenterX = tarObj_.x + tarObj_.w / 2.0f - camera.x;
		float playerCenterY = tarObj_.y + tarObj_.h / 2.0f - camera.y;
		float enemyCenterX = obj_.x + obj_.w / 2.0f - camera.x;
		float enemyCenterY = obj_.y + obj_.h / 2.0f - camera.y;
		float newTarX = 0.0f;
		float newTarY = 0.0f;
		//最初に敵を生成するときは、プレイヤーの最後の位置情報が存在しないため、新たに位置を生成する必要ある（生成された位置の座標はアンカーポイントとして記録され、敵の侵入は1回のみと判断される）
		if (obj_.lstTarX == 0.0f && obj_.lstTarY == 0.0f) {
			float* arr = EnemyTarCrd_GetResult(obj_, tarObj_);
			newTarX = arr[0];
			newTarY = arr[1];
			delete[] arr;
		}
		//プレイヤーが歩いているとき（攻撃しているかどうかは関係ない）
		if (tarObj_.walkCnt > 0) {
			//敵の最遠攻撃の2乗
			float maxRadius2Sec = obj_.attackMaxRadius * obj_.attackMaxRadius;
			//敵とプレーヤーX座標の2乗
			float spaceX2Sec = (enemyCenterX - playerCenterX) * (enemyCenterX - playerCenterX);
			//敵とプレーヤーY座標の2乗
			float spaceY2Sec = (enemyCenterY - playerCenterY) * (enemyCenterY - playerCenterY);
			//敵自身が攻撃目標位置に到達した（多少の誤差は許容される）
			if (ceil(obj_.speed * obj_.speed) >= (enemyCenterX - obj_.lstTarX) * (enemyCenterX - obj_.lstTarX) + (enemyCenterY - obj_.lstTarY) * (enemyCenterY - obj_.lstTarY)) {
				//敵と歩行プレイヤーとの距離が最遠攻撃範囲を超えると、敵は攻撃を停止し、新たな位置を生成して移動を開始する（生成された位置の座標はアンカーポイントとして記録される）
				if (maxRadius2Sec < spaceX2Sec + spaceY2Sec) {
					if (obj_.attackState == AttackState::Normal) obj_.attackState = AttackState::Non;
					obj_.charaState = CharaState::Walk;
					float* arr = EnemyTarCrd_GetResult(obj_, tarObj_);
					newTarX = arr[0];
					newTarY = arr[1];
					delete[] arr;
				}
				//敵と歩いているプレイヤーの間の距離はまだ最も遠い攻撃範囲内にあり、立ち続けて攻撃する
				else {
					obj_.charaState = CharaState::Stand;
					if (obj_.attackState == AttackState::Non) obj_.attackState = AttackState::Normal;
				}
			}
			//敵自体はまだ攻撃目標位置に到達しておらず（多少の誤差は許容される）、移動を続けている
			else {
				if (obj_.attackState == AttackState::Normal) obj_.attackState = AttackState::Non;
				obj_.charaState = CharaState::Walk;
				//敵と歩行プレイヤーとの距離が最遠攻撃範囲を超えると、敵は攻撃を停止し、新たな位置を生成して移動を開始する（生成された位置の座標はアンカーポイントとして記録される）
				if (maxRadius2Sec < spaceX2Sec + spaceY2Sec) {
					if (obj_.attackState == AttackState::Normal) obj_.attackState = AttackState::Non;
					obj_.charaState = CharaState::Walk;
					float* arr = EnemyTarCrd_GetResult(obj_, tarObj_);
					newTarX = arr[0];
					newTarY = arr[1];
					delete[] arr;
				}
				//敵の次のアンカーポイント座標と歩いているプレイヤーの間の距離はまだ最も遠い攻撃範囲内にあり、アンカーポイント座標に向かって移動し続ける
				else {
					newTarX = obj_.lstTarX;
					newTarY = obj_.lstTarY;
				}
			}
		}
		//プレイヤーが立っているとき（攻撃しているかどうかは関係ない）
		else {
			//敵は攻撃目標位置に到達し（多少の誤差は許容される）、立ち上がったまま攻撃する
			if (ceil(obj_.speed * obj_.speed) >=(enemyCenterX - obj_.lstTarX) * (enemyCenterX - obj_.lstTarX) + (enemyCenterY - obj_.lstTarY) * (enemyCenterY - obj_.lstTarY)){
				obj_.charaState = CharaState::Stand;
				if (obj_.attackState == AttackState::Non) obj_.attackState = AttackState::Normal;
			}
			//敵自体はまだ攻撃目標位置に到達しておらず（多少の誤差は許容される）、移動を続けている
			else {
				if (obj_.attackState == AttackState::Normal) obj_.attackState = AttackState::Non;
				obj_.charaState = CharaState::Walk;
				newTarX = obj_.lstTarX;
				newTarY = obj_.lstTarY;
			}
		}
		if (obj_.charaState == CharaState::Stand) {
			if (enemyCenterX == playerCenterX) obj_.charaFaceDirect = tarObj_.charaFaceDirect == CharaFaceDirect::Left ? CharaFaceDirect::Right : CharaFaceDirect::Left;
			else if (enemyCenterX > playerCenterX) obj_.charaFaceDirect = CharaFaceDirect::Left;
			else obj_.charaFaceDirect = CharaFaceDirect::Right;
		} else if (obj_.charaState == CharaState::Walk) {
			float xSpace = abs(newTarX - enemyCenterX);
			float ySpace = abs(newTarY - enemyCenterY);
			float len = sqrt(xSpace * xSpace + ySpace * ySpace);
			float xRealSpeed = 0.0f;
			float yRealSpeed = 0.0f;
			if (ceil(enemyCenterX) == ceil(newTarX)) {
				if (enemyCenterY == newTarY){
					obj_.charaState = CharaState::Stand;
					if (obj_.attackState == AttackState::Non) obj_.attackState = AttackState::Normal;
				} else if (enemyCenterY > newTarY) {
					obj_.charaMoveDirect = CharaMoveDirect::Up;
					yRealSpeed = obj_.speed;
				} else {
					obj_.charaMoveDirect = CharaMoveDirect::Down;
					yRealSpeed = obj_.speed;
				}
			} else if (ceil(enemyCenterX) > ceil(newTarX)) {
				obj_.charaFaceDirect = CharaFaceDirect::Left;
				if (enemyCenterY == newTarY) {
					obj_.charaMoveDirect = CharaMoveDirect::Left;
					xRealSpeed = obj_.speed;
				} else if (enemyCenterY > newTarY) {
					obj_.charaMoveDirect = CharaMoveDirect::UpLeft;
					xRealSpeed = xSpace * obj_.speed / len;
					yRealSpeed = ySpace * obj_.speed / len;
				} else {
					obj_.charaMoveDirect = CharaMoveDirect::DownLeft;
					xRealSpeed = xSpace * obj_.speed / len;
					yRealSpeed = ySpace * obj_.speed / len;
				}
			} else {
				obj_.charaFaceDirect = CharaFaceDirect::Right;
				if (enemyCenterY == newTarY) {
					obj_.charaMoveDirect = CharaMoveDirect::Right;
					xRealSpeed = obj_.speed;
				} else if (enemyCenterY > newTarY) {
					obj_.charaMoveDirect = CharaMoveDirect::UpRight;
					xRealSpeed = xSpace * obj_.speed / len;
					yRealSpeed = ySpace * obj_.speed / len;
				} else {
					obj_.charaMoveDirect = CharaMoveDirect::DownRight;
					xRealSpeed = xSpace * obj_.speed / len;
					yRealSpeed = ySpace * obj_.speed / len;
				}
			}
			CharaMove_Calculate_Absolute(obj_, xRealSpeed, yRealSpeed);
		}
	}
	//--------------------------------------------------------------------------------------
	//敵がプレーヤーに移動するターゲットの座標を計算する
	//--------------------------------------------------------------------------------------
	float* EnemyTarCrd_GetResult(Object& obj_, Object& tarObj_) {
		int tmpXMinOffset = (int)(obj_.attackMinRadius * 10);
		int tmpXMaxOffset = (int)(obj_.attackMaxRadius * 10);
		int tmpYMinOffset = (int)(obj_.attackMinRadius * 10);
		int tmpYMaxOffset = (int)(obj_.attackMaxRadius * 10);
		float playerCenterX = tarObj_.x + tarObj_.w / 2.0f - camera.x;
		float playerCenterY = tarObj_.y + tarObj_.h / 2.0f - camera.y;
		float enemyCenterX = obj_.x + obj_.w / 2.0f - camera.x;
		float enemyCenterY = obj_.y + obj_.h / 2.0f - camera.y;
		float newTarX = 0.0f;
		float newTarY = 0.0f;
		while (true) {
			int tarXOffsetInt = rand() % (tmpXMaxOffset - tmpXMinOffset + 1) + tmpXMinOffset;
			float tarXOffset = enemyCenterX >= playerCenterX ? tarXOffsetInt / 10.0f : (0 - tarXOffsetInt) / 10.0f;
			if (obj_.typeNum == 0) newTarX = rand() % 2 == 0 ? playerCenterX + tarXOffset : playerCenterX - tarXOffset;
			else newTarX = playerCenterX + tarXOffset;
			int tarYOffsetInt = rand() % (tmpYMaxOffset - tmpYMinOffset + 1) + tmpYMinOffset;
			float tarYOffset = enemyCenterY >= playerCenterY ? tarYOffsetInt / 10.0f : (0 - tarYOffsetInt) / 10.0f;
			if (obj_.typeNum == 0) newTarY = rand() % 2 == 0 ? playerCenterY + tarYOffset : playerCenterY - tarYOffset;
			else newTarY = playerCenterY + tarYOffset;
			//敵の最遠攻撃の2乗
			float maxRadius2Sec = obj_.attackMaxRadius * obj_.attackMaxRadius;
			//敵とプレーヤーX座標の2乗
			float spaceX2Sec = (newTarX - playerCenterX) * (newTarX - playerCenterX);
			//敵とプレーヤーY座標の2乗
			float spaceY2Sec = (newTarY - playerCenterY) * (newTarY - playerCenterY);
			if (maxRadius2Sec >= spaceX2Sec + spaceY2Sec) break;
		}
		//未解決：②敵はプレイヤーを攻撃できる範囲内のランダム位置に移動し、ゲームの限界を越えてしまう
		obj_.lstTarX = newTarX;
		obj_.lstTarY = newTarY;
		const float size = 2;
		float* arr = new float[2];
		arr[0] = newTarX;
		arr[1] = newTarY;
		return arr;
	}
	//--------------------------------------------------------------------------------------
	//絶対パスに移動する
	//--------------------------------------------------------------------------------------
	void CharaMove_Calculate_Absolute(Object& obj_, float xSpeed_, float ySpeed_) {
		switch (obj_.charaMoveDirect) {
		case CharaMoveDirect::Left:
			obj_.x -= xSpeed_;
			break;
		case CharaMoveDirect::UpLeft:
			obj_.x -= xSpeed_;
			obj_.y -= ySpeed_;
			break;
		case CharaMoveDirect::Up:
			obj_.y -= ySpeed_;
			break;
		case CharaMoveDirect::UpRight:
			obj_.x += xSpeed_;
			obj_.y -= ySpeed_;
			break;
		case CharaMoveDirect::Right:
			obj_.x += xSpeed_;
			break;
		case CharaMoveDirect::DownRight:
			obj_.x += xSpeed_;
			obj_.y += ySpeed_;
			break;
		case CharaMoveDirect::Down:
			obj_.y += ySpeed_;
			break;
		case CharaMoveDirect::DownLeft:
			obj_.x -= xSpeed_;
			obj_.y += ySpeed_;
			break;
		}
	}
	//--------------------------------------------------------------------------------------
	//相対パスに移動する
	//--------------------------------------------------------------------------------------
	void CharaMove_Calculate_Relative(Object& obj_, Object& cObj_, float xSpeed_, float ySpeed_) {
		switch (obj_.charaMoveDirect) {
			case CharaMoveDirect::Left:
				cObj_.x += xSpeed_;
				break;
			case CharaMoveDirect::UpLeft:
				cObj_.x += xSpeed_;
				cObj_.y += ySpeed_;
				break;
			case CharaMoveDirect::Up:
				cObj_.y += ySpeed_;
				break;
			case CharaMoveDirect::UpRight:
				cObj_.x -= xSpeed_;
				cObj_.y += ySpeed_;
				break;
			case CharaMoveDirect::Right:
				cObj_.x -= xSpeed_;
				break;
			case CharaMoveDirect::DownRight:
				cObj_.x -= xSpeed_;
				cObj_.y -= ySpeed_;
				break;
			case CharaMoveDirect::Down:
				cObj_.y -= ySpeed_;
				break;
			case CharaMoveDirect::DownLeft:
				cObj_.x += xSpeed_;
				cObj_.y -= ySpeed_;
				break;
		}
	}
	//--------------------------------------------------------------------------------------
	//敵のダメージ計算
	//--------------------------------------------------------------------------------------
	void EnemyDamage_Calculate(Object& obj_, bool isAbs_) {
		if (obj_.attackState == AttackState::Normal) {
			if (playerObjs[playerType].charaState != CharaState::Non) {
				float nowCharaCenterX = isAbs_ ? obj_.x + obj_.w / 2.0f : obj_.x;
				float nowCharaCenterY = isAbs_ ? obj_.y + obj_.h / 2.0f : obj_.y;
				float playerCenterX = playerObjs[playerType].x + playerObjs[playerType].w / 2.0f;
				float playerCenterY = playerObjs[playerType].y + playerObjs[playerType].h / 2.0f;
				float radiusSum = (obj_.attackMaxRadius + playerObjs[playerType].injuryRadius) * (obj_.attackMaxRadius + playerObjs[playerType].injuryRadius);
				float xSpace = (nowCharaCenterX - playerCenterX) * (nowCharaCenterX - playerCenterX);
				float ySpace = (nowCharaCenterY - playerCenterY) * (nowCharaCenterY - playerCenterY);
				if (radiusSum >= xSpace + ySpace) {
					if (playerObjs[playerType].healthNow > 0) {
						int tmpDamageMin = (int)(obj_.damageMin * 10);
						int tmpDamageMax = (int)(obj_.damageMax * 10);
						int randⅮamage = rand() % (tmpDamageMax - tmpDamageMin + 1) + tmpDamageMin;
						float damageNow = randⅮamage / 10.0f;
						playerObjs[playerType].healthNow -= damageNow;
						if (playerObjs[playerType].healthNow <= 0) playerObjs[playerType].charaState = CharaState::Dead;
					} else playerObjs[playerType].charaState = CharaState::Dead;
					obj_.attackState = AttackState::Non;
				}
			}
		}
	}
	//--------------------------------------------------------------------------------------
	//プレーヤーのダメージ計算
	//--------------------------------------------------------------------------------------
	void PlayerDamage_Calculate(Object& obj_, bool isAbs_) {
		if (obj_.attackState == AttackState::Normal) {
			for (int eo = 0; eo < _countof(enemyObjs); ++eo) {
				if (enemyObjs[eo].charaState != CharaState::Non) {
					float nowCharaCenterX = isAbs_ ? obj_.x + obj_.w / 2.0f : obj_.x;
					float nowCharaCenterY = isAbs_ ? obj_.y + obj_.h / 2.0f : obj_.y;
					float enemyCenterX = enemyObjs[eo].x + enemyObjs[eo].w / 2.0f;
					float enemyCenterY = enemyObjs[eo].y + enemyObjs[eo].h / 2.0f;
					float radiusSum = (obj_.attackMaxRadius + enemyObjs[eo].injuryRadius) * (obj_.attackMaxRadius + enemyObjs[eo].injuryRadius);
					float xSpace = (nowCharaCenterX - enemyCenterX) * (nowCharaCenterX - enemyCenterX);
					float ySpace = (nowCharaCenterY - enemyCenterY) * (nowCharaCenterY - enemyCenterY);
					if (radiusSum >= xSpace + ySpace) {
						if (enemyObjs[eo].healthNow > 0) {
							int tmpDamageMin = (int)(obj_.damageMin * 10);
							int tmpDamageMax = (int)(obj_.damageMax * 10);
							int randⅮamage = rand() % (tmpDamageMax - tmpDamageMin + 1) + tmpDamageMin;
							float damageNow = randⅮamage / 10.0f;
							enemyObjs[eo].healthNow -= damageNow;
							if (enemyObjs[eo].healthNow <= 0) {
								enemyObjs[eo].charaState = CharaState::Dead;
								if (enemyObjs[eo].typeNum == 0) deadSoldierNum++;
								else deadWizardNum++;
							}
						} else enemyObjs[eo].charaState = CharaState::Dead;
						obj_.attackState = AttackState::Non;
					}
				}
			}
		}
	}
	//--------------------------------------------------------------------------------------
	//初期化処理
	//機能概要：プログラム起動時に1回実行される（素材などの準備を行う）
	//--------------------------------------------------------------------------------------
	void Initialize() {
		srand((unsigned int)time(NULL));
		fontTest = DG::Font::Create("HGS教科書", 5, 10);
		//キーボードの入力を受け取るオブジェクトを生成する
		//アナログスティック
		XI::AnalogAxisKB ls = { DIK_LEFT, DIK_RIGHT, DIK_UP, DIK_DOWN };
		XI::AnalogAxisKB rs = { DIK_NUMPAD4, DIK_NUMPAD6, DIK_NUMPAD8, DIK_NUMPAD2 };
		//トリガー
		XI::AnalogTriggerKB tg = { DIK_E, DIK_R };
		//十字キーとボタン
		XI::KeyDatas_KB key = {
			{ DIK_Z, XI::VGP::B1 }, { DIK_X, XI::VGP::B2 },
			{ DIK_C, XI::VGP::B3 }, { DIK_V, XI::VGP::B4 },
			{ DIK_A, XI::VGP::SE }, { DIK_S, XI::VGP::ST },
			{ DIK_Q, XI::VGP::L1 }, { DIK_W, XI::VGP::R1 },
			{ DIK_D, XI::VGP::L3 }, { DIK_NUMPAD5, XI::VGP::R3 },
			{ DIK_UP, XI::VGP::HU }, { DIK_DOWN, XI::VGP::HD },
			{ DIK_LEFT, XI::VGP::HL }, { DIK_RIGHT, XI::VGP::HR }
		};
		gamePadIn = XI::GamePad::CreateKB(ls, rs, tg, key);

		enemyImg0 = DG::Image::Create("./data/image/Chara/Knight/Knight/Knight.png");
		enemyImg1 = DG::Image::Create("./data/image/Chara/Wizard/Wizard/Wizard.png");
		playerImg0 = DG::Image::Create("./data/image/Chara/Skeleton/Skeleton/Skeleton.png");
		playerImg1 = DG::Image::Create("./data/image/Chara/Slime/Slime/Slime.png");
		shotImg0 = DG::Image::Create("./data/image/Chara/Slime/Slime/Slime-Shot.png");
		shotImg1 = DG::Image::Create("./data/image/Chara/Wizard/Wizard/Wizard-Shot.png");
		healthImg0 = DG::Image::Create("./data/image/Health Bar_green.png");
		healthImg1 = DG::Image::Create("./data/image/Health Bar_red.png");
		bg_black = DG::Image::Create("./data/image/Black.png");
		bg_wall = DG::Image::Create("./data/image/Tileset.png");
		bg_grass = DG::Image::Create("./data/image/Grass.png");
		fb_Eletric = DG::Image::Create("./data/image/Eletric.png");

		playerHit0 = DM::Sound::CreateStream("./data/sound/Attack.wav");
		playerHit0->SetVolume(ge->ConvertVolume(100));
		playerHit1 = DM::Sound::CreateStream("./data/sound/Jump.wav");
		playerHit1->SetVolume(ge->ConvertVolume(100));
		playerMage = DM::Sound::CreateStream("./data/sound/Poison.wav");
		playerMage->SetVolume(ge->ConvertVolume(100));

		Camera_Initialize(camera, blockRow, blockCol, cameraX, cameraY);
		playerType = 0;
		PlayerObj_Initialize(playerObjs[playerType], playerType, playerType, (windowW - 100) / 2.0f, (windowH - 100) / 2.0f);
		//とりあえずこのように敵を生成する
		int soldierRank = 0;
		int wizardRank = 0;
		for (int ei = 0; ei < _countof(enemyObjs); ++ei) {
			int randEnemyXInt = rand() % (windowW - 15 + 1) + 15;
			float randEnemyXFloat = randEnemyXInt * 1.0f;
			int randEnemyYInt = rand() % (windowH - 15 + 1) + 15;
			float randEnemyYFloat = randEnemyYInt * 1.0f;
			if (ei < soldierNum) {
				EnemyObj_Initialize(enemyObjs[ei], ei, soldierRank, 0, randEnemyXFloat, randEnemyYFloat);
				soldierRank++;
			} else {
				EnemyObj_Initialize(enemyObjs[ei], ei, wizardRank, 1, randEnemyXFloat, randEnemyYFloat);
				wizardRank++;
			}
		}
		for (int sso = 0; sso < _countof(slimeShotObjs); ++sso) {
			slimeShotObjs[sso].attackState = AttackState::Non;
		}
		for (int wsox = 0; wsox < wizardNum; ++wsox) {
			for (int wsoy = 0; wsoy < _countof(wizardShotObjs[wsox]); ++wsoy) {
				wizardShotObjs[wsox][wsoy].attackState = AttackState::Non;
			}
		}
	}
	//--------------------------------------------------------------------------------------
	//更新処理
	//機能概要：ゲームの１フレームに当たる処理
	//--------------------------------------------------------------------------------------
	TaskFlag UpDate() {
		auto gpi = gamePadIn->GetState();
		//Qキーを押すとスケルトンとスライムが切り替わる
		if (gpi.L1.down) {
			if(playerObjs[playerType].charaState != CharaState::Non && playerObjs[playerType].charaState != CharaState::Dead){
				playerHealthObjs[playerType].displayHealth = false;
				float lstPosX = playerObjs[playerType].x;
				float lstPosY = playerObjs[playerType].y;
				int lstPlayerType = playerType;
				playerType = playerType == 0 ? 1 : 0;
				PlayerObj_Initialize(playerObjs[playerType], playerType, playerType, lstPosX, lstPosY);
				//スケルトンとスライムはとりあえずヘルスを共有する
				playerObjs[playerType].healthNow = playerObjs[lstPlayerType].healthNow;
			}
		}
		PlayerObj_UpDate(playerObjs[playerType]);
		for (int sso = 0; sso < _countof(slimeShotObjs); ++sso) {
			ShotObj_UpDate(slimeShotObjs[sso]);
		}
		for (int pho = 0; pho < _countof(playerHealthObjs); ++pho) {
			HealthObj_UpDate(playerHealthObjs[pho]);
		}
		for (int eo = 0; eo < _countof(enemyObjs); ++eo) {
			EnemyObj_UpDate(enemyObjs[eo]);
		}
		for (int eho = 0; eho < _countof(enemyHealthObjs); ++eho) {
			HealthObj_UpDate(enemyHealthObjs[eho]);
		}
		for (int wsox = 0; wsox < wizardNum; ++wsox) {
			for (int wsoy = 0; wsoy < _countof(wizardShotObjs[wsox]); ++wsoy) {
				ShotObj_UpDate(wizardShotObjs[wsox][wsoy]);
			}
		}

		timeCnt++;
		return TaskFlag::Game;
	}
	//--------------------------------------------------------------------------------------
	//描画処理
	//機能概要：ゲームの１フレームに当たる表示処理
	//--------------------------------------------------------------------------------------
	void Render() {
		//背景色
		ML::Box2D drawBack(0, 0, windowW, windowH);
		ML::Box2D srcBack(0, 0, 1920, 1080);
		bg_black->Draw(drawBack, srcBack);

		Camera_Draw(camera);
		for (int eho = 0; eho < _countof(enemyHealthObjs); ++eho) {
			HealthObj_Draw(enemyHealthObjs[eho]);
		}
		for (int eo = 0; eo < _countof(enemyObjs); ++eo) {
			EnemyObj_Draw(enemyObjs[eo]);
		}
		for (int pho = 0; pho < _countof(playerHealthObjs); ++pho) {
			HealthObj_Draw(playerHealthObjs[pho]);
		}
		for (int wsox = 0; wsox < wizardNum; ++wsox) {
			for (int wsoy = 0; wsoy < _countof(wizardShotObjs[wsox]); ++wsoy) {
				ShotObj_Draw(wizardShotObjs[wsox][wsoy]);
			}
		}
		PlayerObj_Draw(playerObjs[playerType]);
		for (int sso = 0; sso < _countof(slimeShotObjs); ++sso) {
			ShotObj_Draw(slimeShotObjs[sso]);
		}

		ML::Box2D textBox(2, 2, 450, 250);
		string text = "HEALTH : [ " + to_string((int) (playerObjs[playerType].healthNow < 0.0f ? 0.0f : playerObjs[playerType].healthNow)) + " ]  \nSwordsmen killed : " + to_string(deadSoldierNum) + "\nMages killed : " + to_string(deadWizardNum);
		fontTest->Draw(textBox, text, ML::Color(0.8f, 0.5f, 0.0f, 0.2f));		
	}
	//--------------------------------------------------------------------------------------
	//解放処理
	//--------------------------------------------------------------------------------------
	void Finalize() {
		enemyImg0.reset();
		enemyImg1.reset();
		playerImg0.reset();
		playerImg1.reset();
		shotImg0.reset();
		shotImg1.reset();
		healthImg0.reset();
		healthImg1.reset();
		bg_black.reset();
		bg_wall.reset();
		bg_grass.reset();
		fb_Eletric.reset();
		playerHit0.reset();
		playerHit1.reset();
		playerMage.reset();
		gamePadIn.reset();
		fontTest.reset();
	}
}