#version 410 core

// Clouds by iq: https://www.shadertoy.com/view/XslGRr
// Cloud Ten by nimitz: https://www.shadertoy.com/view/XtS3DD

uniform vec3 fs_screenResolution;
uniform float fs_deltaRunningTime;
uniform vec4 fs_mouseCoordinates;
uniform sampler2D fs_noiseTextureSampler;

in vec4 glFragCoord;
out vec4 fragColor;

#define SAMPLE_COUNT 32
#define PERIOD 1.

// mouse toggle
bool STRUCTURED = true;

// cam moving in a straight line
vec3 lookDir = vec3(cos(0.53 * fs_deltaRunningTime), 0.0, sin(fs_deltaRunningTime));
vec3 camVel = vec3(-20.0, 0.0, 0.0);
float zoom = 1.2; // 1.5;

vec3 sundir = normalize(vec3(-1.0, 0.0, -1.0));

// LUT based 3d value noise
float noise(in vec3 x) {
    vec3 p = floor(x);
    vec3 f = fract(x);
    f = f * f * (3.0 - 2.0 * f);

    vec2 uv = (p.xy + vec2(37.0, 17.0) * p.z) + f.xy;
    vec2 rg = texture(fs_noiseTextureSampler, (uv + 0.5) / 256.0, -100.0).yx;
    return mix(rg.x, rg.y, f.z);
}

vec4 map(in vec3 p) {
    float d = 0.1 + 0.8 * sin(0.6 * p.z) * sin(0.5 * p.x) - p.y;

    vec3 q = p;
    float f;

    f  = 0.5000 * noise(q); q = q * 2.02;
    f += 0.2500 * noise(q); q = q * 2.03;
    f += 0.1250 * noise(q); q = q * 2.01;
    f += 0.0625 * noise(q);
    d += 2.75 * f;

    d = clamp(d, 0.0, 1.0);

    vec4 res = vec4(d);

    vec3 col = 1.15 * vec3(1.0, 0.95, 0.8);
    col += vec3(1.0, 0.0, 0.0) * exp2(res.x * 10.0 - 10.0);
    res.xyz = mix(col, vec3(0.7, 0.7, 0.7), res.x);

    return res;
}

float mysign(float x) { return x < 0.0 ? -1.0 : 1.0 ; }
vec2 mysign(vec2 x) { return vec2(x.x < 0.0 ? -1.0 : 1.0, x.y < 0.0 ? -1.0 : 1.0) ; }

// compute ray march start offset and ray march step delta and blend weight for the current ray
void SetupSampling(out vec2 t, out vec2 dt, out vec2 wt, in vec3 ro, in vec3 rd) {
    if (!STRUCTURED) {
        dt = vec2(PERIOD, PERIOD);
        t = dt;
        wt = vec2(0.5, 0.5);
        return;
    }

    // the following code computes intersections between the current ray, and a set
    // of (possibly) stationary sample planes.

    // much of this should be more at home on the CPU or in a VS.

    // structured sampling pattern line normals
    vec3 n0 = (abs(rd.x) > abs(rd.z)) ? vec3(1.0, 0.0, 0.0) : vec3(0.0, 0.0, 1.0); // non diagonal
    vec3 n1 = vec3(mysign(rd.x * rd.z), 0.0, 1.0); // diagonal

    // normal lengths (used later)
    vec2 ln = vec2(length(n0), length(n1));
    n0 /= ln.x;
    n1 /= ln.y;

    // some useful DPs
    vec2 ndotro = vec2(dot(ro, n0), dot(ro, n1));
    vec2 ndotrd = vec2(dot(rd, n0), dot(rd, n1));

    // step size
    vec2 period = ln * PERIOD;
    dt = period / abs(ndotrd);

    // dist to line through origin
    vec2 dist = abs(ndotro / ndotrd);

    // raymarch start offset - skips leftover bit to get from ro to first strata lines
    t = -mysign(ndotrd) * mod(ndotro, period) / abs(ndotrd);
    if (ndotrd.x > 0.0)
        t.x += dt.x;
    if (ndotrd.y > 0.0)
        t.y += dt.y;

    // sample weights
    float minperiod = PERIOD;
    float maxperiod = sqrt(2.0) * PERIOD;
    wt = smoothstep(maxperiod, minperiod, dt / ln);
    wt /= (wt.x + wt.y);
}

