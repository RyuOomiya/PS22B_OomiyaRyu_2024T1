# include <Siv3D.hpp>

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

	void Draw()
	{
		ball.draw();
	}

	void Update()
	{
		ball.moveBy(velocity * Scene::DeltaTime());
	}

	Ball() : velocity(0, -constants::ball::SPEED), ball(400, 400, 8) {}
};

class Bricks {
public:
	Rect bricks[constants::brick::MAX];
	int refbricksCount = 0;

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
		refbricksCount = 0;
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
					refbricksCount++;
				}
				else // ブロックの左辺または右辺と交差
				{
					ball.velocity.x *= -1;
					refbricksCount++;
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

	Paddle() : paddle(Arg::center(Cursor::Pos().x, 500), 60, 10) {}

	void Draw()
	{
		paddle.rounded(3).draw();
	}


	void Update()
	{
		paddle.x = Cursor::Pos().x - (constants::paddle::SIZE.x / 2);
	}

	void Intersects(Ball& ball) {
		if ((0 < ball.velocity.y) && paddle.intersects(ball.ball))
		{
			ball.velocity = Vec2{
				(ball.ball.x - paddle.center().x) * 10,
				-ball.velocity.y
			}.setLength(constants::ball::SPEED);
		}
	}
};

struct Wall {
	/// @brief 衝突検知
	static void Intersects(Ball& ball) {
		// 天井との衝突を検知
		if ((ball.ball.y < 0) && (ball.velocity.y < 0))
		{
			ball.velocity.y *= -1;
		}

		// 壁との衝突を検知
		if (((ball.ball.x < 0) && (ball.velocity.x < 0))
			|| ((Scene::Width() < ball.ball.x) && (0 < ball.velocity.x)))
		{
			ball.velocity.x *= -1;
		}
	}
};

class Manager {
public:
	bool gameStart = false;
	Font font{ FontMethod::MSDF, 48 };

	void Update()
	{
		if (!gameStart)
		{
			font(U"スペースキーを押してスタート").drawAt(Scene::Center(), Palette::White);
			if (KeySpace.pressed())
			{
				gameStart = true;
			}
		}
	}

	void EndChecker(Ball& ball, Bricks& bricks)
	{
		//ボールの落下を検知
		if ((Scene::Height() < ball.ball.y) && (0 < ball.velocity.y))
		{
			gameStart = false;
		}

		//ブロック全破壊を検知
		if (100 <= bricks.refbricksCount)
		{
			gameStart = false;
		}
	}
};

void Main()
{
	Manager manager;
	Ball ball;
	Bricks bricks;
	Paddle paddle;

#pragma endregion
	while (System::Update())
	{
		manager.Update();

		if (manager.gameStart)
		{
			ball.Update();
			paddle.Update();

			ball.Draw();
			bricks.Draw();
			paddle.Draw();

			bricks.Intersects(ball);
			paddle.Intersects(ball);
			Wall::Intersects(ball);
			manager.EndChecker(ball, bricks);
		}
		if (manager.gameStart == false)
		{
			ball = Ball();
			bricks = Bricks();
			paddle = Paddle();
		}
	}
}
