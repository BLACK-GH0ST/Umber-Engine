//Color the grid Axis in the empty map s
	const char* g_VS =
  R"( cbuffer VSConstants : register(b0)
{
	float4x4 gViewProj;
};
struct VSIn { float3 pos : POSITION; float4 col : COLOR; };
struct VSOut { float4 pos : SV_Position; float4 col : COLOR; };
VSOut main(VSIn i) {
	VSOut o;
	o.pos = mul(float4(i.pos, 1.0f), gViewProj);
	o.col = i.col;
	return o;
}
)";

	const char* g_PS =
  R"(
struct PSIn { float4 pos : SV_Position; float4 col : COLOR; };
float4 main(PSIn i) : SV_Target {
	return i.col;
}
)";
}