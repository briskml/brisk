open Brisk_macos;
open Brisk.Layout;

let sidebarItemStyle = [
  font(~size=15., ()),
  kern(0.09),
  align(`Left),
  color(Color.hex("#282522")),
  padding2(~h=25., ~v=4., ()),
];

let headerStyle = [
  font(~size=24., ~weight=`Semibold, ()),
  kern(0.58),
  align(`Left),
  color(Color.hex("#000000")),
  padding4(~left=4., ~top=0., ()),
  margin2(~v=10., ()),
];

let sectionStyle = [padding4(~left=4., ())];

let sectionHeaderStyle = [
  font(~size=17., ()),
  kern(0.27),
  color(Color.hex("#000000")),
  margin2(~v=9., ()),
];
