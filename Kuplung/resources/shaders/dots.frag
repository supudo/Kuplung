out vec4 fragColor;

void main(void) {
    fragColor = vec4(1.0, 0.0, 0.0, 1.0);

//    vec2 pos = mod(gl_FragCoord.xy, vec2(50.0)) - vec2(25.0);
//    float dist_squared = dot(pos, pos);
//
//    fragColor = (dist_squared < 400.0)
//        ? vec4(.90, .90, .90, 1.0)
//        : vec4(.20, .20, .40, 1.0);
}
