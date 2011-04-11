// seashell inspired by http://www.generativeart.com/on/cic/papers2005/37.SemraArslanSelcuk.htm
cyl_r=5;
cyl_thk=.1;
cyl_fn=10;

shell_rotations=5;
shell_rotation_steps=30;
shell_flare=1.3;
shell_spire=.5;
shell_start_radius=1;

//z
color([.1,.1,.5])
cylinder(r=.1,h=7.5);
// x

color([.5,.1,.1])
rotate([0,90,0]) cylinder(r=.1,h=2.5);
// y

color([.1,.5,.1])
rotate([90,0]) cylinder(r=.1,h=5);

module segment(angle,radius,z){
	translate([0,0,z])
	rotate([90,0,angle])
    translate([radius,0,0])
   
	difference() {
		cylinder(h=cyl_thk,r=radius,center=true,$fn=cyl_fn);
		cylinder(h=cyl_thk*2,r=radius-cyl_thk,center=true,$fn=cyl_fn);
     }
}
module shell(){
 for (i=[0:shell_rotation_steps*shell_rotations]){
  assign(rotpercent=(i/shell_rotation_steps)){
  assign( angle=rotpercent*360,
	r=shell_start_radius*pow(shell_flare,1+rotpercent),
	z=2*shell_start_radius*pow(1+shell_spire,1+rotpercent)){
   segment(angle,r,z);
  }
  }
 }
}

shell();

//segment(0,5,0);
//segment(0,5,4);
