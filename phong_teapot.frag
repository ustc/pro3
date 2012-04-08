varying vec3 ec_vnormal, ec_vposition, ec_vtangent, ec_vbitangent;
uniform sampler2D mytexture;
uniform sampler2D mynormalmap;

void main() {
  mat3 tform;
  vec3 P, N, L, V, H, mapN, tcolor;
  vec4 diffuse_color, specular_color;
  float shininess = gl_FrontMaterial.shininess;
  float pi = 3.14159;
  tform = mat3(normalize(ec_vtangent), normalize(ec_vbitangent),
               normalize(ec_vnormal));
  P = ec_vposition;
  L = normalize(gl_LightSource[0].position.xyz - P);
  V = normalize(-P);
  H = normalize(L+V);
  mapN = normalize(2.0*vec3(texture2D(mynormalmap, gl_TexCoord[0].st))
                   -vec3(1.0, 1.0, 1.0));
  N = normalize(tform*mapN);
  //N = normalize(ec_vnormal);
  tcolor = vec3(texture2D(mytexture, gl_TexCoord[0].st));
  diffuse_color = vec4(tcolor, 1.0)*max(dot(N, L), 0.0);
  specular_color = gl_FrontMaterial.specular*pow(max(dot(H,N),0.0),shininess);
  gl_FragColor = diffuse_color + specular_color;
}
