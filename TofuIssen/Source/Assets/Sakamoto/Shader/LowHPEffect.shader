Shader "Custom/LowHPEffect"
{
    Properties
    {
        //ShaderLabの構造を定義する部分は
        //セミコロンが不要

        //テクスチャを定義
        _MainTex("Base Texture",2D) = "white" {}
        //エフェクトをどれぐらい強くかけるか
        _intensity("Effect Intensity",Range(0,1)) = 0
        //Vignetteの強さ(intensityは全てのエフェクトの強さを表すが、
        //vignetteStrはVignetteの強さを表す))
        _vignStr("Vignette Strength",Range(0,1)) = 0.7
        //血の量
        _bloodAmt("Blood Amount",Range(0,1)) = 0.5
        //彩度の低下
        _desat("Desaturation",Range(0,1)) = 0.8
        //脈動の速さ
        _pulseSpeed("Pulse Speed",Range(0,8)) = 2.0
    }
    SubShader
    {
        //描画順を指定
        //今回は透明オブジェクトの描画順を指定するために
        //Transparentを指定
        Tags { "Queue" = "Transparent" }

        Pass
        {
            CGPROGRAM
            //シェーダーの種類を指定
            //頂点シェーダーとフラグメントシェーダーの関数名を指定する
            #pragma vertex   vert
            #pragma fragment frag

            //Unityのシェーダーでよく使われる関数や
            //定数が定義されているファイルをインクルード
            #include "UnityCG.cginc"
            
            //Unityエディタ上からプロパティを受け取るための変数
            sampler2D _MainTex;
            float _intensity;
            float _vignStr;
            float _bloodAmt;
            float _desat;
            float _pulseSpeed;

            //頂点データの構造体
            struct appdata
            {
                float4 vertex : POSITION; //頂点の位置
                float2 uv : TEXCOORD0; //テクスチャ座標
            };

            //頂点シェーダからフラグメントシェーダに渡す構造体
            struct v2f
            {
                float2 uv : TEXCOORD0; //テクスチャ座標
                float4 pos : SV_POSITION; //スクリーン上の位置
            };

            //頂点シェーダーの定義
            v2f vert(appdata v)
            {
                v2f o;
                o.pos = UnityObjectToClipPos(v.vertex); //頂点の位置をスクリーン上の位置に変換
                o.uv = v.uv; //テクスチャ座標を渡す
                return o;
            }

            //fixed4について
            //fixed4はRGBAの4つの数値をまとめた型
            //色を返すのでこれを使う

            //フラグメント関数(ピクセルシェーダー)の定義
            //v2fは頂点シェーダーからフラグメントシェーダーに渡す構造体
            //SV_Targetが、シェーダーならではの部分で
            //「セマンティクス」と呼ばれる
            //この関数の戻り値を画面(レンダーターゲット)に出力する
            //という宣言になる。GPUに対して、
            //この値はどこへ送るかを教えるための仕組み
            fixed4 frag(v2f i) : SV_Target
            {
                //画面上のUV座標を取得
                float2 uv = i.uv;

                //元の画像を取得
                fixed4 col = tex2D(_MainTex, uv);
                
                //彩度低下(グレースケールに近づける)
                //float3の値はRGBの重みで、これを使って輝度を計算する
                //0.299, 0.587, 0.114は人間の目が感じる色の明るさの重み
                //それとcol.rgbの内積を取ることで、元の色をグレースケールに変換する
                //lum(luminance)は、日本語だと「輝度」と呼ばれることが多い
                //つまり明るさを表す値
                float lum = dot(col.rgb,float3(0.299, 0.587, 0.114));
                //元の色とグレーの値を線形補間する
                //これをすることで元の色がグレー寄りの色になる
                col.rgb = lerp(col.rgb,lum,_desat * _intensity);

                //ビネット(画面の周辺が暗くなる効果)
                //中心を原点とした距離を計算するために、UV座標から0.5を引いて中心を(0,0)にする
                float2 center = uv - 0.5; //画面の中心からの距離を計算
                float vign = dot(center,center); //距離の二乗を計算
                //距離が遠いほどvignは大きくなる
                //*3.0はビネットの落ち具合を強調するための定数
                //これが大きいほど、画面の周辺が急激に暗くなる
                col.rgb *= 1.0 - vign * _vignStr * _intensity * 3.0;

                //血のにじみ(端に赤を加算する)
                float edge = saturate(vign * 2.0); //距離が遠いほどedgeは大きくなる
                //カラーを赤に近づけるために、元の色と赤(0.5,0,0)を線形補間する
                col.rgb = lerp(col.rgb,float3(0.5,0,0),edge * _bloodAmt * _intensity);

                return col;
            }

            ENDCG
        }
    }
}
