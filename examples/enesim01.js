/* The loader is part of the loader bin, which basically
 * sets a global object of name "ender"
 */

const enesim = ender.enesim;

enesim.init();
var r = new enesim.renderer.checker();
r.evenColor = 0xffffff;
enesim.shutdown();
