Shader "Custom/ShockwaveShaderSample"
{
    Properties
    {
        //Unityエディタからシェーダーに値を渡すためのプロパティを定義する部分
        _DistStrength("歪みの強さ",Range(0,1)) = 0.1
        _Alpha("透明度",Range(0,1)) = 1.0
    }

    SubShader
    {
        //先に不透明のオブジェクトを描画させてから描画するため、透明オブジェクトの描画順を指定
        Tags { "RenderType"="Transparent" "Queue"="Transparent" }
        
        //GrabPassは今の画面を丸ごとコピーしてテクスチャとして使えるようにする機能
        GrabPass{"_GrabTexture"}

        //実際に1回描画する処理のかたまり
        Pass
        {
            //C言語のような構文でシェーダーを書く部分
            CGPROGRAM
            //シェーダの関数名を指定
            #pragma vertex vert
            #pragma fragment frag
            // make fog work
            #pragma multi_compile_fog
            //Unityのシェーダーでよく使われる関数や定数が定義されているファイルをインクルード
            #include "UnityCG.cginc"

            sampler2D _GrabTexture;//GrabPassでコピーしたテクスチャ
            float _DistStrength;//歪みの強さ
            float _Alpha;  //透明度

            //CPUからシェーダーに値を渡すための変数
            struct appdata
            {
                float4 vertex : POSITION;
                float2 uv : TEXCOORD0;
            };

            //頂点シェーダで決めた、場所などをフラグメントシェーダに渡すための構造体
            struct v2f
            {
                float4 vertex : SV_POSITION;//スクリーン上の位置
                float4 grabPos : TEXCOORD0;//GrabPassでコピーしたテクスチャのどこをサンプリングするか
                float2 uv : TEXCOORD1;//uv座標
                
                
            };

            //どこにどのように歪みをかけるかを計算する頂点シェーダー
            //vertex to fragmentをもじった名前
            v2f vert (appdata v)
            {
                v2f output;
                //3D空間の頂点の位置をスクリーン座標に変換している
                output.vertex = UnityObjectToClipPos(v.vertex);
                //そのスクリーン座標を使って、GrabTextureのどこを読めばいいかの座標に変換している
                output.grabPos = ComputeGrabScreenPos(output.vertex);
                output.uv = v.uv;
                return output;
            }

            //頂点シェーダで決めた場所に、どのような色を表示するかを決めるフラグメントシェーダー
            fixed4 frag (v2f i) : SV_Target
            {
                //このピクセルがリングの中心からどの方向にあるか
                float2 dir = i.uv - float2(0.5,0.5);
                //i.uv.x * 2.0 - 1.0をするのはUV.xがそのままだと
                //リングメッシュは内側の縁がUV.x = 0.0,リングの中央がUV.x = 0.5,
                //外側の縁がUV.x = 1.0になっている
                //リングの中央が一番歪み最大にしたいのでUV.xが0.5の時は
                //edge値が1になるようにしたい
                //リングの内側、外側に近いときはedge値は0になるようにしたい
                float edge = 1.0 - abs(i.uv.x * 2.0 - 1.0);
                //実際にずらす量を計算
                //dir = どの方向に
                //_DistStrength = どのくらいの強さで
                //リングの端は弱くなるように
                float2 offset = dir * _DistStrength * edge;
                //ここで「ずらし」を実行する
                //tex2Dprojはtex2Dの中まで、grabPosのようなfloat4の
                //座標から色を取るときに使用する
                //tex2Dproj(サンプラーの状態,テクスチャ座標）
                float4 grab = 
                tex2Dproj(
                    _GrabTexture,//サンプラーの状態
                    float4(i.grabPos.xy + offset * i.grabPos.w,//この部分でオフセットを計算i.grabPos.wをかけているのは座標の計算上の補正
                        i.grabPos.z,i.grabPos.w)
                        );

                //αを設定
                grab.a = _Alpha;
                return grab;
            }
            ENDCG
        }
    }
}
