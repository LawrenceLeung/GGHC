// Gramaphone like surface

sphere_w=1;
sphere_inner_w=.98;
sphere_cnt=6;
sphere_offset=.7;
$fs=.1;


//z
color([.1,.1,.5])
cylinder(r=.1,h=7.5);
// x

color([.5,.1,.1])
rotate([0,90,0]) cylinder(r=.1,h=2.5);
// y

color([.1,.5,.1])
rotate([90,0]) cylinder(r=.1,h=5);



module outer(w){
 for(i=[0:sphere_cnt]){
  rotate([0,0,i*360/sphere_cnt])
  translate([w*sphere_offset,0,0])
  sphere(w);
 }
}

module test(){
scale([1,1,2])
difference(){
 outer(sphere_w);
 translate([0,0,.5]) scale([1.1,1.1,1])
 outer(sphere_w*sphere_inner_w);
}
 translate([0,0,-3.5])
 cylinder(r1=.5,r2=1.5,h=3);
}

mic_h=1.7;
mic_r1=.2;
mic_r2=.5;
mic_rot=[0,-55,0];

speak_h=3;
speak_r1=.3;
speak_r2=2;
speak_rot=[0,45,0];
base_x=1.7;
base_y=2;
baze_z=.6;
base_trans=[.3,0,0];

module test2(){
difference(){
  union(){
  rotate(mic_rot)
  cylinder(r1=mic_r1, r2=mic_r2,h=mic_h);
  rotate(speak_rot)
  cylinder(r1=speak_r1, r2=speak_r2,h=speak_h);
  translate(base_trans)
  cube([base_x,base_y,baze_z],center=true);
  }

  union(){ 
   rotate(mic_rot)
    cylinder(r1=mic_r1*sphere_inner_w, r2=mic_r2*sphere_inner_w,h=mic_h+1);
  rotate(speak_rot)
  cylinder(r1=speak_r1*sphere_inner_w, r2=speak_r2*sphere_inner_w,h=speak_h+1);
  translate(base_trans)
  cube([base_x*sphere_inner_w,base_y*sphere_inner_w,baze_z*sphere_inner_w],center=true);
  }
}
}

test2();


