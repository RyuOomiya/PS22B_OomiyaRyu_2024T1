# include <Siv3D.hpp>

/*
	古き良き書き方での実装
	・安全性や利便性などは一切考えていない
*/

namespace constants {
	namespace brick {
		/// @brief ブロックのサイズ
		constexpr Size SIZE{ 40, 20 };

		/// @brief ブロックの数　縦
		constexpr int Y_COUNT = 5;

		/// @brief ブロックの数　横
		constexpr int X_COUNT = 20;

		/// @brief 合計ブロック数
		constexpr int MAX = Y_COUNT * X_COUNT;
	}
	namespace ball {
		/// @brief ボールの速さ
		constexpr double SPEED = 480.0;
	}
	namespace paddle {
		constexpr Size SIZE{ 60, 10 };
	}
}

class Ball {
public:
	Vec2 velocity;
	Circle ball;

	void Draw() {
		ball.draw();
	}

	void Update()
	{
		ball.moveBy(velocity * Scene::DeltaTime());
	}

	Ball() : velocity(0, constants::ball::SPEED), ball(400, 400, 8) {}
};

class Bricks {
public:
	Rect bricks[constants::brick::MAX];

	Bricks() {
		using namespace constants::brick;
		for (int y = 0; y < Y_COUNT; ++y) {
			for (int x = 0; x < X_COUNT; ++x) {
				int index = y * X_COUNT + x;
				bricks[index] = Rect{
					x * SIZE.x,
					60 + y * SIZE.y,
					SIZE
				};
			}
		}
	}

	 void Draw() {
		 for (int i = 0; i < constants::brick::MAX; ++i) {
			 bricks[i].stretched(-1).draw(HSV{ bricks[i].y - 40 });
		 }
	}

	 void Intersects(Ball& ball) {
		 using namespace constants::brick;

		 for (int i = 0; i < MAX; ++i) {
			// 参照で保持
			Rect& refBrick = bricks[i];

			// 衝突を検知
			if (refBrick.intersects(ball.ball))
			{
				// ブロックの上辺、または底辺と交差
				if (refBrick.bottom().intersects(ball.ball) || refBrick.top().intersects(ball.ball))
				{
					ball.velocity.y *= -1;
				}
				else // ブロックの左辺または右辺と交差
				{
					ball.velocity.x *= -1;
				}

				// あたったブロックは画面外に出す
				refBrick.y -= 600;

				// 同一フレームでは複数のブロック衝突を検知しない
				break;
			}
		}
	 }
};

class Paddle {
public:
	Rect paddle;

	Paddle() : paddle(Arg::center(Cursor::Pos().x, 500), 60, 10){}

	void Draw()
	{
		paddle.rounded(3).draw();
	}
	void Update()
	{
		paddle.x = Cursor::Pos().x - (constants::paddle::SIZE.x / 2);
	}
};

void Main()
{
	Ball ball;
	Bricks bricks;
	Paddle paddle;
#pragma endregion

	while (System::Update())
	{
		ball.Update();
		paddle.Update();

		ball.Draw();
		bricks.Draw();
		paddle.Draw();

		bricks.Intersects(ball);
		
		////==============================
		//// コリジョン
		////==============================
		//// ブロックとの衝突を検知
		//for (int i = 0; i < MAX; ++i) {
		//	// 参照で保持
		//	Rect& refBrick = bricks[i];

		//	// 衝突を検知
		//	if (refBrick.intersects(ball))
		//	{
		//		// ブロックの上辺、または底辺と交差
		//		if (refBrick.bottom().intersects(ball) || refBrick.top().intersects(ball))
		//		{
		//			ballVelocity.y *= -1;
		//		}
		//		else // ブロックの左辺または右辺と交差
		//		{
		//			ballVelocity.x *= -1;
		//		}

		//		// あたったブロックは画面外に出す
		//		refBrick.y -= 600;

		//		// 同一フレームでは複数のブロック衝突を検知しない
		//		break;
		//	}
		//}

		//// 天井との衝突を検知
		//if ((ball.y < 0) && (ballVelocity.y < 0))
		//{
		//	ballVelocity.y *= -1;
		//}

		//// 壁との衝突を検知
		//if (((ball.x < 0) && (ballVelocity.x < 0))
		//	|| ((Scene::Width() < ball.x) && (0 < ballVelocity.x)))
		//{
		//	ballVelocity.x *= -1;
		//}

		//// パドルとの衝突を検知
		//if ((0 < ballVelocity.y) && paddle.intersects(ball))
		//{
		//	ballVelocity = Vec2{
		//		(ball.x - paddle.center().x) * 10,
		//		-ballVelocity.y
		//	}.setLength(BALL_SPEED);
		//}
	}
}
