/* The loader is part of the loader bin, which basically
 * sets a global object of name "ender"
 */

const enesim = ender.enesim;

enesim.init();
var r = new enesim.renderer.checker();
print("r = " + r.getName());

r.evenColor = 0xff000000;
r.oddColor = 0xffff0000;
r.width = 50;
r.height = 50;

var s = new enesim.surface(enesim.format.ARGB8888, 320, 240);
//print("s = " + s);

var b = s.getBuffer();
//print("b = " + b);

r.draw(s, enesim.rop.FILL, null, 0, 0, null);
enesim.image.file.save("out.png", b,  null, null);
enesim.shutdown();
