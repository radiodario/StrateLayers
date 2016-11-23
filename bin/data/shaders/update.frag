#version 330

#define SPEED_OF_SOUND 50
#define TWO_PI 6.28318530718

// ping pong inputs
uniform sampler2DRect particles0;
uniform sampler2DRect particles1;
uniform sampler2DRect particles2;

uniform vec2 resolution;
uniform vec3 mouse;
uniform float elapsed;
uniform float time;
uniform float damping;
uniform vec2 timeStep;
uniform vec2 dimDiv;
uniform vec2 mults;

uniform float forceGain;

in vec2 texCoordVarying;

layout(location = 0) out vec4 posOut;
layout(location = 1) out vec4 velOut;
layout(location = 2) out vec4 forceOut;

float rand(float n){return fract(sin(n) * 43758.5453123);}

// Simplex 2D noise
//
//vec3 permute(vec3 x) { return mod(((x*34.0)+1.0)*x, 289.0); }

// Modulo 289 without a division (only multiplications)
vec3 mod289(vec3 x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec2 mod289(vec2 x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

// Modulo 7 without a division
vec3 mod7(vec3 x) {
  return x - floor(x * (1.0 / 7.0)) * 7.0;
}

// Permutation polynomial: (34x^2 + x) mod 289
vec3 permute(vec3 x) {
  return mod289((34.0 * x + 1.0) * x);
}

float snoise(vec2 v){
  const vec4 C = vec4(0.211324865405187, 0.366025403784439,
           -0.577350269189626, 0.024390243902439);
  vec2 i  = floor(v + dot(v, C.yy) );
  vec2 x0 = v -   i + dot(i, C.xx);
  vec2 i1;
  i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
  vec4 x12 = x0.xyxy + C.xxzz;
  x12.xy -= i1;
  i = mod(i, 289.0);
  vec3 p = permute( permute( i.y + vec3(0.0, i1.y, 1.0 ))
  + i.x + vec3(0.0, i1.x, 1.0 ));
  vec3 m = max(0.5 - vec3(dot(x0,x0), dot(x12.xy,x12.xy),
    dot(x12.zw,x12.zw)), 0.0);
  m = m*m ;
  m = m*m ;
  vec3 x = 2.0 * fract(p * C.www) - 1.0;
  vec3 h = abs(x) - 0.5;
  vec3 ox = floor(x + 0.5);
  vec3 a0 = x - ox;
  m *= 1.79284291400159 - 0.85373472095314 * ( a0*a0 + h*h );
  vec3 g;
  g.x  = a0.x  * x0.x  + h.x  * x0.y;
  g.yz = a0.yz * x12.xz + h.yz * x12.yw;
  return 130.0 * dot(m, g);
}

#define M_PI 3.14159265358979323846

float rand(vec2 co){return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);}
float rand (vec2 co, float l) {return rand(vec2(rand(co), l));}
float rand (vec2 co, float l, float t) {return rand(vec2(rand(co, l), t));}

float perlin(vec2 p, float dim, float time) {
    vec2 pos = floor(p * dim);
    vec2 posx = pos + vec2(1.0, 0.0);
    vec2 posy = pos + vec2(0.0, 1.0);
    vec2 posxy = pos + vec2(1.0);

    float c = rand(pos, dim, time);
    float cx = rand(posx, dim, time);
    float cy = rand(posy, dim, time);
    float cxy = rand(posxy, dim, time);

    vec2 d = fract(p * dim);
    d = -0.5 * cos(d * M_PI) + 0.5;

    float ccx = mix(c, cx, d.x);
    float cycxy = mix(cy, cxy, d.x);
    float center = mix(ccx, cycxy, d.y);

    return center * 2.0 - 1.0;
}

// update the force grid;
vec3 updateForce() {
  vec3 pos = texture(particles0, texCoordVarying.st).xyz;
  vec3 force = texture(particles2, texCoordVarying.st).xyz;

  vec2 p = pos.xy/dimDiv.y + vec2(time/timeStep.y, 0);
  p *= mults.y;
  float fy = .5*snoise(p)
    +.25*snoise(2.*p)
    +.125*snoise(4.*p)
    +.0625*snoise(8.*p)
    +.03125*snoise(16.*p)
    +.015*snoise(32.*p);

  p = pos.xy/dimDiv.x + vec2(0, time/timeStep.x);
  p *= mults.x;
  float fx = .5*snoise(p)
    +.25*snoise(2.*p)
    +.125*snoise(4.*p)
    +.0625*snoise(8.*p)
    +.03125*snoise(16.*p)
    +.015*snoise(32.*p);

  force = vec3(fx, fy, 0);
  force *= forceGain;

  return force;
}

vec3 getForceForParticle(vec3 pos) {
  vec2 forceIndex = vec2(
      1000 * (pos.x + resolution.x * 0.5)/resolution.x,// * 0.5 + 0.5; // texture coordinates basically
      1000 * (pos.y + resolution.x * 0.5)/resolution.x
    );
  return texture(particles2, forceIndex).xyz;
}

void main()
{

    vec4 pos = texture(particles0, texCoordVarying.st);
    vec3 vel = texture(particles1, texCoordVarying.st).xyz;
    vec3 force = getForceForParticle(pos.xyz);
    //vec3 force = vec3(0.);

    // accelerate
    vel += elapsed * force;

    // bounce off the sides
    vel.x *= step(abs(pos.x), resolution.x/2) * 2.0 - 1.0;
    vel.y *= step(abs(pos.y), resolution.y/2) * 2.0 - 1.0;

    // damping
    vel *= 1 - damping;

    // move
    pos.xyz += elapsed * vel;

    //pos.x += force.x;
    //pos.y += force.y;

    posOut = pos;
    velOut = vec4(vel, 0.0);
    forceOut = vec4(updateForce(), 0.0); //vec4(updateForce(force.xy, fidx), 0.0);
}