vec4 raymarch(in vec3 ro, in vec3 rd) {
    vec4 sum = vec4(0, 0, 0, 0);

    // setup sampling - compute intersection of ray with 2 sets of planes
    vec2 t, dt, wt;
    SetupSampling(t, dt, wt, ro, rd);

    // fade samples at far extent
    float f = 0.6; // magic number - TODO justify this
    float endFade = f * float(SAMPLE_COUNT) * PERIOD;
    float startFade = 0.8 * endFade;

    for (int i=0; i<SAMPLE_COUNT; i++) {
        if (sum.a > 0.99)
            continue;

        // data for next sample
        vec4 data = t.x < t.y ? vec4(t.x, wt.x, dt.x, 0.0) : vec4(t.y, wt.y, 0.0, dt.y);
        // somewhat similar to: https://www.shadertoy.com/view/4dX3zl
        //vec4 data = mix(vec4(t.x, wt.x, dt.x, 0.0), vec4(t.y, wt.y, 0.0, dt.y), float(t.x > t.y));
        vec3 pos = ro + data.x * rd;
        float w = data.y;
        t += data.zw;

        // fade samples at far extent
        w *= smoothstep(endFade, startFade, data.x);

        vec4 col = map(pos);

        // iqs goodness
        float dif = clamp((col.w - map(pos + 0.6 * sundir).w) / 0.6, 0.0, 1.0);
        vec3 lin = vec3(0.51, 0.53, 0.63) * 1.35 + 0.55 * vec3(0.85, 0.57, 0.3) * dif;
        col.xyz *= lin;

        col.xyz *= col.xyz;

        col.a *= 0.75;
        col.rgb *= col.a;

        // integrate
        sum += col * (1.0 - sum.a) * w;
    }

    sum.xyz /= (0.001 + sum.w);

    return clamp(sum, 0.0, 1.0);
}

vec3 sky(vec3 rd) {
    vec3 col = vec3(0.0);

    float hort = 1.0 - clamp(abs(rd.y), 0.0, 1.0);
    col += 0.5 * vec3(0.99, 0.5, 0.0) * exp2(hort * 8.0 - 8.0);
    col += 0.1 * vec3(0.5, 0.9, 1.0) * exp2(hort * 3.0 -3.0);
    col += 0.55 * vec3(0.6, 0.6, 0.9);

    float sun = clamp(dot(sundir,rd), 0.0, 1.0);
    col += 0.2 * vec3(1.0, 0.3, 0.2) * pow(sun, 2.0);
    col += 0.5 * vec3(1.0, 0.9, 0.9) * exp2(sun * 650.0 - 650.0);
    col += 0.1 * vec3(1.0, 1.0, 0.1) * exp2(sun * 100.0 - 100.0);
    col += 0.3 * vec3(1.0, 0.7, 0.0) * exp2(sun * 50.0 - 50.0);
    col += 0.5 * vec3(1.0, 0.3, 0.05) * exp2(sun * 10.0 - 10.0);

    float ax = atan(rd.y, length(rd.xz)) / 1.0;
    float ay = atan(rd.z, rd.x) / 2.0;
    float st = texture(fs_noiseTextureSampler, vec2(ax, ay)).x;
    float st2 = texture(fs_noiseTextureSampler, 0.25 * vec2(ax, ay)).x;
    st *= st2;
    st = smoothstep(0.65, 0.9, st);
    col = mix(col, col + 1.8 * st, clamp(1.0 -1.1 * length(col), 0.0, 1.0));

    return col;
}

void mainImage(out vec4 frag_Color, in vec2 fragCoord) {
    // click mouse to use naive raymarching
    if (fs_mouseCoordinates.z > 0.0)
        STRUCTURED = false;

    vec2 q = fragCoord.xy / fs_screenResolution.xy;
    vec2 p = -1.0 + 2.0 * q;
    p.x *= fs_screenResolution.x / fs_screenResolution.y;
    vec2 mo = -1.0 + 2.0 * fs_mouseCoordinates.xy / fs_screenResolution.xy;

    // camera
    vec3 ro = vec3(0.0, 1.5, 0.0) + fs_deltaRunningTime * camVel;
    vec3 ta = ro + lookDir; //vec3(ro.x, ro.y, ro.z-1.);
    vec3 ww = normalize(ta - ro);
    vec3 uu = normalize(cross(vec3(0.0, 1.0, 0.0), ww));
    vec3 vv = normalize(cross(ww, uu));
    float fov = 1.;
    vec3 rd = normalize(fov * p.x * uu + fov * 1.2 * p.y * vv + 1.5 * ww);

    // divide by forward component to get fixed z layout instead of fixed dist layout
    //vec3 rd_layout = rd / mix(dot(rd, ww), 1.0, samplesCurvature);
    vec4 clouds = raymarch(ro, rd);

    vec3 col = clouds.xyz;

    // sky if visible
    if (clouds.w <= 0.99)
      col = mix(sky(rd), col, clouds.w);

    col = clamp(col, 0.0, 1.0);
    col = smoothstep(0.0, 1.0, col);
    col *= pow(16.0 * q.x * q.y * (1.0 - q.x) * (1.0 - q.y), 0.12); //Vign

    frag_Color = vec4(col, 1.0);
}

void main(void) {
    vec4 color = vec4(0.0, 0.0, 0.0, 1.0);
    mainImage(color, glFragCoord.xy);
    fragColor = color;
}
