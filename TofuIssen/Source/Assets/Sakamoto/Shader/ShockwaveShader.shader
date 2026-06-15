Shader "Custom/ShockwaveShader"
{
    Properties
    {
        _MainTex ("Texture", 2D) = "white" {}
        _Center("衝撃波の中心座標",Vector) = (0.0,0.0,0.0,0.0)
        _DistortionStrength("歪みの強さ",float) = 0.0
        _Radius("リングの現在の半径",float) = 0.0
        _Width("リングの幅",float) = 0.0
    }
    SubShader
    {
        Cull Off
        ZWrite Off
        ZTest Always

        Pass
        {
            CGPROGRAM
            #pragma vertex vert
            #pragma fragment frag
            // make fog work
            #pragma multi_compile_fog

            #include "UnityCG.cginc"

            sampler2D _MainTex;
            //アスペクト比を取得するために以下を宣言
            float4 _MainTex_TexelSize;
            float4 _Center;//C#側から値をもらうときはVectorだがシェーダ内ではfloat4になる
            float _DistortionStrength;
            float _Radius;
            float _Width;

            struct appdata
            {
                float4 vertex : POSITION;
                float2 uv : TEXCOORD0;
            };

            struct v2f
            {
                float2 uv : TEXCOORD0;
                float4 vertex : SV_POSITION;
            };

            v2f vert (appdata v)
            {
                v2f output;
                //3D空間の頂点の位置をスクリーン座標に変換している
                output.vertex = UnityObjectToClipPos(v.vertex);
                output.uv = v.uv;
                return output;
            }

            fixed4 frag (v2f i) : SV_Target
            {
                //ピクセルから中心までの方向ベクトルを求める
                //ピクセルの位置-中心のXY座標
                float2 dir = i.uv - _Center.xy;

                //使用するリングが楕円形にならないように
                //アスペクト比をかけてあげる
                float aspect = _MainTex_TexelSize.z / _MainTex_TexelSize.w;
                dir.x *= aspect;

                //方向ベクトルの距離を求める
                float distance = length(dir);

                //エッジの強さの計算
                //ここで求めたいのはこのピクセルが
                //リングの上にどれだけ乗っているかという値
                //リングの中央で最大(1.0),リングの端で0.0になるようなイメージ
                float diff = abs(distance - _Radius);

                //リングからの距離を0～1の範囲に収める
                float edge = 1.0 - saturate(abs(distance - _Radius) / (_Width * 0.5));

                //正規化した方向ベクトル
                //横のピクセルに行けば行くほどoffsetが増えてしまうので
                //dir.xをoffsetで割ってdirをもとに戻しておく
                dir.x = dir.x / aspect;
                float2 normDir = normalize(dir);

                //歪みがかかった時の歪み量
                float2 offset = normDir * _DistortionStrength * edge;

                //もともとのピクセルに歪み量を足して、その色を返す
                return tex2D(_MainTex,i.uv + offset);
            }
            ENDCG
        }
    }
}
