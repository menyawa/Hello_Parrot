# include <Siv3D.hpp> // OpenSiv3D v0.4.3

// アニメーション描画用のクラス(構造体)
struct AnimationTexture {
    Array<Texture> textures;

    // フレームの時間
    Array<int32> delays;

    int32 duration = 0;

    explicit operator bool() const noexcept {
        return !textures.isEmpty();
    }

    Size size() const noexcept {
        if (!textures) {
            return Size(0, 0);
        }

        return textures.front().size();
    }

    size_t frames() const noexcept {
        return textures.size();
    }

    size_t getFrameIndex(int32 timeMillisec) const noexcept {
        return AnimatedGIFReader::MillisecToIndex(timeMillisec, delays, duration);
    }

    const Texture& getTexture(int32 timeMillisec) const noexcept {
        return textures[getFrameIndex(timeMillisec)];
    }
};


void Main() {
    //GIFを読み込み、コマごとに分解する？(ほぼコピペ)
    AnimationTexture nomalParrot;
    {
        // GIF ファイルを開く
        const AnimatedGIFReader gif(U"./Resorce/Image/60fpsparrot.gif");

        if (!gif) {
            throw Error(U"Failed to open a gif file");
        }

        Array<Image> images;

        // GIF アニメーションを読み込み
        if (gif.read(images, nomalParrot.delays, nomalParrot.duration)) {
            // Image を Texture に変換
            nomalParrot.textures = images.map([](const Image& i) { return Texture(i); });
        } else {
            throw Error(U"Failed to load a gif animation");
        }
    }
    AnimationTexture exceptionallyFastParrot;
    {
        // GIF ファイルを開く
        const AnimatedGIFReader gif(U"./Resorce/Image/exceptionallyfastparrot.gif");

        if (!gif) {
            throw Error(U"Failed to open a gif file");
        }

        Array<Image> images;

        // GIF アニメーションを読み込み
        if (gif.read(images, exceptionallyFastParrot.delays, exceptionallyFastParrot.duration)) {
            // Image を Texture に変換
            exceptionallyFastParrot.textures = images.map([](const Image& i) { return Texture(i); });
        } else {
            throw Error(U"Failed to load a gif animation");
        }
    }

	//背景を白色に
	Scene::SetBackground(ColorF(1.0, 1.0, 1.0));
	// 大きさ 60 のフォントを用意
	const Font font(60);

	// parrotの座標、移動半径、x, yの差分等
	Vec2 angleParrotPos(Scene::Center());
    const int radius = 20000;
    double delta = 0;
    double deltaDelta = 1.0;
    double angle = Math::Pi * delta / 180;
    double cosDelta = Math::Sqrt(radius) * std::cos(angle);
    double sinDelta = Math::Sqrt(radius) * std::sin(angle);
    Vec2 nomalParrotPos1(angleParrotPos.x - cosDelta, angleParrotPos.y - sinDelta);
	Vec2 nomalParrotPos2(angleParrotPos.x + cosDelta, angleParrotPos.y + sinDelta);
    
    //Sキーを押したらスタート
    while (System::Update()) {
        font(U"Sキーを押してスタート").drawAt(Scene::Center(), Palette::Black);
        if (KeyS.pressed()) break;
    } 

	while (System::Update()) {
		font(U"Hello Parrot!").drawAt(Scene::Center().x, Scene::Center().y - 200, Palette::Black);
        const int32 timeMillisec = static_cast<int32>(Scene::Time() * 1000);

        //parrotを全て描画
        nomalParrot.getTexture(timeMillisec).drawAt(nomalParrotPos1);
        exceptionallyFastParrot.getTexture(timeMillisec).drawAt(angleParrotPos);
        nomalParrot.getTexture(timeMillisec).drawAt(nomalParrotPos2);

        delta += deltaDelta;
        //角度が2π以上になっていたら直す
        if (delta >= 360.0) {
            //一度直すだけだと加速するにつれdeltaDeltaが大きくなりdeltaが360以下にならない場合が出てきて、消え失せる
            //while文で360度以下まで直そうとしたが、それだと最後まで見えはするがフリーズする
            //前者の方が面白いなって思ったので前者に
            //while (delta >= 360.0) delta -= 360.0;
            delta -= 360.0;
            deltaDelta *= 2; //一周毎に加速する
        } 
        //差分で角度、座標を更新する
        angle = Math::Pi * delta / 180;
        cosDelta = Math::Sqrt(radius) * std::cos(angle);
        sinDelta = Math::Sqrt(radius) * std::sin(angle);
        nomalParrotPos1 = Vec2(angleParrotPos.x - cosDelta, angleParrotPos.y - sinDelta);
        nomalParrotPos2 = Vec2(angleParrotPos.x + cosDelta, angleParrotPos.y + sinDelta);
	}
}
