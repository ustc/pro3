varying vec3 ec_vnormal, ec_vposition, ec_vtangent, ec_vbitangent;
attribute vec3 tangent;
attribute vec3 bitangent;

void main()
{
	ec_vnormal = gl_NormalMatrix*gl_Normal;
	ec_vposition = (gl_ModelViewMatrix*gl_Vertex).xyz;
	gl_Position = gl_ProjectionMatrix*gl_ModelViewMatrix*gl_Vertex;
	gl_TexCoord[0] = gl_MultiTexCoord0;
  	ec_vtangent = gl_NormalMatrix*tangent;
  	ec_vbitangent = gl_NormalMatrix*bitangent;
}
